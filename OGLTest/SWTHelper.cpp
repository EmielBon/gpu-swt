//
//  StrokeWidthTransform.cpp
//  OGLTest
//
//  Created by Emiel Bon on 13-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "SWTHelper.h"
#include "Chain.h"
#include "Ray.h"
#include "Component.h"
#include "BoundingBox.h"
#include "MathHelper.h"
#include "ImgProc.h"
#include "ConnectedComponentsHelper.h"
#include "RenderWindow.h"
#include "SWTParameters.h"

List<Ray> CastRays(const cv::Mat &edgeMap, const cv::Mat &gradients, GradientDirection direction);
LinkedList< Ptr<Chain> > MakePairs(List<Ptr<Component>> &components);
cv::Mat CalculateStrokeWidths(const cv::Mat &edgeMap, const cv::Mat &gradients, GradientDirection direction);
void SubtractEdges(cv::Mat &input, const cv::Mat &edges);
float CalculateSimilarity(const Chain& c1, const Chain &c2);
bool ChainingIteration(LinkedList<Ptr<Chain>> &chains);
// Filters
void FilterOnOverlappingBoundingBoxes(List<Ptr<Component>> &components);
void FilterInvalidComponents(List< Ptr<Component> > &components, int imageSize);
void FilterNonWords(LinkedList< Ptr<Chain> > &chains);
// Drawing functions
inline void Draw(const cv::Mat &image, const String &description);
void DrawBoundingBoxes(const cv::Mat &input, const List< Ptr<Component> > &components, const String &description);
void DrawChains(const cv::Mat &input, const LinkedList< Ptr<Chain> > &components, const String &description);

List<BoundingBox> SWTHelper::StrokeWidthTransform(const cv::Mat &input)
{
    cv::Mat grayImage = ImgProc::ConvertToGrayscale(input); Draw(grayImage, "Grayscale");
#ifdef EQUALIZE_HISTOGRAM
    cv::Mat grayImage2 = grayImage.clone();
    grayImage = ImgProc::ContrastStretch(grayImage2, 0, 100);
    //cv::equalizeHist(grayImage2, grayImage);
    Draw(grayImage, "Equalized grayscale");
#endif
#ifdef SHARPEN_INPUT
    grayImage = ImgProc::Sharpen(grayImage); Draw(grayImage, "Sharpened");
#endif
    cv::Mat edges     = ImgProc::CalculateEdgeMap(grayImage); Draw(edges, "Edge map (Canny edge detector)");
    cv::Mat gradients = ImgProc::CalculateGradients(grayImage, true); Draw(gradients, "Gradients");
    
    grayImage.release();
    
    List< Ptr<Component> > components;
    
    cv::Mat swtImage1 = CalculateStrokeWidths(edges, gradients, GradientDirection::With); Draw(ImgProc::NormalizeImage(swtImage1, 1.0f, FLT_MAX, 1.0f), "SWT 1");
    //SubtractEdges(swtImage1, edges);
    cv::Mat componentMap1 = ConnectedComponentsHelper::FindComponents(swtImage1, components, FLT_MAX);
    Draw(componentMap1, "Stroke Widths 'with the gradient'");
    
    cv::Mat swtImage2 = CalculateStrokeWidths(edges, gradients, GradientDirection::Against); Draw(ImgProc::NormalizeImage(swtImage2, 1.0f, FLT_MAX, 1.0f), "SWT 2");
    //SubtractEdges(swtImage2, edges);
    cv::Mat componentMap2 = ConnectedComponentsHelper::FindComponents(swtImage2, components, FLT_MAX);
    Draw(componentMap2, "Stroke Widths 'against the gradient'");
    
    FilterInvalidComponents(components, input.rows * input.cols);
    DrawBoundingBoxes(input, components, "Valid components (letter candidates)");
    
    edges.release();
    gradients.release();
    
    FilterOnOverlappingBoundingBoxes(components);
    DrawBoundingBoxes(input, components, "Boundingboxes with overlap <= 2 (letters)");
    
    for(auto component : components)
        component->SetMeanColorFromImage(input);
    
    auto chains = MakePairs(components);
    DrawChains(input, chains, "Paired components");
    
    while(ChainingIteration(chains))
        ;
    DrawChains(input, chains, "Chains");
    
    FilterNonWords(chains);
    DrawChains(input, chains, "Chains with length >= 3 (words)");
    
    List<BoundingBox> boundingBoxes;
    
    for(auto chain : chains)
        boundingBoxes.push_back(chain->BoundingBox());
    
    return boundingBoxes;
}

inline bool IsEdgePixel(const cv::Mat &edgeMap, int x, int y)
{
    return (edgeMap.at<uchar>(y, x) == 255);
}

inline bool InRange(const cv::Mat &image, int x, int y)
{
    return (x >= 0 && x < image.cols && y >= 0 && y < image.rows);
}

void SubtractEdges(cv::Mat &input, const cv::Mat &edges)
{
    for(int i = 0; i < edges.cols; ++i)
    for(int j = 0; j < edges.rows; ++j)
    {
        if (IsEdgePixel(edges, i, j))
            input.at<float>(j, i) = FLT_MAX;
    }
}

void Draw(const cv::Mat &image, const String &description)
{
    RenderWindow::Instance().AddTexture(image, description);
}

void FilterOnOverlappingBoundingBoxes(List<Ptr<Component>> &components)
{
    List<Ptr<Component>> filtered(components.size());
    
    auto end = std::copy_if(components.begin(), components.end(), filtered.begin(), [&] (Ptr<Component> c1) {
        int overlapCount = 0;
        for(auto c2 : components)
        {
            if (c1 == c2) continue;
            if (c1->BoundingBox().Contains(c2->BoundingBox()) != ContainmentType::Disjoint)
                overlapCount++;
        }
        return (overlapCount <= 2);
    });
    
    filtered.resize(std::distance(filtered.begin(), end));
    components = filtered;
}

LinkedList< Ptr<Chain> > MakePairs(List<Ptr<Component>> &components)
{
    LinkedList< Ptr<Chain> > chains;
    
    for(auto c1 : components)
    {
        for(auto c2 : components)
        {
            if (c1 == c2)
                continue;
            if (!c1->CanLinkWith(*c2))
                continue;
            
            auto pair = Ptr<Chain>( new Chain({c1, c2}) );
            
            // Throw away non (near-)horizontal pairs
            if (acos(pair->Direction().dot( {1.0f, 0.0f} )) > MaxNullAngleDifference)
                continue;
            
            chains.push_back(pair);
        }
    }
    return chains;
}

void FilterInvalidComponents(List< Ptr<Component> > &components, int imageSize)
{
    // Remove invalid components
    auto end = std::remove_if(components.begin(), components.end(), [&] (Ptr<Component> component) {
        int componentSize = component->BoundingBox().Area();
        float sizeRatio = (float) componentSize / (float) imageSize;
        return !component->IsValid() || sizeRatio < 0.0005 || sizeRatio > 0.02;
    });
    components.resize(std::distance(components.begin(), end));
}

void FilterNonWords(LinkedList< Ptr<Chain> > &chains)
{
    auto end = std::remove_if(chains.begin(), chains.end(), [] (Ptr<Chain> chain) {
        auto chainLength = chain->Components.size();
        return (chainLength < MinChainLength || chainLength > MaxChainLength);
    });
    chains.resize(std::distance(chains.begin(), end));
}

Ptr<Component> GetSharedComponent(const Chain& chain1, const Chain& chain2)
{
    for(auto c1 : chain1.Components)
        for(auto c2 : chain2.Components)
            if (c1 == c2) return c1;
    return nullptr;
}

bool ChainingIteration(LinkedList<Ptr<Chain>> &chains)
{
    using Couple = Tuple<Ptr<Chain>, Ptr<Chain>>;
    Couple lastCouple = {nullptr, nullptr};
    
    float highestSimilarity = 0;
    
    for(auto it = chains.begin(); it != chains.end(); ++it)
    {
        for(auto it2 = std::next(it); it2 != chains.end(); ++it2)
        {
            auto c1 = *it;
            auto c2 = *it2;
            
            float similarity = CalculateSimilarity(*c1, *c2);
            
            if (similarity > highestSimilarity)
            {
                highestSimilarity = similarity;
                lastCouple = {c1, c2};
            }
        }
    }
    
    if (highestSimilarity > 0)
    {
        chains.push_back( Chain::Merge(*std::get<0>(lastCouple), *std::get<1>(lastCouple)) );
        chains.remove(std::get<0>(lastCouple));
        chains.remove(std::get<1>(lastCouple));
    }
    
    return (highestSimilarity > 0);
}

float CalculateSimilarity(const Chain &c1, const Chain &c2)
{
    auto shared = GetSharedComponent(c1, c2);
    if (!shared)
        return 0;
    
    float angle = acos(c1.Direction().dot(c2.Direction()));
    
	if (angle >= (M_PI / 8))
		return 0;
	return 1 - (angle / (M_PI / 2));
}

List<Ray> CastRays(const cv::Mat &edgeMap, const cv::Mat &gradients, GradientDirection direction)
{
    List<Ray> rays;
    
    float prec = RayStepPrecision;
    
    bool darkOnLight = (direction == GradientDirection::With);
    
    int counter = 0;
    
    for(int i = 0; i < edgeMap.cols; ++i)
    for(int j = 0; j < edgeMap.rows; ++j)
    {
        if (edgeMap.at<uchar>(j, i) == 0)
            continue;
        
        Ray ray;
        bool discard = false;
        
        cv::Vec2f dp = gradients.at<cv::Vec2f>(j, i) * (darkOnLight ? 1 : -1);
        if (MathHelper::Length(dp) < 0.5)
        {
            printf("Edge pixel without gradient %i\n", counter++);
            continue;
        }
        
        cv::Vec2f realPosition(i + 0.5f, j + 0.5f);
        Point position((int)realPosition[0], (int)realPosition[1]);
        
        ray.Points.push_back(position);
        
        while(true)
        {
            realPosition += dp * prec;
            position = cv::Vec2i((int)realPosition[0], (int)realPosition[1]);
            discard = !InRange(edgeMap, position[0], position[1]);
            if (position == ray.Points.back())
                continue;
#ifndef INCLUDE_SECOND_EDGE_PIXEL_IN_RAY
            if (discard || IsEdgePixel(edgeMap, position[0], position[1]))
                break;
#endif
            ray.Points.push_back(position);
            
#ifdef  INCLUDE_SECOND_EDGE_PIXEL_IN_RAY
            if (discard || IsEdgePixel(edgeMap, position[0], position[1]))
                break;
#endif
        }
        
        if (discard)
            continue;
        
        if (ray.Points.size() < 2)
            continue;
        
        Point &q = ray.Points.back();
        int gx = q[0];
        int gy = q[1];
        
        cv::Vec2f dq = gradients.at<cv::Vec2f>(gy, gx) * (darkOnLight ? 1 : -1);
        
        if (acos(dq.dot(-dp)) < MaxOppositeEdgeGradientDifference && ray.Length() < MaxRayLength)
        {
            rays.push_back(ray);
        }
    }
    
    return rays;
}

cv::Mat CalculateStrokeWidths(const cv::Mat &edgeMap, const cv::Mat &gradients, GradientDirection direction)
{
    auto rays = CastRays(edgeMap, gradients, direction);
    cv::Mat swtImage(edgeMap.size(), CV_32FC1, FLT_MAX);
    
    for(auto &ray : rays)
    {
        for(auto &point : ray.Points)
        {
            float &value = swtImage.at<float>(point[1], point[0]);
            value = std::min(value, ray.Length());
        }
    }
    
    for(auto &ray : rays)
    {
        List<float> swtValues;
        
        for(auto &point : ray.Points)
        {
            swtValues.push_back( swtImage.at<float>(point[1], point[0]) );
        }
        
#ifdef USE_FAKE_MEDIAN
        float median = MathHelper::Mean(swtValues);
#else
        float median = MathHelper::Median(swtValues);
#endif
        for(auto &point : ray.Points)
        {
            float &value = swtImage.at<float>(point[1], point[0]);
            value = std::min(value, median);
        }
    }
    
    return swtImage;
}

void DrawBoundingBoxes(const cv::Mat &input, const List< Ptr<Component> > &components, const String &description)
{
    cv::Mat bboxImage = input.clone();
    for(auto component : components)
        cv::rectangle(bboxImage, component->BoundingBox().Bounds, {0, 255, 255, 255});
    Draw(bboxImage, description);
}

void DrawChains(const cv::Mat &input, const LinkedList< Ptr<Chain> > &chains, const String &description)
{
    cv::Mat lineImage = input.clone();
    for(auto chain : chains)
    {
        for(auto it = chain->Components.begin(); it != chain->Components.end(); ++it)
        {
            auto c1 = *it;
            
            cv::rectangle(lineImage, c1->BoundingBox().Bounds, {0, 255, 255, 255});
            
            if (std::next(it) == chain->Components.end())
                break;
            
            auto c2 = *std::next(it);
            
            cv::line(lineImage, c1->BoundingBox().Center(), c2->BoundingBox().Center(), {0, 255, 0, 255}, 1);
        }
    }
    Draw(lineImage, description);
}
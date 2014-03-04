//
//  Renderer.cpp
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "RenderWindow.h"
#include "ContentLoader.h"
#include "Program.h"
#include "DrawableRect.h"
#include "Texture.h"
#include "Image.h"
#include "Ray.h"
#include "ImgProc.h"
#include "MathHelper.h"
#include "Component.h"
#include "Pair.h"
#include "Chain.h"
#include <opencv2/imgproc/imgproc.hpp>

#include "types.h"
#include <stdio.h>
#include <algorithm>
#include <assert.h>

#define USE_MEDIAN_APPROXIMATION 1;

RenderWindow::RenderWindow(int width, int height, const String &title)
    : base(width, height, title), device(GraphicsDevice)
{
    ContentLoader::SetContentPath("/Users/emiel/Desktop/OGLTest/OGLTest/");
    rect1 = New<DrawableRect>(-1.0f, -1.0f, 1.0f, 1.0f);
    currentTextureIndex = 0;
    
    // Load the input image as a cv::Mat
    cv::Mat input = ContentLoader::LoadV<cv::Mat>("sign800x600");
    glfwSetWindowSize(window, input.size().width, input.size().height);
    
    AddTexture(input, "Input image");
    // Load the shader program
    program = ContentLoader::Load<Program>("SimpleShader");
    
    // Calculate edge map, gradientX and gradientY
    cv::Mat grayImage = ImgProc::ConvertToGrayscale(input); AddTexture(grayImage, "Grayscale");
    cv::Mat sharper   = ImgProc::Sharpen(grayImage); AddTexture(sharper, "Sharpened");
    
    cv::Mat edgeMap   = ImgProc::CalculateEdgeMap(grayImage); AddTexture(edgeMap, "Edge map (Canny edge detector)");
    cv::Mat gradients = ImgProc::CalculateGradients(grayImage, true); AddTexture(gradients, "Gradients");
    
    grayImage.release();
    sharper.release();
    
    // Perform the Stroke Width Transform operation
    List<Component> components;
    List<Component> components1;
    cv::Mat swtImage1 = StrokeWidthTransform(edgeMap, gradients, true);
    cv::Mat componentMap1 = FindComponents(swtImage1, components1, FLT_MAX); AddTexture(componentMap1, "Components 'with the gradient'");
    for(auto &component : components1)
    {
        cv::rectangle(componentMap1, component.BoundingBox().Bounds, {0, 1, 1, 1});
        components.push_back(component);
    }
    AddTexture(componentMap1, "Component bounding boxes 'with the gradient'");
    
    List<Component> components2;
    // Perform the Stroke Width Transform operation
    cv::Mat swtImage2 = StrokeWidthTransform(edgeMap, gradients, false);
    cv::Mat componentMap2 = FindComponents(swtImage2, components2, FLT_MAX); AddTexture(componentMap2, "Components 'against the gradient'");
    for(auto &component : components2)
    {
        cv::rectangle(componentMap2, component.BoundingBox().Bounds, {0, 1, 1, 1});
        components.push_back(component);
    }
    AddTexture(componentMap2, "Component bounding boxes 'against the gradient'");
    
    List<Component> bboxFilteredComponents;
    for(auto &component1 : components)
    {
        int overlapCount = 0;
        for(auto &component2 : components)
        {
            if (&component1 == &component2) continue;
            if (component1.BoundingBox().Contains(component2.BoundingBox()) == ContainmentType::Contains)
            {
                overlapCount++;
            }
        }
        if (overlapCount <= 2)
            bboxFilteredComponents.push_back(component1);
    }
    
    cv::Mat bboxImage = input.clone();
    
    for(auto &component : bboxFilteredComponents)
    {
        cv::rectangle(bboxImage, component.BoundingBox().Bounds, {0, 255, 255, 255});
    }
    AddTexture(bboxImage, "Bounding box filtered");
    
    List<Pair> pairs;
    for(auto &component1 : bboxFilteredComponents)
    {
        for(auto &component2 : bboxFilteredComponents)
        {
            if (&component1 == &component2) continue;
            if (MathHelper::GetRatio(component1.Mean(), component2.Mean()) > 2.0) continue;
            if (MathHelper::GetRatio(component1.BoundingBox().Height(), component2.BoundingBox().Height()) > 2.0) continue;
            if (MathHelper::Length((component1.BoundingBox().Center() - component2.BoundingBox().Center())) > 3 * std::max(component1.BoundingBox().Width(), component2.BoundingBox().Width())) continue;
            
            // todo: test for about same color with Delta-E
            pairs.push_back( {&component1, &component2} );
        }
    }
    
    cv::Mat lineImage = bboxImage.clone();
    
    LinkedList< Ptr<Chain> > chains;
    for(auto &pair : pairs)
    {
        cv::line(lineImage, pair.Component1->BoundingBox().Center(), pair.Component2->BoundingBox().Center(), {0, 255, 0, 255}, 1);
        chains.push_back( New<Chain>(pair) );
    }
    
    AddTexture(lineImage, "Component pairing");
    
    while(ChainingIteration(chains))
        ;
    
    cv::Mat chainImage = bboxImage.clone();
    
    for(auto chain : chains)
    {
        for(auto it = chain->Components.begin(); it != std::prev(chain->Components.end()); ++it)
        {
            auto c1 = *it;
            auto c2 = *std::next(it);
            cv::line(chainImage, c1->BoundingBox().Center(), c2->BoundingBox().Center(), {0, 255, 0, 255}, 1);
        }
    }
    
    AddTexture(chainImage, "End result chains");
    
    printf("%lu\n", chains.size());
}

Component* RenderWindow::GetSharedComponent(const Chain& chain1, const Chain& chain2)
{
    for(auto c1 : chain1.Components)
        for(auto c2 : chain2.Components)
            if (c1 == c2) return c1;
    
    return nullptr;
}

bool RenderWindow::ChainingIteration(LinkedList<Ptr<Chain>> &chains)
{
    using Couple = Tuple<Ptr<Chain>, Ptr<Chain>>;
    Couple lastCouple = {nullptr, nullptr};
    
    bool found = false;
    
    for(auto it = chains.begin(); it != chains.end() && !found; ++it)
    {
        for(auto it2 = std::next(it); it2 != chains.end() && !found; ++it2)
        {
            auto c1 = *it;
            auto c2 = *it2;
            auto shared = GetSharedComponent(*c1, *c2);
            if (!shared) continue;
            float dangle = fabs(c1->PolarAngle() - c2->PolarAngle());
            if (dangle > 3.14 / 16 && dangle < 3.14 - 3.14 / 16) continue;
            lastCouple = {c1, c2};
            found = true;
        }
    }
    
    if (found)
    {
        chains.push_back( Chain::Merge(*std::get<0>(lastCouple), *std::get<1>(lastCouple)) );
        chains.remove(std::get<0>(lastCouple));
        chains.remove(std::get<1>(lastCouple));
    }
    
    return found;
}

cv::Mat RenderWindow::StrokeWidthTransform(const cv::Mat &edgeMap, const cv::Mat &gradients, bool lightOnDark)
{
    auto rays = CastRays(edgeMap, gradients, lightOnDark);
    cv::Mat swtImage(edgeMap.size(), CV_32FC1, FLT_MAX);
    CalculateStrokeWidths(swtImage, *rays);
    AddTexture( ImgProc::NormalizeImage(swtImage, 0.9f, FLT_MAX, 1.0f) );
    return swtImage;
}

Ptr< List<Ray> > RenderWindow::CastRays(const cv::Mat &edgeMap, const cv::Mat &gradients, bool darkOnLight) const
{
    auto rays = New<List<Ray>>();
    
    float prec = 0.1;
    
    for(int i = 0; i < edgeMap.cols; ++i)
    for(int j = 0; j < edgeMap.rows; ++j)
    {
        if (edgeMap.at<uchar>(j, i) == 0)
            continue;
        
        Ray ray;
        bool discard = false;
        
        cv::Vec2f dp = gradients.at<cv::Vec2f>(j-1, i-1) * (darkOnLight ? 1 : -1);
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
            ray.Points.push_back(position);
            if (discard || IsEdgePixel(edgeMap, position[0], position[1]))
                break;
        }
        
        if (discard)
            continue;
        
        Point &q = ray.Points.back();
        int gx = q[0] - 1;
        int gy = q[1] - 1;
        
        cv::Vec2f dq = gradients.at<cv::Vec2f>(gy, gx) * (darkOnLight ? 1 : -1);
        
        if (dq.dot(-dp) >= 0/*1/3.0f && ray.Length() < 100*/)
        {
            rays->push_back(ray);
        }
    }
    
    return rays;
}

void RenderWindow::CalculateStrokeWidths(cv::Mat &swtImage, const List<Ray> rays) const
{
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
        
#ifdef USE_MEDIAN_APPROXIMATION
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

}

void RenderWindow::DrawRect(const DrawableRect &rect)
{
    device.VertexBuffer = rect.VertexBuffer;
    device.IndexBuffer  = rect.IndexBuffer;
    
    auto &texture = *textures[currentTextureIndex];
    
    program->Apply();
    program->Uniforms["Texture"].SetValue(texture);
    program->Uniforms["Channels"].SetValue(texture.GetColorChannels());
    
    device.DrawPrimitives(PrimitiveType::TriangleList);
}

void RenderWindow::Draw()
{
    static bool keyPressed = false;
    
    if ( glfwGetKey(window, GLFW_KEY_RIGHT) && !keyPressed)
    {
        currentTextureIndex = (currentTextureIndex + 1) % textures.size();
        keyPressed = true;
        glfwSetWindowTitle(window, textureDescriptors[currentTextureIndex].c_str());
    }
    if ( glfwGetKey(window, GLFW_KEY_LEFT) && !keyPressed)
    {
        currentTextureIndex--;
        if (currentTextureIndex < 0)
            currentTextureIndex += textures.size();
        keyPressed = true;
        glfwSetWindowTitle(window, textureDescriptors[currentTextureIndex].c_str());
    }
    if (!glfwGetKey(window, GLFW_KEY_RIGHT) && !glfwGetKey(window, GLFW_KEY_LEFT))
    {
        keyPressed = false;
    }
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    DrawRect(*rect1);
    
    base::Draw();
}

cv::Mat RenderWindow::FindComponents(const cv::Mat &swtImage, List<Component> &components, float bgValue) const
{
    cv::Mat componentMap(swtImage.size(), CV_32FC1, 0.0f);
    
    int current_label = 0;
    for (int i = 0; i < swtImage.size().width;  ++i)
    for (int j = 0; j < swtImage.size().height; ++j)
    {
        Stack<Point> stack;
        
        if (IsDiscovered(componentMap, i, j) || IsBackground(swtImage, i, j, bgValue))
            continue;
        
        stack.push(Point(i, j));
        current_label++;
        Component currentComponent;
        
        // Depth First Search. Stack invariant: Every point on the stack is a valid undiscovered point
        while(!stack.empty())
        {
            Point &p1 = stack.top();
            int x = p1[0], y = p1[1];
            stack.pop();
            // Mark the current cell
            componentMap.at<float>(y, x) = current_label;
            currentComponent.AddPoint( SWTPoint(x, y, swtImage.at<float>(y, x)) );
            // Discover neighbours
            for(int dx = -1; dx <= 1; ++dx)
            for(int dy = -1; dy <= 1; ++dy)
            {
                if(dx == 0 && dy == 0) continue;
                Point p2 = Point(x + dx, y + dy);
                if (IsNeighbour(p1, p2, swtImage, componentMap, bgValue))
                    stack.push(p2);
            }
        }
        
        if (IsValidComponent(currentComponent))
            components.push_back(currentComponent);
    }
    
    List<cv::Vec3f> colors;
    colors.push_back(cv::Vec3f(0, 0, 0)); // bg color
    for(int i = 1; i < current_label; ++i)
    {
        float r = (rand() % 255) / 255.0f;
        float g = (rand() % 255) / 255.0f;
        float b = (rand() % 255) / 255.0f;
        colors.push_back(cv::Vec3f(r, g, b));
    }
    
    cv::Mat colorComponents(swtImage.size(), CV_32FC3);
    
    for (int i = 0; i < componentMap.size().width;  ++i)
    for (int j = 0; j < componentMap.size().height; ++j)
    {
        float value = componentMap.at<float>(j, i);
        colorComponents.at<cv::Vec3f>(j, i) = colors[(int)value];
    }
    
    return colorComponents;
}

bool RenderWindow::IsValidComponent(Component &component) const
{
    auto &bbox = component.BoundingBox().Bounds;
    int width = bbox.width;
    int height = bbox.height;
    float aspectRatio = (float)width / height;
    float mean = component.Mean();
    float variance = component.Variance();
    // todo: width > 10 in original paper, have to make some sort of scale
    return (width > 5 && width < 300 && height > 10 && height < 300 && aspectRatio > 0.1 && aspectRatio < 10 && variance <= mean / 2);
}

bool RenderWindow::IsNeighbour(const Point &p1, const Point &p2, const cv::Mat &swtImage, const cv::Mat &componentMap, float bgValue) const
{
    int x1 = p1[0], y1 = p1[1];
    int x2 = p2[0], y2 = p2[1];
    
    float swt1 = swtImage.at<float>(y1, x1);
    float swt2 = swtImage.at<float>(y2, x2);
    
    if (x2 < 0 || x2 >= swtImage.cols) return false; // out of bounds
    if (y2 < 0 || y2 >= swtImage.rows) return false; // out of bounds
    if (MathHelper::GetRatio(swt1, swt2) > 3.0f) return false; // SWT ratio too large
    if (IsBackground(swtImage, x2, y2, bgValue)) return false;
    if (IsDiscovered(componentMap, x2, y2)) return false;
    
    return true;
}

bool RenderWindow::IsDiscovered(const cv::Mat &componentMap, int x, int y) const
{
    return componentMap.at<float>(y, x) != 0.0f;
}

bool RenderWindow::IsBackground(const cv::Mat &swtImage, int x, int y, float bgValue) const
{
    return swtImage.at<float>(y, x) == bgValue;
}

void RenderWindow::AddTexture(cv::Mat mat, const String &descriptor)
{
    textures.push_back( New<Texture>(mat) );
    textureDescriptors.push_back(descriptor);
}
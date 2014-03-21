//
//  ConnectedComponentsHelper.cpp
//  OGLTest
//
//  Created by Emiel Bon on 06-03-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "ConnectedComponentsHelper.h"
#include "MathHelper.h"
#include "Component.h"
#include "SWTParameters.h"

// Help functions
cv::Mat ColorizeLabels(const cv::Mat &input, int highestLabel);
bool IsNeighbour(const Point &p1, const Point &p2, const cv::Mat &input, const cv::Mat &componentMap, float bgValue);
inline bool IsDiscovered(const cv::Mat &componentMap, int x, int y);
inline bool IsBackground(const cv::Mat &swtImage, int x, int y, float bgValue);

cv::Mat ConnectedComponentsHelper::FindComponents(const cv::Mat &input, List< Ptr<Component> > &components, float bgValue)
{
    cv::Mat componentMap(input.size(), CV_16UC1, cv::Scalar(0));
    
    int current_label = 0;
    for (int i = 0; i < input.size().width;  ++i)
    for (int j = 0; j < input.size().height; ++j)
    {
        Stack<Point> stack;
        
        if (IsDiscovered(componentMap, i, j) || IsBackground(input, i, j, bgValue))
            continue;
        
        stack.push(Point(i, j));
        current_label++;
        auto currentComponent = New<Component>();
        
        OrderedSet<Point> points( [](Point p1, Point p2) {
            return (p1[0] == p2[0]) ? p1[1] < p2[1] : p1[0] < p2[0];
        });
        
        // Depth First Search. Stack invariant: Every point on the stack is a valid undiscovered point
        while(!stack.empty())
        {
            Point p1 = stack.top();
            int x = p1[0], y = p1[1];
            stack.pop();
            // Mark the current cell
            componentMap.at<ushort>(y, x) = current_label;
            //currentComponent->AddPoint(p1);
            points.insert(p1);
            // Discover neighbours
            for(int dx = -1; dx <= 1; ++dx)
            for(int dy = -1; dy <= 1; ++dy)
            {
                if(dx == 0 && dy == 0) continue;
                Point p2 = Point(x + dx, y + dy);
                if (IsNeighbour(p1, p2, input, componentMap, bgValue))
                    stack.push(p2);
            }
        }
        
        for(auto &p : points)
            currentComponent->AddPoint(p);
        currentComponent->SetMeanStrokeWidthFromImage(input);
        components.push_back(currentComponent);
    }

    cv::Mat colorComponents = ColorizeLabels(componentMap, current_label);
    
    return colorComponents;
}

cv::Mat ColorizeLabels(const cv::Mat &input, int highestLabel)
{
    List<cv::Vec3b> colors;
    colors.reserve(highestLabel);
    colors.push_back(cv::Vec3b(0, 0, 0)); // bg color
    for(int i = 1; i < highestLabel; ++i)
    {
        uchar r = rand() % 255;
        uchar g = rand() % 255;
        uchar b = rand() % 255;
        colors.push_back(cv::Vec3b(r, g, b));
    }
    
    cv::Mat colorComponents(input.size(), CV_8UC3);
    
    for (int i = 0; i < input.size().width;  ++i)
    for (int j = 0; j < input.size().height; ++j)
    {
        float value = input.at<ushort>(j, i);
        colorComponents.at<cv::Vec3b>(j, i) = colors[value];
    }
    
    return colorComponents;
}

bool IsNeighbour(const Point &p1, const Point &p2, const cv::Mat &input, const cv::Mat &componentMap, float bgValue)
{
    int x1 = p1[0], y1 = p1[1];
    int x2 = p2[0], y2 = p2[1];
    
    if (x2 < 0 || x2 >= input.size().width) return false; // out of bounds
    if (y2 < 0 || y2 >= input.size().height) return false; // out of bounds
    
    float swt1 = input.at<float>(y1, x1);
    float swt2 = input.at<float>(y2, x2);
    
    if (MathHelper::GetRatio(swt1, swt2) > MaxSWTRatio) return false; // SWT ratio too large
    if (IsBackground(input, x2, y2, bgValue)) return false;
    if (IsDiscovered(componentMap, x2, y2)) return false;
    
    return true;
}

bool IsDiscovered(const cv::Mat &componentMap, int x, int y)
{
    return componentMap.at<ushort>(y, x) != 0;
}

bool IsBackground(const cv::Mat &swtImage, int x, int y, float bgValue)
{
    return swtImage.at<float>(y, x) == bgValue;
}
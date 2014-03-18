//
//  Component.cpp
//  OGLTest
//
//  Created by Emiel Bon on 24-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "Component.h"
#include "MathHelper.h"
#include "ImgProc.h"
#include "SWTParameters.h"
#include <numeric>

void Component::Update()
{
    if (!dirty) return;
    boundingBox = cv::boundingRect(points);
    dirty = false;
}

void Component::SetMeanStrokeWidthFromImage(const cv::Mat &image)
{
    float sum = 0;
    for(auto &p : points)
        sum += image.at<float>(p[1], p[0]);
    meanStrokeWidth = sum / points.size();
    sum = 0;
    for(auto &p : points)
    {
        float d = image.at<float>(p[1], p[0]) - meanStrokeWidth;
        sum += d * d;
    }
    strokeWidthVariance = sum / points.size();
}

void Component::SetMeanColorFromImage(const cv::Mat &image)
{
    cv::Vec3i sum(0, 0, 0);
    for(auto &p : points)
    {
        auto &c = image.at<cv::Vec3b>(p[1], p[0]);
        sum[0] += c[0];
        sum[1] += c[1];
        sum[2] += c[2];
    }
    sum[0] /= points.size();
    sum[1] /= points.size();
    sum[2] /= points.size();
    
    assert(sum[0] <= 255 && sum[1] <= 255 && sum[2] <= 255);
    
    meanColor = cv::Vec3b(sum[0], sum[1], sum[2]);
}

bool Component::HasSmallerPosition(Ptr<Component> c1, Ptr<Component> c2)
{
    Point p1 = c1->BoundingBox().BaseLineCenter();
    Point p2 = c2->BoundingBox().BaseLineCenter();
    
    return (p1[0] == p2[0]) ? (p1[1] < p2[1]) : (p1[0] < p2[0]);
}

bool Component::CanLinkWith(const Component &other)
{
    if (MathHelper::GetRatio(GetMeanStrokeWidth(), other.GetMeanStrokeWidth()) >= MaxComponentSWTRatio)
        return false;
    if (MathHelper::GetRatio(BoundingBox().Height(), other.BoundingBox().Height()) >= MaxComponentHeightRatio)
        return false;
    if (MathHelper::Length((BoundingBox().Center() - other.BoundingBox().Center())) >= MaxComponentDistanceRatio * std::max(BoundingBox().Width(), other.BoundingBox().Width()))
        return false;
    
    // todo: maybe use Delta-E?
    cv::Vec3b color1 = GetMeanColor();
    cv::Vec3b color2 = other.GetMeanColor();
    
    float c1 = color1[0] * 0.30f + color1[1] * 0.59 + color1[2] * 0.11;
    float c2 = color2[0] * 0.30f + color2[1] * 0.59 + color2[2] * 0.11;
    
    if (fabs(c1 - c2) / 255.0f >= MaxColorDifference)
        return false;
    
    /*color1 = ImgProc::ConvertColor(color1, CV_BGR2YUV);
    color2 = ImgProc::ConvertColor(color2, CV_BGR2YUV);
    
    uchar u = abs(color1[1] - color2[1]);
    uchar v = abs(color1[2] - color2[2]);
    
    printf("%i ", u * v);
    
    if (u * v > 2)
        return false;
    */
    
    return true;
}

bool Component::IsValid() const
{
    float aspectRatio = (float)BoundingBox().Width() / BoundingBox().Height();
    float mean = GetMeanStrokeWidth();
    float occupancy = GetOccupancy();
    float variance = GetStrokeWidthVariance();
    
    // todo: have to make width and height some sort of scale
    return (aspectRatio >= MinAspectRatio && aspectRatio <= MaxAspectRatio &&
            occupancy   >= MinOccupancy   &&
           (aspectRatio < 1 || occupancy <= MaxOccupancy)   &&
            variance <= mean / MaxVarianceToMeanRatio);
}
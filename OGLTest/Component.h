//
//  Component.h
//  OGLTest
//
//  Created by Emiel Bon on 24-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "BoundingBox.h"

struct Component
{
public:

    Component();
    
    void AddPoint(const Point &point);
    
    const List<Point>& Points() const;
    
    const BoundingBox& BoundingBox() const;
    
    float GetMeanStrokeWidth() const;
    
    void SetMeanStrokeWidth(float strokeWidth);
    
    void SetMeanStrokeWidthFromImage(const cv::Mat &image);
    
    float GetStrokeWidthVariance() const;
    
    const cv::Vec3b& GetMeanColor() const;
    
    void SetMeanColor(const cv::Vec3b &color);
    
    void SetMeanColorFromImage(const cv::Mat &image);
    
    float GetOccupancy() const;
    
    // Can be used with std::sort to sort components from left-to-right, top-to-bottom
    static bool HasSmallerPosition(Ptr<Component> c1, Ptr<Component> c2);
    
    bool CanLinkWith(const Component &other);
    
    bool IsValid() const;
    
private:
    
    void Update();
    
private:
    
    bool dirty;
    List<Point> points;
    cv::Vec3b meanColor;
    ::BoundingBox boundingBox;
    float meanStrokeWidth;
    float strokeWidthVariance;
};

inline Component::Component() : dirty(true)
{
    
}

inline void Component::AddPoint(const Point &point)
{
    dirty = true;
    points.push_back(point);
}

inline const List<Point>& Component::Points() const
{
    return points;
}

inline const BoundingBox& Component::BoundingBox() const
{
    const_cast<Component*>(this)->Update();
    return boundingBox;
}

inline float Component::GetMeanStrokeWidth() const
{
    return meanStrokeWidth;
}

inline void Component::SetMeanStrokeWidth(float strokeWidth)
{
    meanStrokeWidth = strokeWidth;
}

inline float Component::GetStrokeWidthVariance() const
{
    return strokeWidthVariance;
}

inline const cv::Vec3b& Component::GetMeanColor() const
{
    return meanColor;
}

inline void Component::SetMeanColor(const cv::Vec3b &color)
{
    meanColor = color;
}

inline float Component::GetOccupancy() const
{
    return (float)points.size() / this->BoundingBox().Area();
}
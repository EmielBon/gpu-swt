//
//  Renderer.h
//  OGLTest
//
//  Created by Emiel Bon on 31-01-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "Window.h"
#include "types.h"

class RenderWindow : public Window
{
private:
    
    typedef Window base;
    
public:
    
    RenderWindow(int width, int height, const String &title);
    
    void DrawRect(const DrawableRect &rect);
    
    void Draw();
    
private:
    
    void AddTexture(cv::Mat mat, const String &descriptor = "");
    
    cv::Mat StrokeWidthTransform(const cv::Mat &edgeMap, const cv::Mat &gradients, bool lightOnDark);
    
    bool IsEdgePixel(const cv::Mat &edgeMap, int x, int y) const;
    
    bool InRange(const cv::Mat &edgeMap, int x, int y) const;
    
    Ptr< List<Ray> > CastRays(const cv::Mat &edgeMap, const cv::Mat &gradients, bool darkOnLight) const;
    
    void CalculateStrokeWidths(cv::Mat &swtImage, const List<Ray> rays) const;
    
    cv::Mat FindComponents(const cv::Mat &swtImage, List<Component> &components, float bgValue) const;
    
    bool IsNeighbour(const Point &p1, const Point &p2, const cv::Mat &swtImage, const cv::Mat &componentMap, float bgValue) const;
    
    bool IsDiscovered(const cv::Mat &componentMap, int x, int y) const;
    
    bool IsBackground(const cv::Mat &swtImage, int x, int y, float bgValue) const;
    
    bool IsValidComponent(Component &component) const;
    
    Component* GetSharedComponent(const Chain &c1, const Chain &c2);
    
    bool ChainingIteration(LinkedList<Ptr<Chain>> &chains);
    
private:
    
    ::GraphicsDevice&  device;
    Ptr<Program>       program;
    Ptr<DrawableRect>  rect1;
    List<Ptr<Texture>> textures;
    List<String>       textureDescriptors;
    int                currentTextureIndex;
};

inline bool RenderWindow::IsEdgePixel(const cv::Mat &edgeMap, int x, int y) const
{
    return (edgeMap.at<uchar>(y, x) == 255);
}

inline bool RenderWindow::InRange(const cv::Mat &image, int x, int y) const
{
    return (x >= 0 && x < image.cols && y >= 0 && y < image.rows);
}
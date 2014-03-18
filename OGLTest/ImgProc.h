//
//  ImgProc.h
//  OGLTest
//
//  Created by Emiel Bon on 14-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#pragma once

#include "types.h"
#include "opencv2/imgproc/imgproc.hpp"

class ImgProc
{
public:
    
    static cv::Mat ConvertToGrayscale(const cv::Mat &image);
    
    static cv::Mat Sharpen(const cv::Mat &image);
    
    static cv::Mat ConvertToFloat(const cv::Mat &image, int type);
    
    // Input has to be CV_8U
    static cv::Mat CalculateEdgeMap(const cv::Mat &image);
    
    static cv::Mat CalculateGradientX(const cv::Mat &image);
    
    static cv::Mat CalculateGradientY(const cv::Mat &image);
    
    static cv::Mat CalculateGradients(const cv::Mat &image, bool normalize);
    
    // Input has to be CV_32FC1
    static cv::Mat NormalizeImage(const cv::Mat &image, float scale);

    static cv::Mat NormalizeImage(const cv::Mat &image, float scale, float bgValue, float newBgValue);
    
    static cv::Mat ContrastStretch(const cv::Mat &input, int lowerPercentile, int upperPercentile);
    
    static cv::Vec3b ConvertColor(const cv::Vec3b &input, int type);
    
    static cv::Mat DrawBoundingBoxes(const cv::Mat &input, const List<BoundingBox> &bboxes, const cv::Scalar &color);
    
    static void DrawBresenhamLine(int x0, int y0, int x1, int y1, cv::Mat &input);
    
    static void Plot(int x, int y, cv::Mat &input);
};

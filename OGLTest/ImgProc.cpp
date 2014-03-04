//
//  ImgProc.cpp
//  OGLTest
//
//  Created by Emiel Bon on 14-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "ImgProc.h"
#include "opencv2/imgproc/imgproc.hpp"

static const float LOW_THRESHOLD = 70 /*/ 255.f*/;
static const float HIGH_THRESHOLD = 120 /*/ 255.f*/;
static const int KERNEL_SIZE = 3;

void ImgProc::DrawBresenhamLine(int x0, int y0, int x1, int y1, cv::Mat &input)
{
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    
    if (steep)   { std::swap(x0, y0); std::swap(x1, y1); }
    if (x0 > x1) { std::swap(x0, x1); std::swap(y0, y1); }
    
    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int err = dx / 2;
    int ystep = (y0 < y1 ? 1 : -1);
    int y = y0;
    
    for (int x = x0; x <= x1; ++x)
    {
        if (steep)
            Plot(y, x, input);
        else
            Plot(x, y, input);
        err = err - dy;
        if (err < 0) { y += ystep; err += dx; }
    }
}

void ImgProc::Plot(int x, int y, cv::Mat &input)
{
    input.at<GLubyte>(y, x) = 255;
}

cv::Mat ImgProc::ConvertToGrayscale(const cv::Mat &image)
{
    cv::Mat luv, grayImage;
    cv::cvtColor(image, luv, CV_RGB2Luv);
    cv::extractChannel(luv, grayImage, 0);
    return grayImage;
}

cv::Mat ImgProc::Sharpen(const cv::Mat &image)
{
    cv::Mat blurred, sharper;
    cv::GaussianBlur(image, blurred, cv::Size(7, 7), 5);
    cv::addWeighted(image, 1.5, blurred, -0.5, 0, sharper);
    return sharper;
}

cv::Mat ImgProc::ConvertToFloat(const cv::Mat &image)
{
    cv::Mat imagef(image.size(), CV_32FC1);
    image.convertTo(imagef, CV_32FC1, 1.0f / 255.0f);
    return imagef;
}

cv::Mat ImgProc::CalculateEdgeMap(const cv::Mat &image)
{
    cv::Mat edgeMap;
    cv::Canny(image, edgeMap, LOW_THRESHOLD, HIGH_THRESHOLD, KERNEL_SIZE, true);
    return edgeMap;
}

cv::Mat ImgProc::CalculateGradientX(const cv::Mat &image)
{
    cv::Mat gradientX(image.size(), CV_32FC1);
    cv::Sobel(image, gradientX, CV_32F, 1, 0, CV_SCHARR);
    return gradientX;
}

cv::Mat ImgProc::CalculateGradientY(const cv::Mat &image)
{
    cv::Mat gradientY(image.size(), CV_32FC1);
    cv::Sobel(image, gradientY, CV_32F, 0, 1, CV_SCHARR);
    return gradientY;
}

cv::Mat ImgProc::CalculateGradients(const cv::Mat &image, bool normalize)
{
    cv::Mat gradientX = CalculateGradientX(image);
    cv::Mat gradientY = CalculateGradientY(image);
    
    cv::Mat gradients;
    cv::Mat inputs[] = { gradientX, gradientY };
    cv::merge(inputs, 2, gradients);
    
    if (normalize)
    {
        for(int i = 0; i < image.cols; ++i)
        for(int j = 0; j < image.rows; ++j)
        {
            cv::Vec2f &gradient = gradients.at<cv::Vec2f>(j, i);
            cv::Vec2f normalized = cv::normalize(gradient);
            gradient = normalized;
        }
    }
    
    return gradients;
}

cv::Mat ImgProc::NormalizeImage(const cv::Mat &image, float scale)
{
    return NormalizeImage(image, scale, -1, 0);
}

cv::Mat ImgProc::NormalizeImage(const cv::Mat &image, float scale, float bgValue, float newBgValue)
{
    cv::Mat normalized(image.size(), CV_32FC1, 0.0f);
    float max = 0;
    
    for(int i = 0; i < image.size().width; ++i)
    for(int j = 0; j < image.size().height; ++j)
    {
        float value = image.at<float>(j, i);
        if (value != bgValue)
            max = std::max(value, max);
    }
    
    if (max == 0)
        throw std::runtime_error("Error normalizing image, division by 0");
    
    for(int i = 0; i < image.size().width; ++i)
    for(int j = 0; j < image.size().height; ++j)
    {
        float value = image.at<float>(j, i);
        normalized.at<float>(j, i) = (value == bgValue) ? newBgValue
                                                        : value / max * scale;
    }
    return normalized;
}
//
//  ImgProc.cpp
//  OGLTest
//
//  Created by Emiel Bon on 14-02-14.
//  Copyright (c) 2014 Emiel Bon. All rights reserved.
//

#include "ImgProc.h"
#include "Component.h"
#include "SWTParameters.h"
#include "MathHelper.h"

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
    clock_t f;
    f = clock();
    cv::cvtColor(image, luv, CV_BGR2Luv);
    cv::extractChannel(luv, grayImage, 0);
    f = clock() - f;
    printf("OpenCV grayscale timer: %lu\n", f);
    return grayImage;
}

cv::Mat ImgProc::Sharpen(const cv::Mat &image)
{
    cv::Mat blurred, sharper;
    clock_t f;
    f = clock();
    cv::GaussianBlur(image, blurred, cv::Size(7, 7), 5);
    //cv::addWeighted(image, 1.5, blurred, -0.5, 0, sharper);
    f = clock() - f;
    printf("OpenCV gaussian timer: %lu\n", f);
    
    return sharper;
}

cv::Mat ImgProc::ConvertToFloat(const cv::Mat &image, int type)
{
    cv::Mat imagef(image.size(), type);
    image.convertTo(imagef, type, 1.0f / 255.0f);
    return imagef;
}

cv::Mat ImgProc::CalculateEdgeMap(const cv::Mat &image)
{
    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(3, 3), 5);
    
    int width  = image.size().width;
    int height = image.size().height;
    
    /*int meanSum = 0;
    
    for(int i = 0; i < width; ++i)
    for(int j = 0; j < height; ++j)
        meanSum += image.at<uchar>(j, i);
    
    int mean = meanSum / (width * height);
    */
    
    List<uchar> values;
    values.reserve(width * height);
    for(int i = 0; i < width;  ++i)
    for(int j = 0; j < height; ++j)
        values.push_back( blurred.at<uchar>(j, i) );
    
    uchar mean = MathHelper::Median(values);
    
    cv::Mat edgeMap;
    clock_t f;
    f = clock();
    cv::Canny(blurred, edgeMap, 0.66 * mean, 1.33 * mean, KERNEL_SIZE, true);
    f = clock() - f;
    printf("OpenCV Canny timer: %lu\n", f);
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
    clock_t f;
    f = clock();
    cv::Mat gradientX = CalculateGradientX(image);
    cv::Mat gradientY = CalculateGradientY(image);
    f = clock() - f;
    printf("OpenCV Sobel timer: %lu\n", f);
    
    cv::Mat gradients;
    cv::Mat inputs[] = { gradientX, gradientY };
    cv::merge(inputs, 2, gradients);
    
    if (normalize)
    {
        for(int i = 0; i < image.size().width;  ++i)
        for(int j = 0; j < image.size().height; ++j)
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

cv::Mat ImgProc::ContrastStretch(const cv::Mat &input, int lowerPercentile, int upperPercentile)
{
    List<int> histogram;
    histogram.resize(255, 0);
    
    int width  = input.size().width;
    int height = input.size().height;
    
    for(int i = 0; i < width;  ++i)
    for(int j = 0; j < height; ++j)
    {
        histogram[ input.at<uchar>(j, i) ]++;
    }
    
    int min = 0, max = 0;
    int percentile = 0;
    int counter = 0;
    const int totalPixels = width * height;
    int total = 0;
    
    while(percentile <= lowerPercentile)
    {
        total += histogram[counter++];
        percentile = total * 100 / totalPixels;
    }
    
    min = counter;
    
    while(percentile <= upperPercentile && counter < 255)
    {
        total += histogram[counter++];
        percentile = total * 100 / totalPixels;
    }
    
    max = counter;
    
    cv::Mat output(input.size(), CV_8UC1);
    
    for(int i = 0; i < width;  ++i)
    for(int j = 0; j < height; ++j)
    {
        output.at<uchar>(j, i) = cv::saturate_cast<uchar>(((int)input.at<uchar>(j, i) - min) * ((255 - 0) / (max - min)) + 0);
    }
    
    return output;
}

cv::Mat ImgProc::DrawBoundingBoxes(const cv::Mat &input, const List<BoundingBox> &bboxes, const cv::Scalar &color)
{
    cv::Mat output = input.clone();
    for(auto &bbox : bboxes)
        cv::rectangle(output, bbox.Bounds, color);
    return output;
}

cv::Vec3b ImgProc::ConvertColor(const cv::Vec3b &input, int type)
{
	cv::Mat mat (cv::Size (1,1), CV_8UC3);
	mat.at<cv::Vec3b>(0,0) = input;
	cv::cvtColor(mat, mat, type);
	return mat.at<cv::Vec3b>(0,0);
}
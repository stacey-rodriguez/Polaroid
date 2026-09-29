#pragma once
#include <opencv2/opencv.hpp>


void polaroidColor(cv::Mat& img);
void addGrain(cv::Mat& img);
void imperfections(cv::Mat& img);

cv::Mat addFrame(const cv::Mat& img);


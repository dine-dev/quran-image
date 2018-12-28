#ifndef CONTOUR_H
#define CONTOUR_H

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include <numeric>
#include <vector>
#include <algorithm> 

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

bool comp(std::vector<cv::Point> & contour_1, std::vector<cv::Point> & contour_2); 
void crop(cv::Mat & src, cv::Mat & cropped);
void whiten(cv::Mat & src, int height);
void lines(cv::Mat & src);

void misc(cv::Mat & src, const cv::Mat & tpl);

#endif
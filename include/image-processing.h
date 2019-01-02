#ifndef CONTOUR_H
#define CONTOUR_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "utils.h"


bool comp(std::vector<cv::Point> & contour_1, std::vector<cv::Point> & contour_2); 
void crop(cv::Mat & src, cv::Mat & cropped);
void whiten(cv::Mat & src, int height);
void lines(cv::Mat & src, std::vector<int> & detected_lines, cv::Rect & frame_bounding_rect);
void words(const cv::Mat & src, std::vector<cv::Rect> & contour_rect);

void misc(cv::Mat & src, const cv::Mat & tpl);

#endif
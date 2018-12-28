#include "image-processing.h"

bool comp(std::vector<cv::Point>& contour_1, std::vector<cv::Point>& contour_2)
{
    return (cv::contourArea(contour_1) < cv::contourArea(contour_2));
}

void crop(cv::Mat& src, cv::Mat& cropped)
{
    cv::Mat src_gray, src_HSV, dst;
    
    // threshold in grayscale
    // Convert the image to Gray
    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

    // 0: Binary 1: Binary Inverted 2: Threshold Truncated 3: Threshold to Zero 4: Threshold to Zero Inverted
    int threshold_type = 4, threshold_value = 185; // max value = 255;
    int const max_BINARY_value = 255;

    cv::threshold(src_gray, dst, threshold_value, max_BINARY_value, threshold_type);

    // display intermediate
    if(0) {
        cv::imshow("thresholded", dst);
    }

    // determine contours
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dst, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    // find max area contours
    std::vector<std::vector<cv::Point> >::iterator result;
    result = std::max_element(contours.begin(), contours.end(), comp);

    cv::Rect bounding_rect = cv::boundingRect((*result));

    // display intermediate
    if(0) {
        cv::Scalar color = cv::Scalar(0, 0, 0);
        cv::rectangle(src, bounding_rect.tl(), bounding_rect.br(), color, 2, 8, 0);
        cv::imshow("src with bounding rectangle", src);
    }

    cropped = src(bounding_rect);
    return;
}

void whiten(cv::Mat& src, int height)
{
    cv::Mat src_HSV, src_threshold;

    // convert from BGR to HSV colorspace
    cv::cvtColor(src, src_HSV, cv::COLOR_BGR2HSV);

    // threshold the HSV image to get only yellow color
    cv::inRange(src_HSV, cv::Scalar(24, 31, 250), cv::Scalar(35, 50, 255), src_threshold);
    // src_threshold.rowRange(height, src.rows) = 0;

    src.setTo(255, src_threshold);
    return;
}

void misc(cv::Mat & src, const cv::Mat & tpl){
    cv::Mat src_HSV, src_threshold, tpl_HSV, tpl_threshold;
    
    // convert from BGR to HSV colorspace
    cv::cvtColor(src, src_HSV, cv::COLOR_BGR2HSV);
    cv::cvtColor(tpl, tpl_HSV, cv::COLOR_BGR2HSV);

    // threshold the HSV image to get only yellow color
    int low_H=0, high_H=180, low_S=0, high_S=40, low_V=68, high_V=255;
    cv::inRange(src_HSV, cv::Scalar(low_H, low_S, low_V), cv::Scalar(high_H, high_S, high_V), src_threshold);
    cv::inRange(tpl_HSV, cv::Scalar(low_H, low_S, low_V), cv::Scalar(high_H, high_S, high_V), tpl_threshold);
        
    // for src image determine contours then find max area contour and the associated bounding rectangle
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> >::const_iterator result;
    
    cv::findContours(src_threshold, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    result = std::max_element(contours.begin(), contours.end(), comp);
    cv::Rect bounding_rect_src = cv::boundingRect((*result));
    
    // same for tpl image
    contours.clear();
    hierarchy.clear();
    
    cv::findContours(tpl_threshold, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    result = std::max_element(contours.begin(), contours.end(), comp);
    cv::Rect bounding_rect_tpl = cv::boundingRect((*result));
    
    //display intermediary results
    if(0) {
        //rectangle(src, bounding_rect_src.tl(), bounding_rect_src.br(), cv::Scalar(255,0,0), 2, 8, 0 );
        cv::imshow("src",src);
        //rectangle(tpl, bounding_rect_tpl.tl(), bounding_rect_tpl.br(), cv::Scalar(255,0,0), 2, 8, 0 );
        cv::imshow("tpl",tpl);
    }
    
    // make a region of interest containing the src page with ayah
    cv::Rect roi( bounding_rect_tpl.tl(), bounding_rect_src.size() );
    
    // copy the src ayah page into the src_cropped tpl ayah page
    src(bounding_rect_src).copyTo( tpl( roi ) );
    
    // copy cropped page in src
    src = tpl;
    
    return;
}

void lines(cv::Mat & src) {
    //hist = cv2.reduce(rotated,1, cv2.REDUCE_AVG).reshape(-1);
    cv::Mat src_gray, th, hist;
    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    cv::threshold( src_gray, th, 127, 255, cv::THRESH_BINARY_INV|cv::THRESH_OTSU);
    //cv::imshow("threshold",th);
    //cv::imshow("threshold",src_gray);
    
    //std::cout << src.type() << "\n";
    //std::cout << src_gray.type() << "\n";
    cv::reduce(th, hist, 1, cv::REDUCE_SUM, CV_64FC1);
    
    int hist_thr = 22000;
    for(int indx = 1; indx < hist.rows; ++indx) {
        //std::cout << hist.at<double>(indx) << " ";
        if(hist.at<double>(indx-1) < hist_thr && hist.at<double>(indx) > hist_thr) {
            cv::line(src, cv::Point(0,indx), cv::Point(src.rows, indx), cv::Scalar(255,0,0), 1);
        }
    }
    std::cout << "\n";
    cv::imshow("tot",src);
    //std::cout << hist << "\n";
    return;
    
}

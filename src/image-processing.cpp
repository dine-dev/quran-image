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

void lines(cv::Mat & src, int page_number) {
    
    cv::Mat src_gray_thr;
    cv::cvtColor(src, src_gray_thr, cv::COLOR_BGR2GRAY);
    cv::threshold( src_gray_thr, src_gray_thr, 240, 255, cv::THRESH_BINARY);
//    cv::imshow("src",src);
//    cv::imshow("threshold",src_gray_thr);
    
    // for src image determine contours then find max area contour and the associated bounding rectangle
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> >::const_iterator result;
    
    cv::findContours(src_gray_thr, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    result = std::max_element(contours.begin(), contours.end(), comp);
    cv::Rect bounding_rect_src = cv::boundingRect((*result));

    // keep only text part of page (remove page decoration frame)
    src_gray_thr = src_gray_thr(bounding_rect_src);
    //cv::imshow("crop",src_gray_thr);
    
    const double number_of_line_per_page = 15;
    const int estimated_line_height = src_gray_thr.rows/number_of_line_per_page;
    
    // subdivide in equi-height line 
    if(0) {
        double ayah_line_height = src_gray_thr.rows/number_of_line_per_page;
        int decoration_frame_offset = bounding_rect_src.tl().x;
        for(int lin = 0; lin < 16; lin++) {
            cv::line(src,cv::Point(0, decoration_frame_offset + (int)(ayah_line_height*lin)), cv::Point(src.rows, decoration_frame_offset + (int)(ayah_line_height*lin)), cv::Scalar(0, 0, 255), 1);
        }
        cv::imshow("crop src",src);
        return;
    }
    
    // invert black and white
    cv::bitwise_not ( src_gray_thr, src_gray_thr );
    
    std::vector<float> white_pixels_in_row(src_gray_thr.rows, 0);
    for(int lin=0; lin<src_gray_thr.rows; lin++) {
        for (int col=0; col<src_gray_thr.cols; col++) {
            //std::cout << (int)th.at<uchar>(lin,col) << " "; //white
            white_pixels_in_row[lin] += (int)src_gray_thr.at<uchar>(lin,col) ? 1 : 0;
        }
    }
    
    // print vector
    int lineOffset = bounding_rect_src.tl().x;
    //cv::Mat copy;
    std::vector<int> detected_lines; 
    for(int thr = 22; thr < 28; thr++) {
        //src.copyTo(copy);
        //copy = cv::Mat(src);
        for(int indx = 1; indx < src_gray_thr.rows; ++indx) {
            if(white_pixels_in_row.at(indx - 1) < thr && white_pixels_in_row.at(indx) > thr) {
                //cv::line(copy, cv::Point(0, indx + lineOffset), cv::Point(src.rows, indx + lineOffset), cv::Scalar(0, 0, 255), 1);
                detected_lines.push_back(indx + lineOffset);
            }
        }
        //cv::imshow("lines thr: " + std::to_string(thr), copy);
        //copy.release();
    }
    
    // remove duplicate
    std::sort(detected_lines.begin(), detected_lines.end());
    detected_lines.erase( unique( detected_lines.begin(), detected_lines.end() ), detected_lines.end() );
    
    // remove adjacent lines
    int adjacent_line_tolerance = 5;
    for(int indx = 1; indx < detected_lines.size(); /*++indx*/) {
        if(std::abs( detected_lines[indx - 1] - detected_lines[indx] ) < adjacent_line_tolerance ) {
            detected_lines.erase(detected_lines.begin() + indx);
        }
        else {
            ++indx;
        }
    }
    
    // append beginning line if missing (special case four last pages)
//    if(detected_lines[0] > estimated_line_height) {
//        detected_lines.insert(detected_lines.begin(),detected_lines[0] - estimated_line_height);
//    }
    
    // remove to small line
    int max_line_height = 50;
    for(int indx = 1; indx < detected_lines.size(); /*++indx*/) {
        if(std::abs( detected_lines[indx - 1] - detected_lines[indx] ) < max_line_height ) {
            detected_lines.erase(detected_lines.begin() + indx);
        }
        else {
            ++indx;
        }
    }
    
    // insert missing line
    
    for(int indx = 0; indx < detected_lines.size(); ++indx) {
        cv::line(src, cv::Point(0, detected_lines[indx]), cv::Point(src.rows, detected_lines[indx]), cv::Scalar(0, 0, 255), 1);
    }
    //printVector(detected_lines);
    //cv::imshow("detected lines", src);
    //cv::waitKey();
    
    if(detected_lines.size() < 15) {
        std::cout << page_number << "\n";
    }

    return;
    
}

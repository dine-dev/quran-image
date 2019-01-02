/*
export DYLD_LIBRARY_PATH="/Users/bdz/Perso/dev/libraries/opencv/lib"
g++ -Wall -std=c++11 -I/Users/bdz/Perso/dev/libraries/opencv/include/opencv4 -o contour contour.cpp -L/Users/bdz/Perso/dev/libraries/opencv/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs
*/

#include "image-processing.h"
#include "data.h"
#include "utils.h"

void detectLines(std::string riwaya);

/**
 * @function main
 */
int main(int argc, char** argv) {
    
    const std::string riwaya = "hafs"; //warsh

    detectLines(riwaya);
    return 0;
}

void detectWord(const cv::Mat & src, const std::vector<int> & detected_lines, const cv::Rect & frame_bounding_rect, int page_number) {

    //cv::imshow("base", src);
    cv::Rect roi;
    cv::Mat clone = src.clone();
    for(int indx = 0; indx < detected_lines.size(); ++indx) {
        //cv::line(src, cv::Point(0, detected_lines[indx]), cv::Point(src.rows, detected_lines[indx]), cv::Scalar(0, 0, 255), 1);
        
        roi = cv::Rect (cv::Point(frame_bounding_rect.tl().x, detected_lines[indx] + (indx ? 0 : 3)),
                        cv::Size(frame_bounding_rect.size().width, (indx < detected_lines.size()-1) ? detected_lines[indx+1] - detected_lines[indx] : 60));
        
        cv::Mat crop = src(roi);
        
        std::vector<cv::Rect> contour_rect;
        words(crop, contour_rect);
        for (size_t idx = 0; idx < contour_rect.size(); idx++) {
            //cv::rectangle(crop, contour_rect[idx].tl(), contour_rect[idx].br(), cv::Scalar(0, 0, 255), 2, 8, 0);
            cv::rectangle(clone, cv::Point(frame_bounding_rect.tl().x+contour_rect[idx].tl().x, contour_rect[idx].tl().y + detected_lines[indx]),
                               cv::Point(frame_bounding_rect.tl().x+contour_rect[idx].br().x,contour_rect[idx].br().y + detected_lines[indx]),
                               cv::Scalar(0, 0, 255), 2, 8, 0);
        }
    }
    //cv::imshow("wdetect", clone);
    cv::imwrite("../res/hafs_word_detection/" + std::to_string(page_number) + ".jpg", clone);
    //cv::waitKey();
    return;
}

void detectLines(std::string riwaya) {
    cv::Mat src;
    cv::Rect frame_bounding_rect;
    std::vector<int> pages_number(604), detected_lines;
    std::iota(pages_number.begin(), pages_number.end(), 1);
    
    //pages_number = {3};
    std::for_each(pages_number.cbegin(), pages_number.cend(), [&](const int page_number) {
        src = cv::imread("../res/"+ riwaya +"_whiten/" + std::to_string(page_number) + ".jpg", 1);
        //src = cv::imread("../res/"+ riwaya +"/" + std::to_string(page_number) + ".jpg", 1);
        lines(src, detected_lines, frame_bounding_rect);
        //cv::imwrite("../res/hafs_line_detection/" + std::to_string(page_number) + ".jpg", src);
        //cv::imwrite("../res/warsh_line_detection/" + std::to_string(page_number) + ".jpg", src);
        detectWord(src, detected_lines, frame_bounding_rect, page_number);
    });
}

void renameFile(){
    //std::string dir_name = "../res/warsh_renamed";//"../res/hafs_renamed";
    std::string dir_name = "../res/hafs_renamed";
    std::string base_filename, file_without_extension, file_rename;
    std::vector<std::string> files = filesInDirectory(dir_name);
    int file_name_int;
    std::for_each(files.cbegin(), files.cend(), [&](const std::string file) {
        // keep file name only with extension
        base_filename = file.substr(file.find_last_of("/\\") + 1);
        
        // remobe extension
        std::string::size_type const p(base_filename.find_last_of('.'));
        file_without_extension = base_filename.substr(0, p);
        file_name_int = std::stoi( file_without_extension );
        file_rename = dir_name + "/page" + std::to_string(file_name_int/100) + std::to_string((file_name_int%100)/10) + std::to_string(file_name_int%10) + ".jpg";
        //std::cout << file << " to " << file_rename << "\n";
        std::rename(file.c_str(), file_rename.c_str());
    });
}

void cropMiscPage() {
    cv::Mat src, cropped;
    const std::string image_file_format = ".jpg";
    
    // vector contained failed pages
    std::vector<int> misc_pages = {/*26, 64, 77, 92, 251, 268, */344};
    //std::vector<int> misc_pages = {77, 315};
    
    // the template
    cropped = cv::imread("../res/hafs_whiten/tpl_for_misc.jpg", 1);

    std::for_each(misc_pages.cbegin(), misc_pages.cend(), [&](const int page_number) {
        src = cv::imread("../res/hafs_whiten/" + std::to_string(page_number) + image_file_format, 1);
        // cv::imshow("src "+ std::to_string(page_number),src);
        misc(src, cropped);
        // cv::imshow("src cropped "+ std::to_string(page_number),src);
        cv::imwrite("../res/hafs_whiten/" + std::to_string(page_number) + "_misc"+ image_file_format, cropped);
    });
}

void getQuranImages() {
    
    // quran page url
    //http://www.easyquran.com/quran-jpg-w/images/1.jpg
    //http://www.easyquran.com/quran-jpg/images/1.jpg
    const std::string quran_page_url = "http://www.easyquran.com/quran-jpg/images/";
    const std::string image_file_format = ".jpg";
    
    std::vector<int> pages_number(604);
    std::iota(pages_number.begin(), pages_number.end(), 1);

    std::vector<std::string> quran_pages_url(604);
    std::for_each(pages_number.cbegin(), pages_number.cend(), [&](const int page_number) {
        quran_pages_url.at(page_number - 1) = quran_page_url + std::to_string(page_number) + image_file_format;
    });

    // retrieve quran image file from internet with wget
    std::for_each(pages_number.cbegin(), pages_number.cend(), [&](const int page_number) {
        wgetImage(quran_pages_url.at(page_number - 1), "../res/hafs/" + std::to_string(page_number) + image_file_format);
    });
}

void cropImages() {
    cv::Mat src, cropped;
    const std::string quran_page_url = "http://www.easyquran.com/quran-jpg/images/";
    const std::string image_file_format = ".jpg";
    
    std::vector<int> pages_number(604);
    std::iota(pages_number.begin(), pages_number.end(), 1);
    
    // crop image and whiten header
    int page_header_height = 10; // in pixel
    std::for_each(pages_number.cbegin(), pages_number.cend(), [&](const int page_number) {
        src = cv::imread(std::to_string(page_number) + image_file_format, 1);
        // crop image
        crop(src, cropped);
        
        // save image into crop folder
        //cv::imwrite("crop/" + std::to_string(page_number) + image_file_format, cropped);

        // whiten header
        whiten(cropped, page_header_height);
        
        // save image into whiten folder
        cv::imwrite("../res/hafs_whiten/" + std::to_string(page_number) + image_file_format, cropped);
    });

    return;
}

void cropSingleImage () {
    cv::Mat src, cropped;
    const std::string image_file_format = ".jpg";
    // Load an image
    src = cv::imread("../res/hafs/3.jpg", 1);
    crop(src, cropped);
    cv::imshow("crop", cropped);

    // remove yellow background
    int page_header_height = 10; // in pixel
    whiten(cropped, page_header_height);
    cv::imshow("white", cropped);
    cv::imwrite("../res/hafs_whiten/tpl_for_misc" + image_file_format, cropped);
    cv::waitKey();
}
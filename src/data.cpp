#include "data.h"

void wgetImage(const std::string& url, const std::string& file_name)
{
    // return if file already exist
    if(access( file_name.c_str(), 0 ) == 0) {
        return;
    }
    
    CURL* image;
    CURLcode imgresult;
    FILE* fp;

    image = curl_easy_init();
    if(image) {
        // Open file
        fp = fopen(file_name.c_str(), "wb");
        if(fp == NULL) {
            std::cout << "File cannot be opened!\n";
            // Clean up the resources
            curl_easy_cleanup(image);
            return;
        }

        curl_easy_setopt(image, CURLOPT_URL, url.c_str());
        curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(image, CURLOPT_WRITEDATA, fp);

        // Grab image
        imgresult = curl_easy_perform(image);
        if(imgresult) {
            std::cout << "Cannot grab the image!\n";
        }
        
        // Close the file
        fclose(fp);
    }

    // Clean up the resources
    curl_easy_cleanup(image);
    
    return;
}
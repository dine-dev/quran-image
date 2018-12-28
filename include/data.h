#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <unistd.h>
#include <curl/curl.h>

void wgetImage(const std::string& url, const std::string& file_name);


#endif
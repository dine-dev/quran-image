#include "utils.h"

std::vector<std::string> filesInDirectory(const std::string & dir_name) {
    glob_t glob_result;
    std::string pattern = dir_name + "/*";
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    std::vector<std::string> files;
    for(unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}
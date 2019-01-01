#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <glob.h>

template<typename Real> void printVector(const std::vector<Real> &x) {
    std::cout << "[";
    std::copy(x.cbegin(), x.cend(), std::ostream_iterator<Real>(std::cout, " "));
    std::cout << "]\n\n";    
    return;
}

std::vector<std::string> filesInDirectory(const std::string & dir_name);

#endif
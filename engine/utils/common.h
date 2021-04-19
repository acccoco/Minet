#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#include <vector>
#include <algorithm>

template<class T>
static bool vector_find(const std::vector<T> &vec, const T &value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

#endif //RENDER_COMMON_H

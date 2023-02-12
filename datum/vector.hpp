#pragma once

#include <vector>

namespace WarGrey::STEM {
    template<typename T> void inline vector_fill(std::vector<T>& vs, const T& val = T()) {
        std::fill(vs.begin(), vs.end(), val);
    }
    
    template<typename T> void inline vector_reset(std::vector<T>& vs, int n, const T& val = T()) {
        vs.assign(n, val);
    }

    template<typename T> T inline vector_sum(std::vector<T>& vs) {
        T total = T();

        for (int idx = 0; idx < vs.size(); idx ++) {
            total += vs[idx];
        }

        return total;
    }

    template<typename T> int inline vector_replace_sorted_minimum(std::vector<T>& vs, const T val, T* replaced = nullptr) {
        int ret_idx = -1;

        for (int idx = 0; idx < vs.size(); idx ++) {
            if (vs[idx] > val) {
                if (replaced != nullptr) {
                    (*replaced) = vs[idx];
                }

                vs[idx] = val;
                ret_idx = idx;
                break;
            }
        }

        return ret_idx;
    }

    template<typename T> int inline vector_replace_sorted_maximum(std::vector<T>& vs, const T val, T* replaced = nullptr) {
        int ret_idx = -1;

        for (int idx = 0; idx < vs.size(); idx ++) {
            if (vs[idx] < val) {
                if (replaced != nullptr) {
                    (*replaced) = vs[idx];
                }

                vs[idx] = val;
                ret_idx = idx;
                break;
            }
        }

        return ret_idx;
    }
}

#pragma once // 确保只被 include 一次

namespace WarGrey::STEM {
    /** 声明函数接口 **/
    __lambda__ int random_raw();
    __lambda__ int random_uniform(int min, int max);
}

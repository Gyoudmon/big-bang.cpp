#ifndef _WARGEY_RANDOM_H // 确保只被 include 一次
#define _WARGEY_RANDOM_H

namespace WarGrey::STEM {
    /** 声明函数接口 **/
    int random_raw();
    int random_uniform(int min, int max);
}

#endif


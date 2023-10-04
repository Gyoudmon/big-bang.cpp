#pragma once // 确保只被 include 一次

namespace WarGrey::STEM {
    __lambda__ int random_raw();
    __lambda__ int random_uniform(int min, int max);
    __lambda__ unsigned int random_uniform(unsigned int min, unsigned int max);
    __lambda__ float random_uniform(float min, float max);
    __lambda__ double random_uniform(double min, double max);
    
    __lambda__ bool random_bernoulli(double p_true);

    inline __lambda__ double random() { return random_uniform(0.0, 1.0); }
}

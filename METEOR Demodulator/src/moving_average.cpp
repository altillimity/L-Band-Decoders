#include "moving_average.h"
#include <numeric>

MovingAverage::MovingAverage(int length, float scale, int max_iter, unsigned int vlen) : d_length(length),
                                                                                         d_scale(scale),
                                                                                         d_max_iter(max_iter),
                                                                                         d_vlen(vlen),
                                                                                         d_new_length(length),
                                                                                         d_new_scale(scale),
                                                                                         d_updated(false)
{
    if (d_vlen > 1)
    {
        d_sum = std::vector<float>(d_vlen);
        d_scales = std::vector<float>(d_vlen, d_scale);
    }
}

void volk_sub(float *out, const float *sub, unsigned int num)
{
    for (unsigned int elem = 0; elem < num; elem++)
        out[elem] -= sub[elem];
}

void volk_add(float *out, const float *add, unsigned int num)
{
    for (unsigned int elem = 0; elem < num; elem++)
        out[elem] += add[elem];
}

void volk_mul(float *out, const float *in, const float *scale, unsigned int num)
{
    for (unsigned int elem = 0; elem < num; elem++)
        out[elem] = in[elem] * scale[elem];
}

int MovingAverage::work(float *input, int length, float *output)
{
    const unsigned int num_iter = std::min(length, d_max_iter);
    if (d_vlen == 1)
    {
        float sum = std::accumulate(&input[0], &input[d_length - 1], float{});

        for (unsigned int i = 0; i < num_iter; i++)
        {
            sum += input[i + d_length - 1];
            output[i] = sum * d_scale;
            sum -= input[i];
        }
    }
    else
    { // d_vlen > 1
        // gets automatically optimized well
        std::copy(&input[0], &input[d_vlen], std::begin(d_sum));

        for (int i = 1; i < d_length - 1; i++)
        {
            volk_add(d_sum.data(), &input[i * d_vlen], d_vlen);
        }

        for (unsigned int i = 0; i < num_iter; i++)
        {
            volk_add(d_sum.data(), &input[(i + d_length - 1) * d_vlen], d_vlen);
            volk_mul(&output[i * d_vlen], d_sum.data(), d_scales.data(), d_vlen);
            volk_sub(d_sum.data(), &input[i * d_vlen], d_vlen);
        }
    }
    return num_iter;
}
#include <vector>

class MovingAverage
{
private:
    int d_length;
    float d_scale;
    std::vector<float> d_scales;
    int d_max_iter;
    const unsigned int d_vlen;
    std::vector<float> d_sum;

    int d_new_length;
    float d_new_scale;
    bool d_updated;

public:
    MovingAverage(int length, float scale, int max_iter = 4096, unsigned int vlen = 1);
    int work(float *input, int length, float *output);
};
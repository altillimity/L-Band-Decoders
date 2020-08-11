#include <complex>

class PLL
{
private:
    float d_alpha;      // 1st order loop constant
    float d_beta;       // 2nd order loop constant
    float d_max_offset; // Maximum frequency offset, radians/sample
    float d_phase;      // Instantaneous carrier phase
    float d_freq;       // Instantaneous carrier frequency, radians/sample

public:
    PLL(float alpha, float beta, float max_offset);
    void work(std::complex<float> *sample, int length, float *out);
};
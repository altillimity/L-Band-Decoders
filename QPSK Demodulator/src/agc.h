#include <complex>

/* Based on the AGC from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) */

#define AGC_WINSIZE 1024 * 64
#define AGC_TARGET 1.0f
#define AGC_MAX_GAIN 4000
#define AGC_BIAS_WINSIZE 256 * 1024

class Agc
{
private:
    unsigned window_size_m;
    float avg_m;
    float gain_m;
    float target_ampl_m;
    std::complex<float> bias_m;

public:
    Agc(unsigned window_size, float gain, float target_ampl);
    void work(std::complex<float> *in, size_t length, std::complex<float> *out);
};
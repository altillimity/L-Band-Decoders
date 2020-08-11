#include "agc.h"

Agc::Agc(unsigned window_size, float gain, float target_ampl)
{
    window_size_m = window_size;
    target_ampl_m = target_ampl;
    avg_m = target_ampl;
    gain_m = gain;
    bias_m = 0;
}

void Agc::work(std::complex<float> *in, int length, std::complex<float> *out)
{
    for (int i = 0; i < length; i++)
    {
        float rho;

        bias_m = (bias_m * ((std::complex<float>)(AGC_BIAS_WINSIZE - 1)) + in[i]) / ((std::complex<float>)(AGC_BIAS_WINSIZE));
        in[i] -= bias_m;

        /* Update the sample magnitude average */
        rho = sqrtf(in[i].real() * in[i].real() + in[i].imag() * in[i].imag());
        avg_m = (avg_m * (window_size_m - 1) + rho) / window_size_m;

        gain_m = target_ampl_m / avg_m;
        if (gain_m > AGC_MAX_GAIN)
        {
            gain_m = AGC_MAX_GAIN;
        }
        out[i] = in[i] * gain_m;
    }
}

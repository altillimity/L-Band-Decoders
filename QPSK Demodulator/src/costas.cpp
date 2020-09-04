#include "costas.h"

/* Based on the Costas Loop from GNU Radio (https://github.com/gnuradio/gnuradio) */

Costas::Costas(float loop_bw, unsigned int order) : d_error(0), d_noise(1.0), d_use_snr(false), d_order(order), d_phase(0), d_freq(0), d_max_freq(1.0), d_min_freq(-1.0)
{
    d_damping = sqrtf(2.0f) / 2.0f;

    // Set the bandwidth, which will then call update_gains()
    set_loop_bandwidth(loop_bw);
}

static inline float branchless_clip(float x, float clip)
{
    return 0.5 * (std::abs(x + clip) - std::abs(x - clip));
}

static inline void fast_cc_multiply(std::complex<float> &out, const std::complex<float> cc1, const std::complex<float> cc2)
{
    // The built-in complex.h multiply has significant NaN/INF checking that
    // considerably slows down performance.  While on some compilers the
    // -fcx-limit-range flag can be used, this fast function makes the math consistent
    // in terms of performance for the Costas loop.
    float o_r, o_i;

    o_r = (cc1.real() * cc2.real()) - (cc1.imag() * cc2.imag());
    o_i = (cc1.real() * cc2.imag()) + (cc1.imag() * cc2.real());

    out.real(o_r);
    out.imag(o_i);
}

inline void sincosf(float x, float *sinx, float *cosx)
{
    *sinx = ::sinf(x);
    *cosx = ::cosf(x);
}

static inline std::complex<float> gr_expj(float phase)
{
    float t_imag, t_real;
    sincosf(phase, &t_imag, &t_real);
    return std::complex<float>(t_real, t_imag);
}

void Costas::work(std::complex<float> *in, size_t length)
{
    // Get this out of the for loop if not used:

    for (int i = 0; i < length; i++)
    {
        const std::complex<float> nco_out = gr_expj(-d_phase);

        fast_cc_multiply(in[i], in[i], nco_out);

        // EXPENSIVE LINE with function pointer, switch was about 20% faster in testing.
        // Left in for logic justification/reference. d_error = phase_detector_2(optr[i]);
        switch (d_order)
        {
        case 2:
            d_error = phase_detector_2(in[i]);
            break;
        case 4:
            d_error = phase_detector_4(in[i]);
            break;
        case 8:
            d_error = phase_detector_8(in[i]);
            break;
        }
        d_error = branchless_clip(d_error, 1.0);

        advance_loop(d_error);
        phase_wrap();
        frequency_limit();
    }
}
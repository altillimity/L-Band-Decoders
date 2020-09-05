#include "rrc_filter.h"
#include <memory.h>

/* Based on the RRC Filter from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) */

/*Static functions {{{*/
/* Variable alpha RRC filter coefficients */
/* Taken from https://www.michael-joost.de/rrcfilter.pdf */
float compute_rrc_coeff(int stage_no, unsigned taps, float osf, float alpha)
{
    float coeff;
    float t;
    float interm;
    int order;

    order = (taps - 1) / 2;

    /* Handle the 0/0 case */
    if (order == stage_no)
    {
        return 1 - alpha + 4 * alpha / M_PI;
    }

    t = abs(order - stage_no) / osf;
    coeff = sin(M_PI * t * (1 - alpha)) + 4 * alpha * t * cos(M_PI * t * (1 + alpha));
    interm = M_PI * t * (1 - (4 * alpha * t) * (4 * alpha * t));

    return coeff / interm;
}

RRCFilter::RRCFilter(unsigned order, unsigned factor, float osf, float alpha)
{
    unsigned taps;
    double *coeffs;

    taps = order * 2 + 1;

    coeffs = (double *)malloc(sizeof(*coeffs) * taps);
    /* Compute the filter coefficients */
    for (i = 0; i < taps; i++)
    {
        coeffs[i] = compute_rrc_coeff(i, taps, osf * factor, alpha);
    }

    fwd_count = taps;

    if (fwd_count)
    {
        /* Initialize the filter memory nodes and forward coefficients */
        fwd_coeff = (float *)malloc(sizeof(*fwd_coeff) * fwd_count);
        mem = (std::complex<float> *)malloc(sizeof(*mem) * fwd_count);
        for (i = 0; i < fwd_count; i++)
        {
            fwd_coeff[i] = (float)coeffs[i];
        }
    }

    free(coeffs);
}

RRCFilter::~RRCFilter()
{
    if (mem)
        free(mem);
    if (fwd_count)
        free(fwd_coeff);
}

void RRCFilter::work(std::complex<float> *in, size_t length, std::complex<float> *out)
{
    for (int ii = 0; ii < length; ii++)
    {
        /* Update the memory nodes */
        memmove(mem + 1, mem, sizeof(*mem) * (fwd_count - 1));
        mem[0] = in[ii];

        /* Calculate the feed-forward output */
        out[ii] = in[ii] * fwd_coeff[0];
        for (i = fwd_count - 1; i > 0; i--)
        {
            out[ii] += mem[i] * fwd_coeff[i];
        }
    }
}
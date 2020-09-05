#include <complex>

/* Based on the RRC Filter from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) */

class RRCFilter
{
public:
	RRCFilter(unsigned order, unsigned factor, float osf, float alpha);
	~RRCFilter();
	void work(std::complex<float> *in, size_t length, std::complex<float> *out);

private:
	std::complex<float> *mem;
	unsigned fwd_count;
	unsigned stage_no;
	float *fwd_coeff;
	size_t i;
};
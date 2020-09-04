#pragma once

/* Based on the MM Clock Recovery from goestools (https://github.com/pietern/goestools) */

#include <complex>
#include <vector>
#include <memory>

class ClockRecovery
{
public:
  explicit ClockRecovery(uint32_t sampleRate, float symbolRate);

  //void setSamplePublisher(std::unique_ptr<SamplePublisher> samplePublisher)
  //{
  //  samplePublisher_ = std::move(samplePublisher);
  //}

  // See http://www.trondeau.com/blog/2011/8/13/control-loop-gain-values.html
  void setLoopBandwidth(float bw);

  // Returns number of samples per symbol.
  float getOmega() const
  {
    return omega_;
  }

  int work(std::complex<float> *in, size_t length, std::complex<float> *out);

protected:
  float omega_;
  float omegaMin_;
  float omegaMax_;
  float omegaGain_;
  float mu_;
  float muGain_;

  // Past samples
  std::complex<float> p0t_;
  std::complex<float> p1t_;
  std::complex<float> p2t_;

  // Past associated quadrants
  std::complex<float> c0t_;
  std::complex<float> c1t_;
  std::complex<float> c2t_;

  std::vector<std::complex<float>> tmp_;
  //std::complex<float> tmp_[8192];

  //std::unique_ptr<SamplePublisher> samplePublisher_;
};

#pragma once

#include <complex>
#include <vector>
#include <memory>

class ClockRecovery
{
public:
  explicit ClockRecovery(float omega, float gain_omega, float mu, float gain_mu, float omega_limit);

  // Returns number of samples per symbol.
  float getOmega() const
  {
    return omega_;
  }

  std::vector<std::complex<float>> work(std::vector<std::complex<float>>, size_t size);

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

  //std::unique_ptr<SamplePublisher> samplePublisher_;
};

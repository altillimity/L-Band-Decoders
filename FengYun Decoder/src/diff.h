#include <vector>
#include <stdint.h>

class FengyunDiff
{
private:
    unsigned char Xin_1, Yin_1, Xin, Yin, Xout, Yout;
    char inBuf = 0;
    uint8_t buffer[2];

public:
    std::vector<uint8_t> work(std::vector<uint8_t> in);
};
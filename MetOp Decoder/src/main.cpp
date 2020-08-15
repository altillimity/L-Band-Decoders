#include <iostream>
#include <fstream>
#include <complex>
#include <vector>

#include "viterbi.h"

#include "getopt/getopt.h"
// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

// Processing buffer size
#define BUFFER_SIZE (8738 * 5)

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " -v 0.165 -o 5 inputfile.raw outputframes.bin" << std::endl;
		std::cout << "		    -o (outsinc after decode frame number(default: 5))" << std::endl;
		std::cout << "		    -v (viterbi treshold(default: 0.170))" << std::endl;
		std::cout << "2020-08-15." << std::endl;
        return 1;
    }
	int outsinc = 5;
	float vittres = 0.170;
	int sw = 0;
	while ((sw = getopt (argc, argv, "o:v:")) != -1)
    switch (sw)
      {
      case 'o':
        outsinc = std::atof(optarg);
        break;
	  case 'v':
        vittres = std::atof(optarg);
        break;
      default:
        break;
      }
    // Output and Input file
    std::ifstream data_in(argv[argc - 2], std::ios::binary);
    std::ofstream data_out(argv[argc - 1], std::ios::binary);

    // MetOp Viterbi decoder
    MetopViterbi viterbi(true, vittres, 1, outsinc, 50);

    // Viterbi output buffer
    uint8_t *viterbi_out = new uint8_t[BUFFER_SIZE];

    // Read buffer
    std::complex<float> buffer[BUFFER_SIZE];

    // Complete filesize
    size_t filesize = getFilesize(argv[argc - 2]);

    // Data we wrote out
    size_t data_out_total = 0;

    std::cout << "---------------------------" << std::endl;
    std::cout << "MetOp Decoder by Aang23" << std::endl;
    std::cout << "Fixed by Tomi HA6NAB" << std::endl;
    std::cout << "---------------------------" << std::endl;
	std::cout << "Viterbi treshold: "<< vittres << std::endl;
	std::cout << "Outsinc after: "<< outsinc << std::endl;
	std::cout << "---------------------------" << std::endl;
    std::cout << std::endl;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, sizeof(std::complex<float>) * BUFFER_SIZE);

        // Push into Viterbi
        int num_samp = viterbi.work(buffer, BUFFER_SIZE, viterbi_out);

        data_out_total += num_samp;

        // Write output
        if (num_samp > 0)
            data_out.write((char *)viterbi_out, num_samp);

        // Console stuff
        std::cout << '\r' << "Viterbi : " << (viterbi.getState() == 0 ? "NO SYNC" : viterbi.getState() == 1 ? "SYNCING" : "SYNCED") << ", Data out : " << round(data_out_total / 1e5) / 10.0f << " MB, Progress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl
              << "Done! Enjoy" << std::endl;

    data_in.close();
    data_out.close();
}
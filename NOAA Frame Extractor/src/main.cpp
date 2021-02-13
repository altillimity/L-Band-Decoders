#include <iostream>
#include "noaa_frame_reader.h"

int main(int argc, char *argv[]){

    std::cout << "----------------------" << std::endl;
    std::cout << " NOAA Frame Extractor " << std::endl;
    std::cout << "       by Zbychu      " << std::endl;
    std::cout << "----------------------" << std::endl;
    std::cout << std::endl;

    std::string type = argv[3];

    NOAAFrameReader frame_reader;
    std::vector<std::array<uint8_t, 104>> frames;
    if (type == "tip" | type == "TIP")
    {
        frames = frame_reader.readFrames(std::ifstream(argv[1], std::ios::binary), 1);
    }
    else if (type == "aip" | type == "AIP")
    {
        frames = frame_reader.readFrames(std::ifstream(argv[1], std::ios::binary), 3);
    }
    std::ofstream out(argv[2]);
    for (int i = 0; i < frames.size(); i++){
        out.write((char *)&frames[i][0], 104);
    }
    out.close();
    std::cout<<"Frames : "<<frames.size()<<std::endl;
}
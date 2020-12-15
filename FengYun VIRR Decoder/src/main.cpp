#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include "virr_deframer.h"
#include "virr_reader.h"
#include "xfr.h"

// Return filesize
size_t getFilesize(std::string filepath)
{
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    std::size_t fileSize = file.tellg();
    file.close();
    return fileSize;
}

int main(int argc, char *argv[])
{
    // Output and Input file
    std::ifstream data_in(argv[1], std::ios::binary);

    // Read buffer
    uint8_t *buffer = new uint8_t[1024];

    // Deframer
    VIRRDeframer virrDefra;

    VIRRReader reader;

    // Graphics
    std::cout << "----------------------------" << std::endl;
    std::cout << "      FengYun-3 (A/B/C)" << std::endl;
    std::cout << "   VIRR Decoder by Aang23" << std::endl;
    std::cout << " with xfr support by Zbychu" << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout << std::endl;

    std::cout << "Demultiplexing and deframing..." << std::endl;

    int vcidFrames = 0, virr_frames = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        if (argc != 2 && argc != 3)
        {
            std::cout << "Usage : " << argv[0] << " inputFrames.bin" << std::endl;
            return 0;
        }

        // Complete filesize
        size_t filesize = getFilesize(argv[1]);

        // Read buffer
        data_in.read((char *)buffer, 1024);

        // Extract VCID
        int vcid = buffer[5] % ((int)pow(2, 6));

        if (vcid == 5)
        {
            vcidFrames++;

            // Deframe MERSI
            std::vector<uint8_t> defraVec;
            defraVec.insert(defraVec.end(), &buffer[14], &buffer[14 + 882]);
            std::vector<std::vector<uint8_t>> out = virrDefra.work(defraVec);

            for (std::vector<uint8_t> frameVec : out)
            {
                virr_frames++;
                reader.work(frameVec);
            }
        }

        // Show our progress
        std::cout << "\rProgress : " << round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f << "%     " << std::flush;
    }

    std::cout << std::endl;
    std::cout << std::endl;

    // Say what we found
    std::cout << "VCID 5 Frames         : " << vcidFrames << std::endl;
    std::cout << "VIRR Frames           : " << virr_frames << std::endl;
    std::cout << "VIRR Lines            : " << reader.lines << std::endl;

    std::cout << std::endl;

    // Write images out
    std::cout << "Writing images... (Can take a while)" << std::endl;

    cimg_library::CImg<unsigned short> image1 = reader.getChannel(0);
    cimg_library::CImg<unsigned short> image2 = reader.getChannel(1);
    cimg_library::CImg<unsigned short> image3 = reader.getChannel(2);
    cimg_library::CImg<unsigned short> image4 = reader.getChannel(3);
    cimg_library::CImg<unsigned short> image5 = reader.getChannel(4);
    cimg_library::CImg<unsigned short> image6 = reader.getChannel(5);
    cimg_library::CImg<unsigned short> image7 = reader.getChannel(6);
    cimg_library::CImg<unsigned short> image8 = reader.getChannel(7);
    cimg_library::CImg<unsigned short> image9 = reader.getChannel(8);
    cimg_library::CImg<unsigned short> image10 = reader.getChannel(9);

    // Takes a while so we say how we're doing
    std::cout << "Channel 1..." << std::endl;
    image1.save_png("VIRR-1.png");

    std::cout << "Channel 2..." << std::endl;
    image2.save_png("VIRR-2.png");

    std::cout << "Channel 3..." << std::endl;
    image3.save_png("VIRR-3.png");

    std::cout << "Channel 4..." << std::endl;
    image4.save_png("VIRR-4.png");

    std::cout << "Channel 5..." << std::endl;
    image5.save_png("VIRR-5.png");

    std::cout << "Channel 6..." << std::endl;
    image6.save_png("VIRR-6.png");

    std::cout << "Channel 7..." << std::endl;
    image7.save_png("VIRR-7.png");

    std::cout << "Channel 8..." << std::endl;
    image8.save_png("VIRR-8.png");

    std::cout << "Channel 9..." << std::endl;
    image9.save_png("VIRR-9.png");

    std::cout << "Channel 10..." << std::endl;
    image10.save_png("VIRR-10.png");

    std::cout << "221 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image221(2048, reader.lines, 1, 3);
    {
        image221.draw_image(0, 0, 0, 0, image2);
        image221.draw_image(0, 0, 0, 1, image2);
        image221.draw_image(0, 0, 0, 2, image1);
    }
    image221.save_png("VIRR-RGB-221.png");
    image221.equalize(1000);
    image221.normalize(0, std::numeric_limits<unsigned char>::max());
    image221.save_png("VIRR-RGB-221-EQU.png");

    std::cout << "621 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image621(2048, reader.lines, 1, 3);
    {
        image621.draw_image(2, 1, 0, 0, image6);
        image621.draw_image(0, 0, 0, 1, image2);
        image621.draw_image(0, 0, 0, 2, image1);
    }
    image621.save_png("VIRR-RGB-621.png");
    image621.equalize(1000);
    image621.normalize(0, std::numeric_limits<unsigned char>::max());
    image621.save_png("VIRR-RGB-621-EQU.png");

    std::cout << "197 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image197(2048, reader.lines, 1, 3);
    {
        image197.draw_image(1, 0, 0, 0, image1);
        image197.draw_image(0, 0, 0, 1, image9);
        image197.draw_image(-2, 0, 0, 2, image7);
    }
    image197.save_png("VIRR-RGB-197.png");
    cimg_library::CImg<unsigned short> image197equ(2048, reader.lines, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage9 = image9, tempImage1 = image1, tempImage7 = image7;
        tempImage9.equalize(1000);
        tempImage1.equalize(1000);
        tempImage7.equalize(1000);
        image197equ.draw_image(1, 0, 0, 0, tempImage1);
        image197equ.draw_image(0, 0, 0, 1, tempImage9);
        image197equ.draw_image(-2, 0, 0, 2, tempImage7);
        image197equ.equalize(1000);
        image197equ.normalize(0, std::numeric_limits<unsigned char>::max());
    }
    image197equ.save_png("VIRR-RGB-197-EQU.png");

    std::cout << "917 Composite..." << std::endl;
    cimg_library::CImg<unsigned short> image917(2048, reader.lines, 1, 3);
    {
        image917.draw_image(0, 0, 0, 0, image9);
        image917.draw_image(1, 0, 0, 1, image1);
        image917.draw_image(-1, 0, 0, 2, image7);
    }
    image917.save_png("VIRR-RGB-917.png");
    cimg_library::CImg<unsigned short> image917equ(2048, reader.lines, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage9 = image9, tempImage1 = image1, tempImage7 = image7;
        tempImage9.equalize(1000);
        tempImage1.equalize(1000);
        tempImage7.equalize(1000);
        image917equ.draw_image(0, 0, 0, 0, tempImage9);
        image917equ.draw_image(1, 0, 0, 1, tempImage1);
        image917equ.draw_image(-1, 0, 0, 2, tempImage7);
        image917equ.equalize(1000);
        image917equ.normalize(0, std::numeric_limits<unsigned char>::max());
    }
    image917equ.save_png("VIRR-RGB-917-EQU.png");

    std::cout << "197 True Color XFR Composite... (by ZbychuButItWasTaken)" << std::endl;
    cimg_library::CImg<unsigned short> image197truecolorxfr(2048, reader.lines, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage1 = image1, tempImage9 = image9, tempImage7 = image7;

        XFR trueColor(26, 663, 165, 34, 999, 162, 47, 829, 165);

        applyXFR(trueColor, tempImage1, tempImage9, tempImage7);

        image197truecolorxfr.draw_image(1, 0, 0, 0, tempImage1);
        image197truecolorxfr.draw_image(0, 0, 0, 1, tempImage9);
        image197truecolorxfr.draw_image(-2, 0, 0, 2, tempImage7);
    }
    image197truecolorxfr.save_png("VIRR-RGB-197-TRUECOLOR.png");

    std::cout << "197 Night XFR Composite... (by ZbychuButItWasTaken)" << std::endl;
    cimg_library::CImg<unsigned short> image197nightxfr(2048, reader.lines, 1, 3);
    {
        cimg_library::CImg<unsigned short> tempImage1 = image1, tempImage9 = image9, tempImage7 = image7;

        XFR trueColor(23, 610, 153, 34, 999, 162, 39, 829, 165);

        applyXFR(trueColor, tempImage1, tempImage9, tempImage7);

        image197nightxfr.draw_image(1, 0, 0, 0, tempImage1);
        image197nightxfr.draw_image(0, 0, 0, 1, tempImage9);
        image197nightxfr.draw_image(-2, 0, 0, 2, tempImage7);
    }
    image197nightxfr.save_png("VIRR-RGB-197-NIGHT.png");

    data_in.close();
}
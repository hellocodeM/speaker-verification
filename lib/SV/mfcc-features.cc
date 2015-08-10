/**
 * Description: MFCC特征提取
 * Author: HelloCodeMing
 * Time: 2015-08-10 21:38:44
 */

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>

#include <ming/all.hpp>
#include <aquila/aquila.h>

int main(int argc, char *argv[])
{
    using namespace Aquila;
    const size_t input_size = 1024;
    const size_t frame_size = 1024;
    const size_t frame_overlap = 512;

    if (argc < 2) {
        ming::println("You should add an .wav");
        return 1;
    }
    Aquila::WaveFile wave(argv[1]);
    Aquila::Mfcc mfcc(input_size);
    std::vector<std::vector<Aquila::SampleType>> features;
    
    // wave property
    //ming::printf("wave name: %\n", wave.getFilename());
    //ming::printf("samples count: %\n", wave.getSamplesCount());
    //ming::printf("sample frequency: %\n", wave.getSampleFrequency());
    //ming::printf("audio length: %\n", wave.getAudioLength());
    //ming::printf("bits per sample: %\n", wave.getBitsPerSample());

    // mfcc
    FramesCollection frames(wave, frame_size, frame_overlap);
    features.reserve(frames.count());
    for (auto frame : frames) {
        features.emplace_back(mfcc.calculate(frame));
    }

    for (auto&& i: features)
        ming::println(i);
    return 0;
}

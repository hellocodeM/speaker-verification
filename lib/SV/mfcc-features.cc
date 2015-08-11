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
    const size_t frame_size = 1024;
    const size_t frame_overlap = 512;

    if (argc < 2) {
        ming::println("You should add an .wav");
        return 1;
    }
    WaveFile wave(argv[1]);
    Mfcc mfcc(frame_size);
    std::vector<std::vector<SampleType>> features;
    
    // wave property
    //ming::printf("wave name: %\n", wave.getFilename());
    //ming::printf("samples count: %\n", wave.getSamplesCount());
    //ming::printf("sample frequency: %\n", wave.getSampleFrequency());
    //ming::printf("audio length: %\n", wave.getAudioLength());
    //ming::printf("bits per sample: %\n", wave.getBitsPerSample());

    // mfcc
    FramesCollection frames(wave, frame_size, frame_overlap);
    HannWindow hann(frame_size);
    features.reserve(frames.count());
    for (auto frame : frames) {
        SignalSource sig(frame.toArray(), frame.length());
        sig *= hann;
        features.emplace_back(mfcc.calculate(sig.toArray()));
    }

    for (auto&& i: features)
        ming::println(i);
    return 0;
}

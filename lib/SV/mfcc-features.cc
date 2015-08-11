/**
 * Description: MFCC特征提取
 * Author: HelloCodeMing
 * Time: 2015-08-10 21:38:44
 */

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>

#include <aquila/aquila.h>

int main(int argc, char *argv[])
{
    using namespace Aquila;
    constexpr size_t frame_size = 1024;
    constexpr size_t frame_overlap = 512;

    if (argc < 2) {
        ming::println("You should add an .wav");
        return 1;
    }
    WaveFile wave(argv[1]);
    Mfcc mfcc(frame_size);
    HannWindow hann(frame_size);
    FramesCollection frames(wave, frame_size, frame_overlap);
    std::vector<std::vector<SampleType>> features;

    // mfcc
    features.reserve(frames.count());
    for (auto frame : frames) {
        SignalSource sig(frame.toArray(), frame.length(), frame.getSampleFrequency());
        sig *= hann;
        features.emplace_back(mfcc.calculate(sig));
    }

    for (auto&& i : res) {
		for (auto j : i)
			std::cout << j << " ";
		std::cout << "\n";
	}
    return 0;
}

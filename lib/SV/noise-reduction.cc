/**
 * Description: 使用频谱减法进行语音降噪
 * Author: HelloCodeMIng
 * Time: 2015-08-10 21:39:52
 */


#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include <utility>

#include <ming/printf.hpp>
#include <aquila/aquila.h>

#include "plot.hpp"

using namespace Aquila;

SpectrumType GetNoiseSpectrum(const SignalSource& noise_src, size_t frame_size) {
    constexpr size_t frame_skip = 20;
    constexpr size_t N = 50;

    auto fft = FftFactory::getFft(frame_size);
    SpectrumType spectrum(frame_size);
    FramesCollection frames(noise_src, frame_size, 0);
    

    int end = std::min(frames.count(), frame_skip + N);
    for (int i = frame_skip; i < end; i++) {
        auto frame = *(frames.begin() + i);
        SignalSource segment(frame.toArray(), frame.length());

        auto segment_spectrum = fft->fft(segment.toArray());
        std::transform(
                spectrum.begin(), spectrum.end(), segment_spectrum.begin(), spectrum.begin(),
                [](auto x, auto y) { 
                double abs = std::abs(x) + std::abs(y);
                return std::polar(abs, std::arg(x));
        });
    }
    for (auto& i : spectrum) {
        i /= end - frame_skip;
    }
    return spectrum;
}

void Subtract(SpectrumType& voice, const SpectrumType& noise, double alpha, double beta) {
    assert(voice.size() >= noise.size());
    std::transform(
            voice.begin(), voice.end(), noise.begin(), voice.begin(),
            [=](ComplexType lhs, ComplexType rhs) {
            double abs = std::abs(lhs) - alpha * std::abs(rhs);
            if (abs <= 0)
                abs = beta * std::abs(rhs);
            return std::polar(abs, std::arg(lhs));
    });
}

inline double SNR(double signal, double noise) noexcept {
    return 10.0 * std::log10(signal / noise);
}

constexpr double berouti(double snr) {
    if (-5.0 <= snr && snr <= 20.0)
        return 4 - snr * 3 / 20;
    else if (snr < -5.0)
        return 5;
    return 1;
}

std::shared_ptr<SignalSource> NoiseReduction(
        const SignalSource& voice, 
        const SignalSource& noise,
        size_t frame_size,
        size_t frame_overlap) {
    constexpr double beta = 0.002;
    const double noise_power = power(noise);

    HannWindow hann(frame_size);
    auto noise_spectrum = GetNoiseSpectrum(noise, frame_size);
    auto fft = FftFactory::getFft(frame_size);
    FramesCollection frames(voice, frame_size, frame_overlap);
    std::vector<SampleType> output(voice.length());

    double* sig_out = new double[frame_size];
    size_t step = frame_size - frame_overlap;
    auto output_iter = output.begin();
    for (auto frame : frames) {
        SignalSource sig(frame.toArray(), frame.length());
        double snr = SNR(power(sig), noise_power);
        double alpha = berouti(snr);

        // substract noise
        sig *= hann;
        auto spectrum = fft->fft(sig.toArray());
        Subtract(spectrum, noise_spectrum, alpha, beta);

        fft->ifft(spectrum, sig_out);
        std::transform(
                output_iter,
                std::next(output_iter, frame_size), 
                sig_out, 
                output_iter,
                [](auto x, auto y) { return x + y; });
        std::advance(output_iter, step);
    }
    delete[] sig_out;
    return std::make_shared<SignalSource>(std::move(output), voice.getSampleFrequency());
}

int main()
{
    // configuration
    const size_t frame_size = 1024;
    const size_t frame_overlap = 512;
    const char* wave_path = "data/music.wav";
    const char* noise_path = "data/music.wav";
    const char* output_path = "data/music_out.wav";
    
    TextPlot plot;
    WaveFile wave(wave_path);
    WaveFile noise_src(noise_path);

    /* plot the noise spectrum */
    //PlotSpectrum(GetNoiseSpectrum(noise_src, frame_size));
    
    /* noise reduction */
    auto voice_out = NoiseReduction(wave, noise_src, frame_size, frame_overlap);

    /* plot input and output signal */
    //PlotSignal(wave);
    //PlotSignal(*voice_out);

    WaveFile::save(*voice_out, output_path);
    return 0;
}

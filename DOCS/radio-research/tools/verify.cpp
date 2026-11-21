// Verifies the DSP figures quoted in the roadmap issue, using the repo's own receiver code.
#include "fmreceiver.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdio>
#include <random>
#include <vector>

constexpr double kPi = 3.14159265358979323846;

// Same design as fm::Fir (windowed sinc, Hamming, normalised to unity DC gain)
std::vector<double> firTaps(int taps, double cutoff, double rate)
{
    std::vector<double> h(static_cast<size_t>(taps));
    double sum = 0, fc = cutoff / rate;
    for (int i = 0; i < taps; ++i) {
        double x = i - (taps - 1) / 2.0;
        double s = std::abs(x) < 1e-9 ? 2 * fc : std::sin(2 * kPi * fc * x) / (kPi * x);
        h[size_t(i)] = s * (0.54 - 0.46 * std::cos(2 * kPi * i / (taps - 1)));
        sum += h[size_t(i)];
    }
    for (auto &v : h) v /= sum;
    return h;
}
double firGainDb(const std::vector<double> &h, double f, double rate)
{
    std::complex<double> acc = 0;
    for (size_t n = 0; n < h.size(); ++n) acc += h[n] * std::polar(1.0, -2 * kPi * f / rate * double(n));
    return 20 * std::log10(std::abs(acc));
}

// Stereo transmitter with a chosen pilot frequency, tones L 1 kHz only (R silent)
double gLevel = 0.8, gNoise = 0.004, gAmp = 0.4;
std::vector<uint8_t> transmit(double pilotHz, double seconds)
{
    const int rate = fm::kIqRate;
    size_t n = size_t(seconds * rate);
    std::vector<uint8_t> iq(n * 2);
    std::mt19937 rng(7);
    std::normal_distribution<float> g(0.f, 1.f);
    double phase = 0;
    for (size_t k = 0; k < n; ++k) {
        double t = double(k) / rate, w = 2 * kPi * pilotHz * t;
        double l = gLevel * std::sin(2 * kPi * 1000 * t), r = 0;
        double mpx = 0.9 * (l + r) / 2 + 0.09 * std::sin(w) + 0.9 * (l - r) / 2 * std::sin(2 * w);
        phase += 2 * kPi * 75000.0 * mpx / rate;
        double c = 2 * kPi * fm::kOffsetHz * t + phase;
        double i = gAmp * std::cos(c) + gNoise * g(rng), q = gAmp * std::sin(c) + gNoise * g(rng);
        iq[k * 2] = uint8_t(std::lround(std::clamp(i * 127.5 + 127.5, 0.0, 255.0)));
        iq[k * 2 + 1] = uint8_t(std::lround(std::clamp(q * 127.5 + 127.5, 0.0, 255.0)));
    }
    return iq;
}
double tone(const std::vector<int16_t> &pcm, int ch, double hz, size_t skip)
{
    double re = 0, im = 0; size_t c = 0;
    for (size_t f = skip; f * 2 + 1 < pcm.size(); ++f, ++c) {
        double a = 2 * kPi * hz * double(f) / fm::kAudioRate, x = pcm[f * 2 + size_t(ch)] / 32768.0;
        re += x * std::cos(a); im += x * std::sin(a);
    }
    return 2 * std::sqrt(re * re + im * im) / double(c);
}

int main()
{
    std::printf("== De-emphasis at 57 kHz vs analogue 75 us ==\n");
    const double fs = fm::kAudioRate, tau = 75e-6, p = std::exp(-1 / (fs * tau)), q = -0.1166;
    double maxOne = 0, maxZero = 0;
    for (double f = 20; f <= 15000; f += 10) {
        double w = 2 * kPi * f / fs;
        std::complex<double> z = std::polar(1.0, -w);
        double one = std::abs((1 - p) / (1.0 - p * z));
        double two = std::abs((1 - p) / (1 - q) * (1.0 - q * z) / (1.0 - p * z));
        double ana = 1 / std::sqrt(1 + std::pow(2 * kPi * f * tau, 2));
        maxOne = std::max(maxOne, std::abs(20 * std::log10(one / ana)));
        maxZero = std::max(maxZero, std::abs(20 * std::log10(two / ana)));
        if (f == 10000 || f == 15000)
            std::printf("  %5.0f Hz: one-pole %+.2f dB, with zero %+.3f dB\n", f, 20 * std::log10(one / ana), 20 * std::log10(two / ana));
    }
    std::printf("  max error 20 Hz-15 kHz: one-pole %.2f dB, with zero q=-0.1166 %.3f dB\n", maxOne, maxZero);

    std::printf("== Pilot biquad phase slope (Q=95 at 228 kHz) ==\n");
    const double mr = fm::kMpxRate, w0 = 2 * kPi * 19000 / mr, al = std::sin(w0) / (2 * 95.0);
    auto bpPhase = [&](double f) {
        std::complex<double> z1 = std::polar(1.0, -2 * kPi * f / mr), z2 = z1 * z1;
        std::complex<double> H = (al / (1 + al)) * (1.0 - z2) / (1.0 + (-2 * std::cos(w0) / (1 + al)) * z1 + ((1 - al) / (1 + al)) * z2);
        return std::arg(H) * 180 / kPi;
    };
    std::printf("  phase at 19000: %.3f deg; at 19001: %.3f; at 19002: %.3f (slope %.3f deg/Hz)\n",
                bpPhase(19000), bpPhase(19001), bpPhase(19002), bpPhase(19001) - bpPhase(19000));

    std::printf("== Simulated separation vs pilot frequency (repo receiver) ==\n");
    for (double ph : { 19000.0, 19001.0, 19002.0, 18998.0 }) {
        auto iq = transmit(ph, 5);
        fm::Receiver rx; std::vector<int16_t> pcm;
        for (size_t k = 0; k < iq.size(); k += 16384) rx.process(iq.data() + k, std::min<size_t>(16384, iq.size() - k), pcm);
        double L = tone(pcm, 0, 1000, fm::kAudioRate * 2), R = tone(pcm, 1, 1000, fm::kAudioRate * 2);
        std::printf("  pilot %.0f Hz: separation %.1f dB (blend %.2f)\n", ph, 20 * std::log10(L / std::max(R, 1e-9)), double(rx.quality().blend));
    }

    std::printf("== What limits separation? (pilot 19000) ==\n");
    struct Case { const char *name; double level, noise, amp; };
    for (Case c : { Case{ "baseline 0.8 level, noise", 0.8, 0.004, 0.4 }, Case{ "no noise", 0.8, 0.0, 0.4 },
                    Case{ "low level 0.2", 0.2, 0.004, 0.4 }, Case{ "low level 0.2, no noise", 0.2, 0.0, 0.4 },
                    Case{ "full level 1.0, no noise", 1.0, 0.0, 0.4 } }) {
        gLevel = c.level; gNoise = c.noise; gAmp = c.amp;
        auto iq = transmit(19000, 5);
        fm::Receiver rx; std::vector<int16_t> pcm;
        for (size_t k = 0; k < iq.size(); k += 16384) rx.process(iq.data() + k, std::min<size_t>(16384, iq.size() - k), pcm);
        double L = tone(pcm, 0, 1000, fm::kAudioRate * 2), R = tone(pcm, 1, 1000, fm::kAudioRate * 2);
        std::printf("  %-28s separation %.1f dB  (L %.3f, R %.5f)\n", c.name, 20 * std::log10(L / std::max(R, 1e-9)), L, R);
    }
    gLevel = 0.8; gNoise = 0.004; gAmp = 0.4;

    std::printf("== 19 kHz rejection of the 189-tap 17 kHz audio filter at 228 kHz ==\n");
    auto h = firTaps(189, 17000, fm::kMpxRate);
    for (double f : { 1000.0, 15000.0, 17000.0, 19000.0 }) std::printf("  %5.0f Hz: %+.2f dB\n", f, firGainDb(h, f, fm::kMpxRate));
}

// Probes of the Godwit FM receiver, using its own code and a copy of its test transmitter.
#include "fmreceiver.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

constexpr double kPi = 3.14159265358979323846;

uint32_t blockWord(uint16_t info, uint16_t offset)
{
    uint32_t rem = uint32_t(info) << 10;
    for (int i = 25; i >= 10; --i)
        if ((rem >> i) & 1) rem ^= 0x5B9u << (i - 10);
    return (uint32_t(info) << 10) | ((rem & 0x3FF) ^ offset);
}
std::vector<int> rdsBits(uint16_t pi, const char *ps)
{
    std::vector<int> bits;
    for (int round = 0; round < 40; ++round)
        for (int seg = 0; seg < 4; ++seg) {
            const uint32_t w[4] = { blockWord(pi, 0x0FC), blockWord(uint16_t(seg), 0x198), blockWord(0xE0CD, 0x168),
                                    blockWord(uint16_t((uint8_t(ps[seg * 2]) << 8) | uint8_t(ps[seg * 2 + 1])), 0x1B4) };
            for (uint32_t x : w) for (int i = 25; i >= 0; --i) bits.push_back((x >> i) & 1);
        }
    return bits;
}

struct Tx {
    double seconds = 8;
    bool pilot = true, rds = true;
    double rdsCarrierHz = 57000;   // the station's own 57 kHz (crystal error moves it)
    // audio: "tones" or "loud" (dense, clipped, heavily processed programme)
    bool loud = false;
    double noise = 0.004;
};

std::vector<uint8_t> transmit(const Tx &t, const std::vector<int> &bits)
{
    const int rate = fm::kIqRate;
    const size_t n = size_t(t.seconds * rate);
    std::vector<uint8_t> iq(n * 2);
    std::mt19937 rng(1234);
    std::normal_distribution<float> g(0.f, 1.f);
    // dense programme: band-limited noise per channel, clipped hard like an over-processed station
    double lpL = 0, lpR = 0, lp2L = 0, lp2R = 0;
    double phase = 0; int prev = 0, sym = -1; double level = 0;
    for (size_t k = 0; k < n; ++k) {
        const double tt = double(k) / rate;
        const double w = 2 * kPi * 19000.0 * tt;
        double l, r;
        if (t.loud) {
            lpL += 0.08 * (g(rng) - lpL); lp2L += 0.08 * (lpL - lp2L);
            lpR += 0.08 * (g(rng) - lpR); lp2R += 0.08 * (lpR - lp2R);
            l = std::clamp(lp2L * 6.0, -1.0, 1.0);
            r = std::clamp(lp2R * 6.0, -1.0, 1.0);
        } else {
            l = 0.8 * std::sin(2 * kPi * 1000 * tt);
            r = 0.8 * std::sin(2 * kPi * 3000 * tt);
        }
        double mpx = 0.9 * (l + r) / 2;
        if (t.pilot) {
            mpx += 0.09 * std::sin(w);
            mpx += 0.9 * (l - r) / 2 * std::sin(2 * w);
        } else {
            mpx = 0.9 * (l + r) / 2 / 0.9 * 0.95;   // mono: use the room the pilot and L-R would take
        }
        if (t.rds) {
            const double cyc = t.rdsCarrierHz / 3.0 * tt / 16;
            const int s = int(cyc);
            if (s != sym) { sym = s; prev ^= bits[size_t(s) % bits.size()]; level = prev ? 1 : -1; }
            const double half = cyc - s < 0.5 ? level : -level;
            mpx += 0.04 * half * std::cos(2 * kPi * t.rdsCarrierHz * tt);
        }
        phase += 2 * kPi * 75000.0 * mpx / rate;
        const double c = 2 * kPi * fm::kOffsetHz * tt + phase;
        const double i = 0.4 * std::cos(c) + t.noise * g(rng);
        const double q = 0.4 * std::sin(c) + t.noise * g(rng);
        iq[k * 2] = uint8_t(std::lround(std::clamp(i * 127.5 + 127.5, 0.0, 255.0)));
        iq[k * 2 + 1] = uint8_t(std::lround(std::clamp(q * 127.5 + 127.5, 0.0, 255.0)));
    }
    return iq;
}

void run(fm::Receiver &rx, const std::vector<uint8_t> &iq, std::vector<int16_t> &pcm)
{
    for (size_t k = 0; k < iq.size(); k += 16384)
        rx.process(iq.data() + k, std::min<size_t>(16384, iq.size() - k), pcm);
}

int main()
{
    const auto bits = rdsBits(0x3AAB, "MONO FM ");

    std::printf("== 1. RDS on a station without a pilot ==\n");
    for (double off : { 0.0, 1.0, 3.0 }) {
        Tx t; t.pilot = false; t.rdsCarrierHz = 57000 + off;
        fm::Receiver rx; std::vector<int16_t> pcm;
        run(rx, transmit(t, bits), pcm);
        std::printf("  57 kHz off by %.0f Hz: groups %llu, PS '%s', call '%s'\n", off,
                    (unsigned long long)rx.rds().groups(), rx.rds().ps().c_str(), rx.rds().callSign().c_str());
    }
    {
        Tx t; t.pilot = true;
        fm::Receiver rx; std::vector<int16_t> pcm;
        run(rx, transmit(t, bits), pcm);
        std::printf("  control, stereo with pilot: groups %llu, PS '%s'\n",
                    (unsigned long long)rx.rds().groups(), rx.rds().ps().c_str());
    }

    std::printf("== 2. Stereo blend on a strong, loud, heavily processed station ==\n");
    for (bool loud : { false, true }) {
        Tx t; t.loud = loud; t.seconds = 6;
        fm::Receiver rx; std::vector<int16_t> pcm;
        run(rx, transmit(t, bits), pcm);
        std::printf("  %-26s snr %.1f dB  mpx rms %.3f  pilot %d  blend %.2f\n",
                    loud ? "loud processed programme:" : "test tones (repo's test):",
                    double(rx.quality().snrDb), double(rx.quality().mpxRms), rx.quality().pilot ? 1 : 0,
                    double(rx.quality().blend));
    }

    std::printf("== 3. De-emphasis vs the analogue 75 us curve ==\n");
    const double fs = fm::kAudioRate, a = std::exp(-1.0 / (fs * 75e-6));
    for (double f : { 1000.0, 5000.0, 10000.0, 15000.0 }) {
        const double wv = 2 * kPi * f / fs;
        const double dig = (1 - a) / std::sqrt(1 + a * a - 2 * a * std::cos(wv));
        const double ana = 1 / std::sqrt(1 + std::pow(2 * kPi * f * 75e-6, 2));
        std::printf("  %5.0f Hz: receiver %.2f dB, ideal %.2f dB, error %+.2f dB\n", f,
                    20 * std::log10(dig), 20 * std::log10(ana), 20 * std::log10(dig / ana));
    }
}

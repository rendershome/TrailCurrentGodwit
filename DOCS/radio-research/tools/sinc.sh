#!/bin/bash
# Builds the separation probe against the original receiver and against a copy whose
# L-R is scaled by 1/sinc(38k/228k), and compares separation at 1 kHz and 10 kHz.
set -e
R="$(cd "$(dirname "$0")/../../.." && pwd)/radio/src"

W=/tmp/sincfix; rm -rf $W; mkdir -p $W
cp $R/fmreceiver.h $R/fmreceiver.cpp $W/
cp $R/fmreceiver.cpp $W/fmreceiver_fixed.cpp
# 1/sinc(1/6) = (pi/6)/sin(pi/6) = 1.0471976
sed -i 's|const float d = m_diff.output() \* m_blendNow;|const float d = m_diff.output() * 1.0471976f * m_blendNow;|' $W/fmreceiver_fixed.cpp
grep -c "1.0471976f" $W/fmreceiver_fixed.cpp
cat > $W/sep.cpp <<'EOF'
#include "fmreceiver.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>
#include <vector>
constexpr double kPi = 3.14159265358979323846;
std::vector<uint8_t> tx(double hz) {
    const int rate = fm::kIqRate; size_t n = size_t(5.0 * rate);
    std::vector<uint8_t> iq(n * 2); std::mt19937 rng(7); std::normal_distribution<float> g(0.f, 1.f);
    double ph = 0;
    for (size_t k = 0; k < n; ++k) {
        double t = double(k) / rate, w = 2 * kPi * 19000.0 * t;
        double l = 0.8 * std::sin(2 * kPi * hz * t), r = 0;
        double mpx = 0.9 * (l + r) / 2 + 0.09 * std::sin(w) + 0.9 * (l - r) / 2 * std::sin(2 * w);
        ph += 2 * kPi * 75000.0 * mpx / rate;
        double c = 2 * kPi * fm::kOffsetHz * t + ph;
        double i = 0.4 * std::cos(c) + 0.004 * g(rng), q = 0.4 * std::sin(c) + 0.004 * g(rng);
        iq[k * 2] = uint8_t(std::lround(std::clamp(i * 127.5 + 127.5, 0.0, 255.0)));
        iq[k * 2 + 1] = uint8_t(std::lround(std::clamp(q * 127.5 + 127.5, 0.0, 255.0)));
    }
    return iq;
}
double tone(const std::vector<int16_t> &p, int ch, double hz) {
    double re = 0, im = 0; size_t c = 0;
    for (size_t f = fm::kAudioRate * 2; f * 2 + 1 < p.size(); ++f, ++c) {
        double a = 2 * kPi * hz * double(f) / fm::kAudioRate, x = p[f * 2 + size_t(ch)] / 32768.0;
        re += x * std::cos(a); im += x * std::sin(a);
    }
    return 2 * std::sqrt(re * re + im * im) / double(c);
}
int main() {
    for (double hz : { 1000.0, 5000.0, 10000.0, 14000.0 }) {
        auto iq = tx(hz); fm::Receiver rx; std::vector<int16_t> pcm;
        for (size_t k = 0; k < iq.size(); k += 16384) rx.process(iq.data() + k, std::min<size_t>(16384, iq.size() - k), pcm);
        double L = tone(pcm, 0, hz), R = tone(pcm, 1, hz);
        std::printf("  %5.0f Hz: separation %.1f dB\n", hz, 20 * std::log10(L / std::max(R, 1e-9)));
    }
}
EOF
cd $W
g++ -O2 -std=c++20 -I. sep.cpp fmreceiver.cpp -o sep_orig
g++ -O2 -std=c++20 -I. sep.cpp fmreceiver_fixed.cpp -o sep_fixed
echo "original receiver:"; ./sep_orig
echo "with L-R x 1/sinc(38k/228k):"; ./sep_fixed

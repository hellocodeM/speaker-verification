#ifndef SV_PLOT_HPP
#define SV_PLOT_HPP

/**
 * Description: 调用GNUPlot画图的函数
 * Author: HelloCodeMing
 * Time: 2015-08-10 21:37:05
 */

#include <cstdio>

#include <aquila/aquila.h>

namespace SV {

void DoPlot() {
    const char* cmd = R"(gnuplot -p -e "plot '/tmp/gnuplot.plot' with linespoints;")";
    system(cmd);
}

void PlotSignal(const Aquila::SignalSource& signal) {
    FILE* output = fopen("/tmp/gnuplot.plot", "w");
    for (auto i : signal) {
        fprintf(output, "%lf\n", i);
    }
    fclose(output);
    DoPlot();
}

void PlotSpectrum(const Aquila::SpectrumType& spectrum) {
    FILE* output = fopen("/tmp/gnuplot.plot", "w");
    for (auto i : spectrum) {
        fprintf(output, "%lf\n", std::abs(i));
    }
    fclose(output);
    DoPlot();
}

} /* end of namespace SV */
#endif

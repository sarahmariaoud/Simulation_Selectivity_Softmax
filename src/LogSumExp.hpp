#ifndef logsumexp_h
#define logsumexp_h

#include <vector>    // For std::vector
#include <algorithm> // For std::max_element
#include <cmath>



//-----------------------------------------------------------------
//         LOGSUMEXP
//-----------------------------------------------------------------
struct LogSumExp {
    long double c;
    long double y;
    std::vector<long double> pi;

    LogSumExp(std::vector<long double> x);

    void calculate_y(std::vector<long double> x);

    void calculate_pi(std::vector<long double> x);
};

// Inline implementations

inline LogSumExp::LogSumExp(std::vector<long double> x) {
    c = *std::max_element(std::begin(x), std::end(x));
    calculate_y(x);
    calculate_pi(x);
}

inline void LogSumExp::calculate_y(std::vector<long double> x) {
    long double sum_to_log = 0;
    for (int i = 0; i < x.size(); i++) {
        long double temp = std::exp(x[i] - c);
        sum_to_log += temp;
    }
    sum_to_log = std::log(sum_to_log);
    y = c + sum_to_log;
}

inline void LogSumExp::calculate_pi(std::vector<long double> x) {
    for (int i = 0; i < x.size(); i++) {
        long double temp = std::exp(x[i] - y);
        pi.push_back(temp);
    }
}

//-----------------------------------------------------------------





#endif
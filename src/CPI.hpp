#ifndef CPI_HEADER_H
#define CPI_HEADER_H

#include <vector>
#include <cmath>
#include "include/LowerTriangle.hpp"
#include "LogSumExp.hpp"

//---------------------------
// Structure for calculating Coalescence Probability Index (CPI) using the softmax probabilities
// given the agent characters. The equation used for the calculation can be modified.
//--------------------------
struct CPI {
    LowerTriangle<long double> lt;       // Lower triangle matrix storing calculated probabilities
    long double normalization_factor;    // Normalization factor for softmax probabilities

    // Constructor: Calculates CPI for the given agent characters using the specified parameter 's'
    CPI(std::vector<std::vector<double>> agent_characters, long double s);

    // Function to calculate the argument for the softmax function
    long double softMaxArg(long double di, long double s);

    // Function to calculate the Manhattan distance between two vectors
    long double manh_distance(const std::vector<double>& a1, const std::vector<double>& a2);

    // Function to set the LowerTriangle matrix based on softmax probabilities
    void set_LT(const LogSumExp& SM);
};

// Inline implementations

// Constructor implementation: Calculates CPI for the given agent characters using the specified parameter 's'
inline CPI::CPI(std::vector<std::vector<double>> agent_characters, long double s) : lt(agent_characters.size()) {
    std::vector<long double> x;

    // Calculate arguments for softmax function based on pairwise Manhattan distances
    for (int i = 0; i < agent_characters.size(); i++) {
        for (int j = 0; j <= i; j++) {
            long double ds = manh_distance(agent_characters[i], agent_characters[j]);
            long double arg = softMaxArg(ds, s);
            x.push_back(arg);
        }
    }

    // Calculate softmax probabilities and normalization factor
    LogSumExp SM = LogSumExp(x);
    normalization_factor = SM.y;

    // Set the LowerTriangle matrix using softmax probabilities
    set_LT(SM);
}

// Function implementation: Calculates the argument for the softmax function
inline long double CPI::softMaxArg(long double di, long double s) {
    return -s * di;
}

// Function implementation: Calculates the Manhattan distance between two vectors
inline long double CPI::manh_distance(const std::vector<double>& a1, const std::vector<double>& a2) {
    if (a1.size() != a2.size()) {
        return 0.0;
    }

    long double result = 0.0;
    for (size_t i = 0; i < a1.size(); i++) {
        result += static_cast<long double>(std::abs(a1[i] - a2[i]));
    }

    result = result / static_cast<long double>(a1.size());

    return result;
}

// Function implementation: Sets the LowerTriangle matrix based on softmax probabilities
inline void CPI::set_LT(const LogSumExp& SM) {
    int index = 0;
    for (auto pi : SM.pi) {
        lt.set(index, pi);
        index++;
    }

    // Set diagonal elements to 0 in the LowerTriangle matrix
    for (int i = 0; i < lt.dim; i++) lt.set(i, i, 0);
}

#endif  // CPI_HEADER_H

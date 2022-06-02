#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#include <cmath>
#include <functional>
#include <fstream>
#include <iterator>

static const size_t NUM_EXPERIMENTS = 1000;

double calc_mean(std::vector<double>& vec) {
    double sum = 0;
    for (auto& x : vec)
        sum += x;
    return sum / vec.size();
}

double calc_deviation(std::vector<double>& vec) {
    double sum = 0;
    for (int i = 0; i < vec.size(); i++)
        sum = sum + (vec[i] - calc_mean(vec)) * (vec[i] - calc_mean(vec));
    return sqrt(sum / (vec.size()));
}

double calc_ttest(std::vector<double>& vec1, std::vector<double>& vec2){
    double mean1 = calc_mean(vec1);
    double mean2 = calc_mean(vec2);
    double sd1 = calc_deviation(vec1);
    double sd2 = calc_deviation(vec2);
    double t_test = (mean1 - mean2) / sqrt((sd1 * sd1) / vec1.size() + (sd2 * sd2) / vec2.size());
    return t_test;
}

namespace with_attributes {
    double calc(double x) noexcept {
        if (x > 2) [[unlikely]]
            return sqrt(x);
        else [[likely]]
            return pow(x, 2);
    }
}  // namespace with_attributes


namespace no_attributes {
    double calc(double x) noexcept {
        if (x > 2)
            return sqrt(x);
        else
            return pow(x, 2);
    }
}  // namespace with_attributes

std::vector<double> benchmark(std::function<double(double)> calc_func) {
    std::vector<double> vec;
    vec.reserve(NUM_EXPERIMENTS);

    std::mt19937 mersenne_engine(12);
    std::uniform_real_distribution<double> dist{ 1, 2.2 };

    for (size_t i = 0; i < NUM_EXPERIMENTS; i++) {

        const auto start = std::chrono::high_resolution_clock::now();
        for (auto size{ 1ULL }; size != 100000ULL; ++size) {
            double x = dist(mersenne_engine);
            calc_func(x);
        }
        const std::chrono::duration<double> diff =
            std::chrono::high_resolution_clock::now() - start;
        vec.push_back(diff.count());
    }
    return vec;
}

int main() {
    
    std::vector<double> vec2 = benchmark(no_attributes::calc);
    std::vector<double> vec1 = benchmark(with_attributes::calc);
    std::cout << "with attribute: " << std::fixed << std::setprecision(6) << calc_mean(vec1) << '\n';
    std::cout << "without attribute: " << std::fixed << std::setprecision(6) << calc_mean(vec2) << '\n';
    std::cout << "T statistics" << std::fixed << std::setprecision(6) << calc_ttest(vec1, vec2) << '\n';
}

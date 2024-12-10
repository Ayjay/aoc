#pragma once

#include <iostream>
#include <chrono>

class SimpleTimer {
public:
    explicit SimpleTimer(const std::string& label = "Timer")
        : start_time(std::chrono::high_resolution_clock::now()), label(label) {}

    ~SimpleTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end_time - start_time).count();
        std::cout << label << " elapsed time: " << elapsed << " seconds\n";
    }

    void reset() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end_time - start_time).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::string label;
};
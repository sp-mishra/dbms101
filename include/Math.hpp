#pragma once

#ifndef MATH_HPP
#define MATH_HPP
#include <random>
#include <limits>

namespace groklab {
    template<typename T = long>
    T randomNumber() {
        static_assert(std::is_arithmetic_v<T>, "Template type T must be an arithmetic type");

        static auto init = []() {
            std::random_device rd;  // Seed
            std::mt19937 gen(rd()); // Mersenne Twister engine
            std::uniform_int_distribution<T> dis(std::numeric_limits<T>::min(), std::numeric_limits<T>::max()); // Define a distribution range
            return std::make_tuple(gen, dis);
        };

        static auto [gen, dis] = init();
        return dis(gen); // Generate and return a random number
    }
}
#endif //MATH_HPP
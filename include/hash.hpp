#pragma once

#ifndef HASH_HPP
#define HASH_HPP
#define XXH_INLINE_ALL
#define XXH_STATIC_LINKING_ONLY
#define XXH_ENABLE_AUTOVECTORIZE
#include <string>
#include <xxhash.h>

namespace groklab {
    template<typename T>
    uint64_t hash(const T& input, const std::size_t numberof_buckets = 100) {
        uint64_t hash_value = {};
        if constexpr (std::is_same_v<T, std::string>) {
            hash_value = XXH3_64bits(input.c_str(), input.size());
        } else if constexpr (std::is_same_v<T, const char *>) {
            hash_value = XXH3_64bits(input, std::strlen(input));
        } else {
            std::ostringstream oss;
            oss << input;
            const std::string str = oss.str();
            hash_value = XXH3_64bits(str.c_str(), str.size());
        }

        const uint64_t result_hash_value = numberof_buckets ? hash_value % numberof_buckets : hash_value;
        return result_hash_value;
    }
}
#endif //HASH_HPP

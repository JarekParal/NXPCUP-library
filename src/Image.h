#pragma once

#include <algorithm>
#include <array>
#include <numeric>

namespace nxpcup {

/**
 * Struct for working with 1D image.
 */
template <typename T, std::size_t N>
struct Image {
    static constexpr std::size_t size = N;

    std::array<T, size> data;

    T& operator[](std::size_t i) { return data[i]; }

    const T& operator[](std::size_t i) const { return data[i]; }

    const T* begin() const { return data.begin(); }

    const T* end() const { return data.end(); }

    T* begin() { return data.begin(); }

    T* end() { return data.end(); }

    /**
     * Differentiate the image.
     *
     * @return copy of the image after calculation difference between neighboring pixels
     */
    Image difference() const
    {
        std::array<T, size> res;
        res[0] = 0;
        for (std::size_t i = 1; i < data.size(); i++) {
            res[i] = abs(data[i] - data[i - 1]);
        }
        return Image{ res };
    }
};

} // namespace nxpcup

#pragma once

#include <math.h>
#include <stdint.h>

namespace nxpcup {

class BorderDetector {
public:
    using ImageType = Camera::ImageType;

    struct Config {
        static constexpr int dataSize = Camera::ImageSize;
        static constexpr int CENTER = dataSize / 2;
        static constexpr int RIGHT_BORDER = dataSize;
        static constexpr int NEIGHBORHOOD = 6;

        static_assert(NEIGHBORHOOD >= 0);
    };

    /**
     * Constructor of class BorderDetector.
     *
     * @param config struct @{Config}
     */
    BorderDetector(Config config)
    {
    }

    /**
     * Initialize the detector and set threshold value.
     * Need to have correct data from a camera with well visible lanes.
     *
     * @param data image from camera
     * @param percentCoefficient how many percent of error return (0 <-> 100)
     */
    void initalize(
        const std::array<ImageType, Config::dataSize>& data,
        const uint8_t percentCoefficient = 100)
    {
        uint16_t maxValue = findMaxValue(data);
        m_threshold = (maxValue * percentCoefficient) / 100;
    }

    /**
     * Find the maximal values from center and set position of this max value
     * as left and right border.
     *
     * @param data image from camera
     */
    int findBorder(const std::array<ImageType, Config::dataSize>& data)
    {
        int16_t previousLeftBorder = m_leftBorder;
        int16_t previousRightBorder = m_rightBorder;
        int16_t middle = (previousRightBorder + previousLeftBorder) / 2;
        m_leftBorder = 0;
        m_rightBorder = Config::RIGHT_BORDER;

        bool findPreviousLeft = isBorderAroundPreviousIndex(data, previousLeftBorder, m_leftBorder);
        bool findPreviousRight = isBorderAroundPreviousIndex(data, previousRightBorder, m_rightBorder);

        if (findPreviousLeft && findPreviousRight) {
            return 1;
        }

        if (!findPreviousLeft) {
            int16_t leftMaxBorderIndex = std::max_element(data.begin(), data.begin() + middle) - data.begin();
            if (data[leftMaxBorderIndex] > m_threshold) {
                m_leftBorder = leftMaxBorderIndex;
            }
        }

        if (!findPreviousRight) {
            int16_t rightMaxBorderIndex = std::max_element(data.begin() + middle, data.end()) - data.begin();
            if (data[rightMaxBorderIndex] > m_threshold) {
                m_rightBorder = rightMaxBorderIndex;
            }
        }

        return 0;
    }

    /**
     * Get distance just with information about left border.
     * Take data from last call @{findBorder}.
     */
    int leftDistanceFromCenter() const
    {
        return m_distanceCenter - m_leftBorder;
    }

    /**
     * Get distance just with information about right border.
     * Take data from last call @{findBorder}.
     */
    int rightDistanceFromCenter() const
    {
        return m_rightBorder - m_distanceCenter;
    }

    /**
     * Calculate the error from the actual border (left and right) as distance
     * from the center.
     *
     * @param percentCoefficient how many percent of error return (0 <-> 100)
     * 		  default value: 100 percent
     * @return if the value is smaller than 0, the line of left side is closer
     * 		   to center then the line on right side
     * 		   if the value is bigger than 0, then the result is opposite
     */
    int error(const int percentCoefficient = 100) const
    {
        int rawError = (rightDistanceFromCenter() - leftDistanceFromCenter()) / 2;

        rawError = (rawError * percentCoefficient) / 100;

        return rawError;
    }

    /**
     * Get the position of left border (0 <-> @{Config::dataSize}).
     */
    int leftBorder() const
    {
        return m_leftBorder;
    }

    /**
     * Get the position of right border (0 <-> @{Config::dataSize}).
     */
    int rightBorder() const
    {
        return m_rightBorder;
    }

private:
    /**
     * Find the maximal value in the input data.
     *
     * @param data image from camera
     * @param offset offset from outside pixel in image from camera
     * @return maximal founded value
     */
    int findMaxValue(
        const std::array<ImageType, Config::dataSize>& data,
        const uint8_t offset = 5) const
    {
        int at = 0;
        for (int i = offset; i < (Config::RIGHT_BORDER - offset); i++) {
            if (data[i] > data[at]) {
                at = i;
            }
        }
        return data[at];
    }


    /**
     * Look for the border around previous border in define neighborhood (@{Config::NEIGHBORHOOD})
     *
     * @param data image from camera
     * @param previousBorder position of border from previous detecting
     * @param border reference for variable which will be used if this function found the border in the neighborhood
     * @return true if found the border in the neighborhood, else false
     */
    bool isBorderAroundPreviousIndex(
        const std::array<ImageType, Config::dataSize>& data,
        const int previousBorder,
        int& border)
    {
        bool find = false;
        uint16_t maxValue = m_threshold;
        int index = nxpcup::clamp<int>(previousBorder - Config::NEIGHBORHOOD, 0, Config::dataSize - 1);
        int indexMax = nxpcup::clamp<int>(previousBorder + Config::NEIGHBORHOOD, 0, Config::dataSize - 1);
        for (; index <= indexMax; ++index) {
            if (data[index] > maxValue) {
                maxValue = data[index];
                border = index;
                find = true;
            }
        }
        return find;
    }

    int m_leftBorder = 0;
    int m_rightBorder = Config::RIGHT_BORDER;
    int m_endflag = 0;

    int m_distanceCenter = Config::CENTER;
    uint16_t m_threshold = 0;
};

} // namespace nxpcup

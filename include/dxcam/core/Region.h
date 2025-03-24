#ifndef DXCAM_CPP_REGION_H
#define DXCAM_CPP_REGION_H

namespace DXCam {

/**
 * @brief Represents a rectangular region.
 */
class Region {
   public:
    /**
     * @brief The coordinate of the left boundary of the region.
     */
    int left;
    /**
     * @brief The coordinate of the top boundary of the region.
     */
    int top;
    /**
     * @brief The coordinate of the right boundary of the region.
     */
    int right;
    /**
     * @brief The coordinate of the bottom boundary of the region.
     */
    int bottom;

    /**
     * @brief Get the width of the region.
     * @return The width of the region.
     */
    [[nodiscard]] int get_width() const;

    /**
     * @brief Get the height of the region.
     * @return The height of the region.
     */
    [[nodiscard]] int get_height() const;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_REGION_H

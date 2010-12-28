/**
 * @file   img-ega-common.hpp
 * @brief  Declarations shared between most EGA image types.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CAMOTO_IMG_EGA_COMMON_HPP_
#define _CAMOTO_IMG_EGA_COMMON_HPP_

#include <boost/iostreams/stream.hpp>
#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

/// Plane order description.
/**
 * This structure is used to describe the order of the planes in the image file
 * being converted.
 *
 * Each plane (red, green, etc.) is set to a number representing the order of
 * the data in the underlying file.  Setting red to 1 means the first plane of
 * image data in the file controls the red pixels.  A value of zero means the
 * plane is not present in the data, and a negative value means the bits
 * in the plane should be inverted (commonly used for the transparency plane.)
 *
 * @note These constants also indicate the bits that will be set in the output
 *       data, i.e. the LSB (zero'th) bit will be blue - so each byte will be
 *       composed as the eight-bit binary value 00THIRGB.
 */
#define PLANE_BLUE          0
#define PLANE_GREEN         1
#define PLANE_RED           2
#define PLANE_INTENSITY     3
#define PLANE_HITMAP        4
#define PLANE_OPACITY       5  ///< 1 == transparent, 0 == opaque
#define PLANE_MAX           6
typedef int PLANE_LAYOUT[PLANE_MAX];

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_COMMON_HPP_

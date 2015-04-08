/**
 * @file  img-ega.hpp
 * @brief Partial Image implementation common to all EGA formats.
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_IMG_EGA_HPP_
#define _CAMOTO_IMG_EGA_HPP_

#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// Plane order description.
/**
 * This structure is used to describe the order of the planes in the image file
 * being converted.
 *
 * Each plane is set to a number representing the content in that plane.
 * Setting EGAPlaneLayout[0] to PlanePurpose::Blue1 means that the first plane
 * will be treated as blue pixels for those bits that are set to 1.  (Blue0 is
 * the opposite, and will set blue pixels where the bits in the plane are 0).
 * Usually the bits are 1 (so Red1, Green1, etc.) will be used.
 */
enum class PlanePurpose {
	Unused,     ///< This plane is not present or is unused
	Blue0,      ///< Bits: 0=blue on, 1=blue off
	Blue1,      ///< Bits: 1=blue on, 0=blue off
	Green0,     ///< Bits: 0=green on, 1=green off
	Green1,     ///< Bits: 1=green on, 0=green off
	Red0,       ///< Bits: 0=red on, 1=red off
	Red1,       ///< Bits: 1=red on, 0=red off
	Intensity0, ///< Bits: 0=bright, 1=dark
	Intensity1, ///< Bits: 1=bright, 0=dark
	Hit0,       ///< Bits: 0=hit, 1=miss
	Hit1,       ///< Bits: 1=hit, 0=miss
	Opaque0,    ///< Bits: 0=opaque, 1=transparent
	Opaque1,    ///< Bits: 1=opaque, 0=transparent
};

typedef std::array<PlanePurpose, 6> EGAPlaneLayout;

enum class PlaneCount
{
	Solid = 4,  ///< Number of planes in each tile (nonmasked) image
	Masked = 5  ///< Number of planes in each sprite (masked) image
};

/// EGA byte-planar Image implementation.
/**
 * This class adds support for converting to and from EGA planar format.  This
 * is where an entire plane of data is kept together, and each plane follows
 * the previous one (i.e. the planes aren't interlaced in any way.)
 *
 * Up to six image planes are supported - the usual RGBI planes, as well as
 * transparency and hitmapping.
 *
 */
class Image_EGA: public Image
{
	public:
		Image_EGA(std::unique_ptr<stream::inout> content, Point dimensions,
			EGAPlaneLayout planes, std::shared_ptr<const Palette> pal);
		virtual ~Image_EGA();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;

	protected:
		/// Populate this->pixels and this->mask
		virtual void doConversion() = 0;

		std::unique_ptr<stream::inout> content;
		stream::pos offset;
		Point dims;
		EGAPlaneLayout planes;
		std::shared_ptr<const Palette> pal;

		Pixels pixels;
		Pixels mask;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_PLANAR_HPP_

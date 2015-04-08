/**
 * @file  img-ega-planar.hpp
 * @brief Image implementation adding support for the EGA planar format.
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

#ifndef _CAMOTO_IMG_EGA_PLANAR_HPP_
#define _CAMOTO_IMG_EGA_PLANAR_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-ega.hpp"

namespace camoto {
namespace gamegraphics {

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
class Image_EGA_Planar: public Image_EGA
{
	public:
		Image_EGA_Planar(std::unique_ptr<stream::inout> content,
			stream::pos offset, Point dimensions, EGAPlaneLayout planes,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_EGA_Planar();

		virtual void convert(const Pixels& newContent, const Pixels& newMask);

	protected:
		/// Populate this->pixels and this->mask
		virtual void doConversion();

		stream::pos offset;   ///< Offset of image data from start of stream
};

/// Filetype handler for full screen raw EGA images.
class ImageType_EGA_RawPlanarBGRI: virtual public ImageType
{
	public:
		ImageType_EGA_RawPlanarBGRI();
		virtual ~ImageType_EGA_RawPlanarBGRI();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::unique_ptr<Image> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::unique_ptr<Image> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage)
			const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_PLANAR_HPP_

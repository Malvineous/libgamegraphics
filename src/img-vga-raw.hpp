/**
 * @file  img-vga-raw.hpp
 * @brief Image_VGA specialisation for fixed-size headerless images.
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

#ifndef _CAMOTO_IMG_VGA_RAW_HPP_
#define _CAMOTO_IMG_VGA_RAW_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-vga.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for full screen raw VGA images.
class ImageType_VGARawBase: virtual public ImageType
{
	public:
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::unique_ptr<Image> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::unique_ptr<Image> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(stream::input& content,
			const std::string& filename) const;

	protected:
		unsigned int depth; // palette depth (6 or 8)
};

class ImageType_VGA6Raw: virtual public ImageType_VGARawBase
{
	public:
		ImageType_VGA6Raw();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
};

class ImageType_VGA8Raw: virtual public ImageType_VGARawBase
{
	public:
		ImageType_VGA8Raw();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
};

/// Raw VGA Image implementation.
class Image_VGARaw: virtual public Image_VGA
{
	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param content
		 *   VGA data
		 *
		 * @param width
		 *   Image width
		 *
		 * @param height
		 *   Image height
		 *
		 * @param pal
		 *   Image palette
		 */
		Image_VGARaw(std::unique_ptr<stream::inout> content, Point dims,
			std::shared_ptr<const Palette> pal);
		virtual ~Image_VGARaw();

		virtual Caps caps() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);

	protected:
		Point dims;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_VGA_RAW_HPP_

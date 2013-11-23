/**
 * @file   img-vga-raw-planar.hpp
 * @brief  VGAImage specialisation for planar fixed-size headerless images.
 *
 * Copyright (C) 2010-2013 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_IMG_VGA_RAW_PLANAR_HPP_
#define _CAMOTO_IMG_VGA_RAW_PLANAR_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-vga-planar.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for full screen raw VGA images.
class VGARawPlanarBaseImageType: virtual public ImageType
{
	public:
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsImage) const;
		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const;
		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const;

	protected:
		unsigned int depth; // palette depth (6 or 8)
};

class VGA6RawPlanarImageType: virtual public VGARawPlanarBaseImageType
{
	public:
		VGA6RawPlanarImageType();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
};

class VGA8RawPlanarImageType: virtual public VGARawPlanarBaseImageType
{
	public:
		VGA8RawPlanarImageType();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
};

/// Raw planar VGA Image implementation.
class VGARawPlanarImage: virtual public VGAPlanarImage
{
	protected:
		int width, height;
		PaletteTablePtr pal;

	public:
		/// Constructor
		/**
		 * No truncate function is required as the image dimensions are fixed, so
		 * the file size will always remain constant.
		 *
		 * @param data
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
		VGARawPlanarImage(stream::inout_sptr data, int width, int height, PaletteTablePtr pal);
		virtual ~VGARawPlanarImage();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual void setDimensions(unsigned int width, unsigned int height);
		virtual PaletteTablePtr getPalette();
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_VGA_RAW_PLANAR_HPP_

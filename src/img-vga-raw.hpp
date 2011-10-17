/**
 * @file   img-vga-raw.hpp
 * @brief  VGAImage specialisation for fixed-size headerless images.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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
class VGARawImageType: virtual public ImageType {

	public:

		VGARawImageType()
			throw ();

		virtual ~VGARawImageType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::inout_sptr fsImage) const
			throw (stream::error);

		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const
			throw (stream::error);

		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const
			throw (stream::error);

		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const
			throw ();

};

/// Raw VGA Image implementation.
class VGARawImage: virtual public VGAImage {

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
		VGARawImage(stream::inout_sptr data, int width, int height, PaletteTablePtr pal)
			throw ();

		virtual ~VGARawImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (stream::error);

		virtual PaletteTablePtr getPalette()
			throw (stream::error);

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_VGA_RAW_HPP_

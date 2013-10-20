/**
 * @file   img-gra-vinyl.hpp
 * @brief  Vinyl Goddess From Mars SCR image format.
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

#ifndef _CAMOTO_IMG_SCR_VINYL_HPP_
#define _CAMOTO_IMG_SCR_VINYL_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Vinyl SCR images.
class VinylSCRImageType: virtual public ImageType
{
	public:
		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsImage) const;
		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const;
		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const;
};

/// Vinyl SCR Image implementation.
class VinylSCRImage: virtual public BaseImage
{
	public:
		/// Constructor
		/**
		 * @param data
		 *   Image data
		 *
		 * @param pal
		 *   Image palette
		 */
		VinylSCRImage(stream::inout_sptr data, PaletteTablePtr pal);
		virtual ~VinylSCRImage();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);
		virtual PaletteTablePtr getPalette();

	protected:
		stream::inout_sptr data;
		PaletteTablePtr pal;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_SCR_VINYL_HPP_

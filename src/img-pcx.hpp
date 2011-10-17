/**
 * @file   img-pcx.hpp
 * @brief  Image specialisation for PCX format images.
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

#ifndef _CAMOTO_IMG_PCX_HPP_
#define _CAMOTO_IMG_PCX_HPP_

#include <Magick++.h>
#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// Filetype handler for standard .PCX images.
class PCXImageType: virtual public ImageType {

	public:

		PCXImageType()
			throw ();

		virtual ~PCXImageType()
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

/// Standard PCX Image implementation.
class PCXImage: virtual public Image {

	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   VGA image data.
		 *
		 * @param fnTruncate
		 *   Callback function when stream size must be changed.
		 */
		PCXImage(stream::inout_sptr data)
			throw (stream::error);

		virtual ~PCXImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (stream::error);

		virtual StdImageDataPtr toStandard()
			throw (stream::error);

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw (stream::error);

		virtual PaletteTablePtr getPalette()
			throw (stream::error);

		void setPalette(PaletteTablePtr newPalette)
			throw (stream::error);

	protected:
		stream::inout_sptr data;
		PaletteTablePtr pal;
		uint8_t ver;
		uint8_t encoding;
		uint8_t bitsPerPlane;
		uint8_t numPlanes;
		int width;
		int height;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_PCX_HPP_

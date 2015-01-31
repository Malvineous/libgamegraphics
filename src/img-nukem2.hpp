/**
 * @file  img-nukem2.hpp
 * @brief Image implementation for Duke Nukem II full screen images.
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

#ifndef _CAMOTO_IMG_NUKEM2_HPP_
#define _CAMOTO_IMG_NUKEM2_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Duke Nukem II full-screen images.
class ImageType_Nukem2: virtual public ImageType
{
	public:
		ImageType_Nukem2();

		virtual ~ImageType_Nukem2();

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

/// Duke Nukem II full-screen Image implementation.
class Image_Nukem2: virtual public Image_EGAPlanar {

	public:
		/// Constructor
		Image_Nukem2(stream::inout_sptr data);

		/// Destructor
		virtual ~Image_Nukem2();

		virtual int getCaps();

		virtual void getDimensions(unsigned int *width, unsigned int *height);

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

		virtual PaletteTablePtr getPalette();

		virtual void setPalette(PaletteTablePtr newPalette);

	protected:
		stream::inout_sptr data;   ///< Underlying file
		PaletteTablePtr vgaPal;    ///< Palette

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_NUKEM2_HPP_

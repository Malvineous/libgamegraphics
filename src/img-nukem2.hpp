/**
 * @file   img-nukem2.hpp
 * @brief  Image implementation for Duke Nukem II full screen images.
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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
class Nukem2ImageType: virtual public ImageType
{
	public:
		Nukem2ImageType()
			throw ();

		virtual ~Nukem2ImageType()
			throw ();

		virtual std::string getCode() const
			throw ();

		virtual std::string getFriendlyName() const
			throw ();

		virtual std::vector<std::string> getFileExtensions() const
			throw ();

		virtual std::vector<std::string> getGameList() const
			throw ();

		virtual Certainty isInstance(stream::input_sptr fsImage) const
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

/// Duke Nukem II full-screen Image implementation.
class Nukem2Image: virtual public EGAPlanarImage {

	public:
		/// Constructor
		Nukem2Image(stream::inout_sptr data)
			throw ();

		/// Destructor
		virtual ~Nukem2Image()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw ();

		virtual PaletteTablePtr getPalette()
			throw (stream::error);

		virtual void setPalette(PaletteTablePtr newPalette)
			throw (stream::error);

	protected:
		stream::inout_sptr data;   ///< Underlying file
		PaletteTablePtr vgaPal;    ///< Palette

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_NUKEM2_HPP_

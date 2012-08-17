/**
 * @file   img-ega-planar.hpp
 * @brief  Image implementation adding support for the EGA byte-planar format.
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

#ifndef _CAMOTO_IMG_EGA_PLANAR_HPP_
#define _CAMOTO_IMG_EGA_PLANAR_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-ega-common.hpp"

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
class EGAPlanarImage: virtual public Image {
	protected:
		stream::inout_sptr data;
		stream::pos offset;
		int width, height;
		PLANE_LAYOUT planes;

	public:
		EGAPlanarImage();

		virtual ~EGAPlanarImage();

		/// These could be set in the constructor, but often descendent classes
		/// won't have these values until the end of their constructors.
		virtual void setParams(stream::inout_sptr data,
			stream::pos offset, int width, int height,
			const PLANE_LAYOUT& planes);

		virtual int getCaps();

		virtual void getDimensions(unsigned int *width, unsigned int *height);

		virtual void setDimensions(unsigned int width, unsigned int height);

		virtual StdImageDataPtr toStandard();

		virtual StdImageDataPtr toStandardMask();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		StdImageDataPtr doConversion(bool mask);

};

/// Filetype handler for full screen raw EGA images.
class EGARawPlanarImageType: virtual public ImageType {

	public:

		EGARawPlanarImageType();

		virtual ~EGARawPlanarImageType();

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

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_PLANAR_HPP_

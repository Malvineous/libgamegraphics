/**
 * @file  img-ccomic.hpp
 * @brief Image specialisation for Captain Comic full-screen images.
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

#ifndef _CAMOTO_IMG_CCOMIC_HPP_
#define _CAMOTO_IMG_CCOMIC_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Captain Comic full-screen images.
class CComicImageType: virtual public ImageType
{
	public:
		CComicImageType()
			throw ();

		virtual ~CComicImageType()
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

/// Image implementation for Captain Comic full-screen images.
class CComicImage: virtual public EGAPlanarImage
{
	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param data
		 *   Raw image data in Captain Comic format.
		 *
		 * @throw stream::error
		 *   Read error or invalid file format.
		 */
		CComicImage(stream::inout_sptr data)
			throw (stream::error);

		virtual ~CComicImage()
			throw ();

	protected:
		stream::inout_sptr data;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_CCOMIC_HPP_

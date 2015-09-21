/**
 * @file  img-pcx.hpp
 * @brief Image specialisation for PCX format images.
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

#ifndef _CAMOTO_IMG_PCX_HPP_
#define _CAMOTO_IMG_PCX_HPP_

#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// Base filetype handler for standard .PCX images.  Use one of the
/// specialisations instead of this.
class ImageType_PCXBase: virtual public ImageType
{
	public:
		/// Base PCX file handler.
		/**
		 * @param bitsPerPlane
		 *   Number of bits per pixel in each plane.  This must match the file being
		 *   opened or a stream::error will be thrown.
		 *
		 * @param numPlanes
		 *   Number of colour planes.  This must match the file being opened or a
		 *   stream::error will be thrown.
		 */
		ImageType_PCXBase(int bitsPerPlane, int numPlanes);
		virtual ~ImageType_PCXBase();

		virtual std::string code() const;
		//virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		//virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::unique_ptr<Image> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::unique_ptr<Image> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage)
			const;

	protected:
		uint8_t bitsPerPlane; ///< Number of bits per pixel in each plane
		uint8_t numPlanes;    ///< Number of planes in this file
};

/// Filetype handler for planar EGA (1b4p) .PCX images.
class ImageType_PCX_PlanarEGA: virtual public ImageType_PCXBase
{
	public:
		ImageType_PCX_PlanarEGA();
		virtual ~ImageType_PCX_PlanarEGA();

		virtual std::string friendlyName() const;
		virtual std::vector<std::string> games() const;
};

/// Filetype handler for linear VGA (8b1p) .PCX images.
class ImageType_PCX_LinearVGA: virtual public ImageType_PCXBase
{
	public:
		ImageType_PCX_LinearVGA();
		virtual ~ImageType_PCX_LinearVGA();

		virtual std::string friendlyName() const;
		virtual std::vector<std::string> games() const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_PCX_HPP_

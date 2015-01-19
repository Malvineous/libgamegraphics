/**
 * @file   img-ega-backdrop.hpp
 * @brief  Image implementation of EGA "backdrop" style tiled images.
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

#ifndef _CAMOTO_IMG_EGA_BACKDROP_HPP_
#define _CAMOTO_IMG_EGA_BACKDROP_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-ega-planar.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Cosmo "backdrop" tiled images.
class BackdropImageType: virtual public ImageType
{
	public:
		BackdropImageType(unsigned int width, unsigned int height,
			unsigned int tileWidth, unsigned int tileHeight, unsigned int planeCount);

		virtual Certainty isInstance(stream::input_sptr fsImage) const;
		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const;
		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage)
			const;

	private:
		unsigned int width;
		unsigned int height;
		unsigned int tileWidth;
		unsigned int tileHeight;
		unsigned int planeCount;
};

class CosmoBackdropImageType: virtual public BackdropImageType
{
	public:
		CosmoBackdropImageType();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
};

class Nukem2BackdropImageType: virtual public BackdropImageType
{
	public:
		Nukem2BackdropImageType();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_EGA_BACKDROP_HPP_

/**
 * @file  img-scr-vinyl.hpp
 * @brief Vinyl Goddess From Mars SCR image format.
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

#ifndef _CAMOTO_IMG_SCR_VINYL_HPP_
#define _CAMOTO_IMG_SCR_VINYL_HPP_

#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Vinyl SCR images.
class ImageType_VinylSCR: virtual public ImageType
{
	public:
		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> fileExtensions() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
		virtual std::unique_ptr<Image> create(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual std::unique_ptr<Image> open(
			std::unique_ptr<stream::inout> content, SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage)
			const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_SCR_VINYL_HPP_

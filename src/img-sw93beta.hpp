/**
 * @file  img-sw93beta.cpp
 * @brief ImageType implementations for Shadow Warrior 1993 Beta.
 *
 * Copyright (C) 2010-2017 Adam Nielsen <malvineous@shikadi.net>
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

#ifndef _CAMOTO_IMG_SW93BETA_HPP_
#define _CAMOTO_IMG_SW93BETA_HPP_

#include "img-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

/// Shadow Warrior 1993 Beta background image.
class ImageType_SW93Beta_BG: virtual public ImageType_VGARawBase
{
	public:
		ImageType_SW93Beta_BG();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
};

/// Shadow Warrior 1993 Beta background image.
class ImageType_SW93Beta_Wall: virtual public ImageType_VGARawBase
{
	public:
		ImageType_SW93Beta_Wall();

		virtual std::string code() const;
		virtual std::string friendlyName() const;
		virtual std::vector<std::string> games() const;
		virtual Certainty isInstance(stream::input& content) const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_SW93BETA_HPP_

/**
 * @file  img-bash-sprite.hpp
 * @brief Image implementation for Monster Bash sprite frames.
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

#ifndef _CAMOTO_IMG_BASH_SPRITE_HPP_
#define _CAMOTO_IMG_BASH_SPRITE_HPP_

#include <camoto/gamegraphics/image.hpp>

namespace camoto {
namespace gamegraphics {

/// Image implementation for frames within a Monster Bash sprite.
class Image_BashSprite: virtual public Image
{
	public:
		Image_BashSprite(std::unique_ptr<stream::inout> content);
		virtual ~Image_BashSprite();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual Point dimensions() const;
		virtual void dimensions(const Point& newDimensions);
		virtual Point hotspot() const;
		virtual void hotspot(const Point& newHotspot);
		virtual Point hitrect() const;
		virtual void hitrect(const Point& newHitRect);
		virtual Pixels convert() const;
		virtual Pixels convert_mask() const;
		virtual void convert(const Pixels& newContent,
			const Pixels& newMask);

	protected:
		std::unique_ptr<stream::inout> content;
		uint8_t flags;
		Point dims;
		Point ptHotspot;
		Point ptHitRect;

		/// Convert the image data to a more standard EGA format.
		std::unique_ptr<Image> toEGA() const;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_BASH_SPRITE_HPP_

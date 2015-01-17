/**
 * @file   img-bash-sprite.hpp
 * @brief  Image implementation for Monster Bash sprite frames.
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

#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

/// Image implementation for frames within a Monster Bash sprite.
class BashSpriteImage: virtual public BaseImage
{
	public:
		BashSpriteImage(stream::inout_sptr data);
		virtual ~BashSpriteImage();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual void setDimensions(unsigned int width, unsigned int height);
		virtual void getHotspot(signed int *x, signed int *y);
		virtual void setHotspot(signed int x, signed int y);
		virtual void getHitRect(signed int *x, signed int *y);
		virtual void setHitRect(signed int x, signed int y);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);

	protected:
		stream::inout_sptr data;
		PaletteTablePtr pal;
		uint8_t flags;
		uint8_t width, height;
		int16_t hotspotX, hotspotY;
		uint16_t rectX, rectY;

		/// Convert the image data to a more standard EGA format.
		ImagePtr toEGA();
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_IMG_BASH_SPRITE_HPP_

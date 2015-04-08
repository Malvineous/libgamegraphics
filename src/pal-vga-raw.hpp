/**
 * @file  pal-vga-raw.hpp
 * @brief Palette interface to 768-byte raw 6-bit VGA palette files.
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

#ifndef _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_
#define _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-palette.hpp"

namespace camoto {
namespace gamegraphics {

/// Convert a 6-bit VGA palette value (0-63) to 8-bit (0-255)
constexpr unsigned int pal_6to8(unsigned int six)
{
	return (six << 2) | (six >> 4);
}

/// Convert an 8-bit (0-255) palette value to 6-bit VGA (0-63)
constexpr unsigned int pal_8to6(unsigned int eight)
{
	return eight >> 2;
}

/// Filetype handler for VGA 6-bit palette files.
class ImageType_Palette_VGA: virtual public ImageType
{
	public:
		ImageType_Palette_VGA();
		virtual ~ImageType_Palette_VGA();

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

/// Filetype handler for VGA 8-bit palette files.
class ImageType_VGA8Palette: virtual public ImageType_Palette_VGA
{
	public:
		ImageType_VGA8Palette();
		virtual ~ImageType_VGA8Palette();

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

/// Palette interface to 768-byte raw 6/8-bit VGA palette files.
class Palette_VGA: virtual public Image_Palette
{
	public:
		/// Create a new palette.
		/**
		 * @param data
		 *   Palette content, typically 768 bytes long.
		 *
		 * @param depth
		 *   Either 6 for a 6-bit raw VGA palette (values 0-63) or 8 for an 8-bit
		 *   RGB palette (values 0-255).
		 */
		Palette_VGA(std::unique_ptr<stream::inout> content, unsigned int depth);
		virtual ~Palette_VGA();

		virtual Caps caps() const;
		virtual ColourDepth colourDepth() const;
		virtual std::shared_ptr<const Palette> palette() const;
		virtual void palette(std::shared_ptr<const Palette> newPalette);

	private:
		std::unique_ptr<stream::inout> content;
		unsigned int depth;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PAL_VGA_RAW_HPP_

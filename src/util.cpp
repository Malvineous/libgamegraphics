/**
 * @file  util.cpp
 * @brief Utility functions.
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

#include <camoto/util.hpp> // make_unique
#include <camoto/gamegraphics/image-memory.hpp>
#include <camoto/gamegraphics/util.hpp>

namespace camoto {
namespace gamegraphics {

std::unique_ptr<Image> overlayImage(const Image* base, const Image* overlay)
{
	auto pixBase = base->convert();
	auto maskBase = base->convert_mask();
	auto pixOverlay = overlay->convert();
	auto maskOverlay = overlay->convert_mask();

	auto len = pixBase.size();
	Pixels pixMerged(len);
	Pixels maskMerged(len);

	auto pix_b = pixBase.data();
	auto mask_b = maskBase.data();
	auto pix_o = pixOverlay.data();
	auto mask_o = maskOverlay.data();
	auto pix_m = pixMerged.data();
	auto mask_m = maskMerged.data();

	for (unsigned int i = 0; i < len; i++) {
		if (*mask_o & (int)Image::Mask::Transparent) {
			// Overlay is transparent, use base pixel
			*pix_m = *pix_b;
			*mask_m = *mask_b;
		} else {
			// Overlay is nontransparent, use overlay pixel
			*pix_m = *pix_o;
			*mask_m = *mask_o;
		}
		pix_b++;
		pix_o++;
		pix_m++;
		mask_b++;
		mask_o++;
		mask_m++;
	}
	return std::make_unique<Image_Memory>(
		base->dimensions(),
		pixMerged,
		maskMerged,
		Point{0, 0},
		Point{0, 0},
		nullptr
	);
}

} // namespace gamegraphics
} // namespace camoto

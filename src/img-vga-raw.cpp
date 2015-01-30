/**
 * @file  img-vga-raw.cpp
 * @brief VGAImage specialisation for fixed-size headerless images.
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

#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

std::vector<std::string> VGARawBaseImageType::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> VGARawBaseImageType::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty VGARawBaseImageType::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// TESTED BY: TODO
	if (len == 64000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr VGARawBaseImageType::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) psImage->write(buf, 64);

	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], this->depth));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VGARawImage(psImage, 320, 200, pal));
}

ImagePtr VGARawBaseImageType::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette], this->depth));
		pal = palFile->getPalette();
	}
	return ImagePtr(new VGARawImage(psImage, 320, 200, pal));
}

SuppFilenames VGARawBaseImageType::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal";
	return supps;
}


VGA6RawImageType::VGA6RawImageType()
{
	this->depth = 6;
}

std::string VGA6RawImageType::getCode() const
{
	return "img-vga-raw-fullscreen";
}

std::string VGA6RawImageType::getFriendlyName() const
{
	return "Raw VGA fullscreen image (6-bit palette)";
}


VGA8RawImageType::VGA8RawImageType()
{
	this->depth = 8;
}

std::string VGA8RawImageType::getCode() const
{
	return "img-vga-raw8-fullscreen";
}

std::string VGA8RawImageType::getFriendlyName() const
{
	return "Raw VGA fullscreen image (24-bit palette)";
}


VGARawImage::VGARawImage(stream::inout_sptr data, int width, int height,
	PaletteTablePtr pal)
	:	VGAImage(data, 0),
		width(width),
		height(height),
		pal(pal)
{
}

VGARawImage::~VGARawImage()
{
}

int VGARawImage::getCaps()
{
	return Image::ColourDepthVGA | Image::HasPalette;
}

void VGARawImage::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void VGARawImage::setDimensions(unsigned int width, unsigned int height)
{
	throw stream::error("this image is a fixed size");
}

PaletteTablePtr VGARawImage::getPalette()
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto

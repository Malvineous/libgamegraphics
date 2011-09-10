/**
 * @file   img-magick.cpp
 * @brief  Image specialisation for Magick format images.
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
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

#include "img-magick.hpp"

namespace camoto {
namespace gamegraphics {

MagickImage::MagickImage(TilesetPtr parent, Magick::Image& img, int x, int y,
	int width, int height)
	throw (std::ios::failure) :
		parent(parent),
		img(img),
		x(x),
		y(y),
		width(width),
		height(height)
{
}

MagickImage::~MagickImage()
	throw ()
{
}

int MagickImage::getCaps()
	throw ()
{
	return Image::ColourDepthVGA;
}

void MagickImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void MagickImage::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->img.zoom(Magick::Geometry(width, height));
	return;
}

StdImageDataPtr MagickImage::toStandard()
	throw (std::ios::failure)
{
	try {
		Magick::Pixels pixCache(this->img);
		pixCache.get(this->x, this->y, this->width, this->height);
		Magick::IndexPacket *pixels = pixCache.indexes();

		unsigned long dataSize = width * height;
		uint8_t *imgData = new uint8_t[dataSize];
		StdImageDataPtr ret(imgData);
		while (dataSize-- > 0) *imgData++ = *pixels++;
		return ret;

	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}
}

StdImageDataPtr MagickImage::toStandardMask()
	throw ()
{
	int dataSize = this->width * this->height;

	// Return an entirely opaque mask
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	memset(imgData, 0, dataSize);

	return ret;
}

void MagickImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw (std::ios::failure)
{
	try {
		Magick::Pixels pixCache(this->img);
		pixCache.get(this->x, this->y, this->width, this->height);
		Magick::IndexPacket *pixels = pixCache.indexes();

		unsigned long dataSize = this->width * this->height;
		uint8_t *imgData = newContent.get();
		for (int i = 0; i < dataSize; i++) *pixels++ = *imgData++;

		this->img.syncPixels();

	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}

	return;
}

PaletteTablePtr MagickImage::getPalette()
	throw (std::ios::failure)
{
	int numColours = this->img.colorMapSize();

	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(numColours);

	for (int c = 0; c < numColours; c++) {
		Magick::Color s = this->img.colorMap(c);
		PaletteEntry p;
		p.red = s.redQuantum() * 255 / QuantumRange;
		p.green = s.greenQuantum() * 255 / QuantumRange;
		p.blue = s.blueQuantum() * 255 / QuantumRange;
		p.alpha = 255;
		pal->push_back(p);
	}

	return pal;
}

void MagickImage::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	assert(this->getCaps() & Image::CanSetPalette);

	int numColours = newPalette->size();

	try {
		this->img.colorMapSize(numColours);

		for (int c = 0; c < numColours; c++) {
			Magick::Color s;
			s.redQuantum((*newPalette)[c].red * QuantumRange / 255);
			s.greenQuantum((*newPalette)[c].green * QuantumRange / 255);
			s.blueQuantum((*newPalette)[c].blue * QuantumRange / 255);
			this->img.colorMap(c, s);
		}
	} catch (Magick::Exception& e) {
		throw std::ios::failure(e.what());
	}

	return;
}

} // namespace gamegraphics
} // namespace camoto

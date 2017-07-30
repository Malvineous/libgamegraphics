/**
 * @file  img-sw93beta.cpp
 * @brief ImageType implementations for Shadow Warrior Beta.
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

#include "img-sw93beta.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_SW93Beta_BG::ImageType_SW93Beta_BG()
{
	this->depth = 6;
	this->dims = Point{1284, 25};
}

std::string ImageType_SW93Beta_BG::code() const
{
	return "img-sw93beta-bg";
}

std::string ImageType_SW93Beta_BG::friendlyName() const
{
	return "Shadow Warrior 1993 Beta background image";
}

std::vector<std::string> ImageType_SW93Beta_BG::games() const
{
	return {"Shadow Warrior 1993 Beta"};
}

ImageType::Certainty ImageType_SW93Beta_BG::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: TODO
	if (len == 32100) return Certainty::PossiblyYes;

	// TESTED BY: TODO
	return Certainty::DefinitelyNo;
}


ImageType_SW93Beta_Wall::ImageType_SW93Beta_Wall()
{
	this->depth = 6;
	this->dims = Point{64, 64};
}

std::string ImageType_SW93Beta_Wall::code() const
{
	return "img-sw93beta-wall";
}

std::string ImageType_SW93Beta_Wall::friendlyName() const
{
	return "Shadow Warrior 1993 Beta wall tile";
}

std::vector<std::string> ImageType_SW93Beta_Wall::games() const
{
	return {"Shadow Warrior 1993 Beta"};
}

ImageType::Certainty ImageType_SW93Beta_Wall::isInstance(
	stream::input& content) const
{
	stream::pos len = content.size();

	// TESTED BY: TODO
	if (len == 64*64) return Certainty::Unsure;

	// TESTED BY: TODO
	return Certainty::DefinitelyNo;
}

} // namespace gamegraphics
} // namespace camoto

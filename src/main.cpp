/**
 * @file   main.cpp
 * @brief  Entry point for libgamegraphics.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include <string>
#include <camoto/debug.hpp>
#include <camoto/gamegraphics.hpp>

// Include all the file formats for the Manager to load
#include "img-ccaves.hpp"
//#include "img-xargon.hpp"

namespace camoto {
namespace gamegraphics {

#ifdef DEBUG
static bool registered_exit_func = false;
void exitFunc()
{
	std::cerr << "\e[44m --- Instance counts --- \e[49m\n";
	refcount_dump(GRPType);
	refcount_dump(GRPArchive);
	refcount_dump(FATArchive);
	refcount_dump(FATEntry);
	refcount_dump(SubdirArchive);
	refcount_dump(substream_device);
	refcount_dump(segmented_stream_device);
}
#endif

ManagerPtr getManager()
	throw ()
{
	return ManagerPtr(new Manager());
}

Manager::Manager()
	throw ()
{
#ifdef DEBUG
	if (!registered_exit_func) {
		atexit(exitFunc);
		registered_exit_func = true;
	}
#endif
	this->vcTypes.push_back(GraphicsTypePtr(new CCavesGraphicsType()));
}

Manager::~Manager()
	throw ()
{
}

GraphicsTypePtr Manager::getGraphicsType(int iIndex)
	throw ()
{
	if (iIndex >= this->vcTypes.size()) return GraphicsTypePtr();
	return this->vcTypes[iIndex];
}

GraphicsTypePtr Manager::getGraphicsTypeByCode(const std::string& strCode)
	throw ()
{
	for (VC_GRAPHICSTYPE::const_iterator i = this->vcTypes.begin(); i != this->vcTypes.end(); i++) {
		if ((*i)->getCode().compare(strCode) == 0) return *i;
	}
	return GraphicsTypePtr();
}

} // namespace gamegraphics
} // namespace camoto

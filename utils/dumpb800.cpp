/**
 * @file   dumpb800.cpp
 * @brief  Print a B800 screen dump as CP437 text with ANSI escape codes.
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

#include <iostream>
#include <stdint.h>

using namespace std;

int main(void)
{
	uint8_t t, a;  // text, attribute
	int col = 0;
	int fg = -1, bg = -1, newfg, newbg;
	bool changeAttr = false;
	while (!cin.eof()) {
		cin.read((char *)&t, 1);
		cin.read((char *)&a, 1);
		if (t == 0) t = ' ';
		if (fg != (a & 0x0F)) newfg = (a & 0x0F);
		else newfg = -1;
		if (bg != (a >> 4)) newbg = a >> 4;
		else newbg = -1;

		if ((newfg >= 0) || (newbg >= 0)) {
			cout << "\033[";
			if (newfg >= 0) {
				fg = newfg;
				cout << ((fg & 0x08) ? "1;" : "22;")
					<< 30 + (((fg & 1) << 2) | (fg & 2) | ((fg & 4) >> 2))
				;
			}
			if (newbg >= 0) {
				bg = newbg;
				cout << ((newfg >= 0) ? ";" : "") << ((bg & 0x08) ? "5;" : "25;")
					<< 40 + (((bg & 1) << 2) | (bg & 2) | ((bg & 4) >> 2))
				;
			}
			cout << "m";
			newfg = -1;
			newbg = -1;
		}
		cout << (char)t;
		if (++col == 80) {
			cout << "\033[0m\n";
			col = 0;
			fg = -1;
			bg = -1;
		}
	}
	cout << flush;
	return 0;
}

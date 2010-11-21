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

#include <iostream>
#include <camoto/stream_file.hpp>
#include <camoto/gamegraphics.hpp>

using namespace camoto;
using namespace camoto::gamegraphics;

int main(void)
{
	// Get hold of the Manager class
	ManagerPtr manager = getManager();

	// Use the manager to look up a particular image format, in this case a
	// 256-colour PCX image.  This will fail if the image is not 256 colours,
	// but you can use img-pcx-1b4p for a 16-colour image too.
	ImageTypePtr imageType = manager->getImageTypeByCode("img-pcx-8b1p");

	// Open an image file on disk
	stream::file_sptr file(new stream::file());
	file->open("apogee.pcx");

	// We cheat here - we should check and load any supplementary files, but
	// for the sake of keeping this example simple we know this format doesn't
	// need any supps.
	camoto::SuppData supps;

	// Use the format handler to read in the file we opened.
	ImagePtr img = imageType->open(file, supps);

	// Find out the dimensions of the image.
	unsigned int width, height;
	img->getDimensions(&width, &height);
	std::cout << "This image is " << width << "x" << height << " pixels in size.\n";

	// No cleanup required because all the Ptr variables are shared pointers,
	// which get destroyed automatically when they go out of scope (and nobody
	// else is using them!)

	return 0;
}

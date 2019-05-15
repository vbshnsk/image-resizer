#include "image.h"
#include <conio.h>
#include <iostream>

using namespace std;

int main() {
	openImage toResize("bmp.bmp");
	newImage Resized("test.bmp", toResize, 10);
	Resized.createFile();
	_getch();
}
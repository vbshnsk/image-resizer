#include "image.h"
#include <conio.h>
#include <iostream>

using namespace std;

int main() {
	openImage toResize("1111.bmp");
	newImage Resized("test.bmp", toResize, 1.5);
	Resized.createFile();
	_getch();
}
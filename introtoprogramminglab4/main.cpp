#include "image.h"
#include <conio.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	openImage toResize("Untitled.bmp");
	newImage Resized("newtest.bmp", toResize, 20);
	Resized.createFile();
}
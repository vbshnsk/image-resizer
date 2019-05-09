#pragma once
#pragma pack(1)

#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <iostream>
#include <fstream>

typedef struct {
	int8_t id[2];
	int32_t filesize;
	int16_t reserved[2];
	int32_t headersize;
	int32_t infoSize;
	int32_t width;
	int32_t depth;
	int16_t biPlanes;
	int16_t bits;
	int32_t biCompression;
	int32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	int32_t biClrUsed;
	int32_t biClrImportant;
} BMPHEAD;

typedef struct {
	int8_t redComponent;
	int8_t greenComponent;
	int8_t blueComponent;
} PIXELDATA;

class Image {
protected:
	BMPHEAD header;
	PIXELDATA** pixels;
public:
	const char* name;
	Image() {};
	Image(const char* name) {
		this->name = name;
	}

};


class openImage : private Image {
private:
	void setHeader() {
		/*                                                   same thing but using cstdio
		FILE* file;
		file = fopen(name, "rb");
		fread(&this->header, sizeof(this->header), 1, file);
		*/
		std::ifstream file;
		file.open(name, std::ios::binary);
		file.read(reinterpret_cast<char*>(&this->header), sizeof(this->header));
		file.close();
	}
	void setPixels() {
		pixels = new PIXELDATA * [header.depth];
		for (int i = 0; i < header.depth; i++) {
			pixels[i] = new PIXELDATA[header.width];
		}

		size_t padding;
		if (3 * header.width % 4)
			padding = 4 - 3 * header.width % 4;
		else
			padding = 0;

		std::ifstream file;
		file.open(name, std::ios::binary);
		file.seekg(sizeof(this->header), std::ios::beg);
		for (int i = 0; i < header.depth; i++) {
			for (int k = 0; k < header.width; k++) {
				file.read(reinterpret_cast<char*>(&this->pixels[i][k]), sizeof(this->pixels[i][k]));
			}
			file.seekg(padding, std::ios::cur);
		}
		file.close();
	}
public:
	openImage(const char* name) {
		this->name = name;
		this->setHeader();
		this->setPixels();
	};
};


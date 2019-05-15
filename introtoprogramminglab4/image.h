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
	PIXELDATA** pixels;
	BMPHEAD header;
public:
	const char* name;
	Image() {};
	~Image() {
		for (int i = 0; i < this->header.depth; i++)
			delete[] pixels[i];
		delete pixels;
	}
	void createFile() {
		std::ofstream file;
		file.open(name, std::ios::binary);
		file.write(reinterpret_cast<char*>(&this->header), sizeof(this->header));
		
		char null;
		int padding;
		if (3 * this->header.width % 4)
			padding = 4 - 3 * this->header.width % 4;
		else
			padding = 0;

		for (int i = 0; i < this->header.depth; i++) {
			for (int k = 0; k < this->header.width; k++) {
				file.write(reinterpret_cast<char*>(&this->pixels[i][k]), sizeof(pixels[i][k]));
			}
			file.write(&null, padding);
		}
		file.close();
	}
	BMPHEAD getHeader() {
		return this->header;
	}
	PIXELDATA** getPixels() {
		return this->pixels;
	}
};


class openImage : public Image {
private:
	void setHeader() {
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
		if (3 * this->header.width % 4)
			padding = 4 - 3 * this->header.width % 4;
		else
			padding = 0;

		std::ifstream file;
		file.open(name, std::ios::binary);
		file.seekg(sizeof(this->header), std::ios::beg);
		for (int i = 0; i < this->header.depth; i++) {
			for (int k = 0; k < this->header.width; k++) {
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

class newImage : public Image {
private:
	PIXELDATA** interpolate() {
	}
	void setHeader(Image& source, int pow) {
		this->header = source.getHeader();
		this->header.width *= pow;
		this->header.depth *= pow;

		size_t padding;
		if (3 * this->header.width % 4)
			padding = 4 - 3 *this->header.width % 4;
		else
			padding = 0;

		this->header.filesize = this->header.headersize + (this->header.width * 3 + padding) * this->header.depth;
	}
	void setPixels(Image & source, int pow) {
		PIXELDATA** temp = new PIXELDATA * [this->header.depth];
		for (int i = 0; i < this->header.depth; i++) {
			temp[i] = new PIXELDATA[this->header.width];
		}
		PIXELDATA** sourceP = source.getPixels();
		for (int i = 0; i < this->header.depth; i++) {
			for (int k = 0; k < this->header.width; k++) {
				temp[i][k] = sourceP[i / pow][k / pow];
			}
		}
		this->pixels = temp;
	}
public:
	newImage(const char* name, Image & source, int pow) {
		this->name = name;
		this->setHeader(source, pow);
		this->setPixels(source, pow);
	}
};


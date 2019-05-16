#pragma once
#pragma pack(2)

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
	Image(char* name) {
		this->name = name;
		this->setHeader();
	};
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

	unsigned int getRGBValue(int8_t value) {
		return (value + 256) % 256;
	}


	void interpolate(Image& source) {
		float xdif = ((float)(source.getHeader().width - 1) / (header.width - 1));
		float ydif = ((float)(source.getHeader().depth - 1) / (header.depth - 1));

		PIXELDATA * *temp = source.getPixels();
		PIXELDATA pixel;
		float x, y;
		int p1x, p1y, p2x, p2y;
		int inter1, inter2;
		for (int i = 0; i < header.depth; i++)
			for (int j = 0; j < header.width; j++) {


				x = j * xdif;
				y = i * ydif;


				p2x = (int)x;
				p2y = (int)y;
				p1x = (int)x + 1;
				p1y = (int)y + 1;

				float x1 = (p1x - x) / (p1x - p2x),
					x2 = (x - p2x) / (p1x - p2x),
					y1 = (p1y - y) / (p1y - p2y),
					y2 = (y - p2y) / (p1y - p2y);


				if ((int)x + 1 < source.getHeader().width)	p1x = (int)x + 1;
				else p1x = (int)x;
				if ((int)y + 1 < source.getHeader().depth)	p1y = (int)y + 1;
				else p1y = (int)y;

				inter1 = x1 * getRGBValue(temp[p2y][p2x].blueComponent) + x2 * getRGBValue(temp[p2y][p1x].blueComponent);
				inter2 = x1 * getRGBValue(temp[p1y][p2x].blueComponent) + x2 * getRGBValue(temp[p1y][p1x].blueComponent);
				pixel.blueComponent = y1 * inter1 + y2 * inter2;

				inter1 = x1 * getRGBValue(temp[p2y][p2x].redComponent) + x2 * getRGBValue(temp[p2y][p1x].redComponent);
				inter2 = x1 * getRGBValue(temp[p1y][p2x].redComponent) + x2 * getRGBValue(temp[p1y][p1x].redComponent);
				pixel.redComponent = y1 * inter1 + y2 * inter2;

				inter1 = x1 * getRGBValue(temp[p2y][p2x].greenComponent) + x2 * getRGBValue(temp[p2y][p1x].greenComponent);
				inter2 = x1 * getRGBValue(temp[p1y][p2x].greenComponent) + x2 * getRGBValue(temp[p1y][p1x].greenComponent);
				pixel.greenComponent = y1 * inter1 + y2 * inter2;

				pixels[i][j] = pixel;
			}

	}



	void setHeader(Image & source, double pow) {
		this->header = source.getHeader();
		this->header.width = (int)(this->header.width * pow);
		this->header.depth = (int)(this->header.depth * pow);

		size_t padding;
		if (3 * this->header.width % 4)
			padding = 4 - 3 * this->header.width % 4;
		else
			padding = 0;

		this->header.filesize = this->header.headersize + (this->header.width * 3 + padding) * this->header.depth;
	}

	void setPixels(Image & source) {
		PIXELDATA** temp = new PIXELDATA * [this->header.depth];
		for (int i = 0; i < this->header.depth; i++) {
			temp[i] = new PIXELDATA[this->header.width];
		}
		pixels = temp;
		interpolate(source);
	}
public:
	newImage(const char* name, Image & source, double pow) {
		this->name = name;
		this->setHeader(source, pow);
		this->setPixels(source);
	}
};
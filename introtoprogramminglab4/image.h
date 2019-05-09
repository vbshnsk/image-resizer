#pragma once
#pragma pack(1)
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>

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

class image {
private:
	BMPHEAD header;
	PIXELDATA** pixels;
	const char* name;
public:
	image(const char* name) {
		this->name = name;
	};
	void setHeader() {
		std::fstream filepp;
		filepp.open(name, std::ios::in | std::ios::binary);
		char* buff = new char[54]; // all of the header info, needs to be transfered to BMPHEAD
		filepp.read(buff, sizeof(this->header));
	}

	
	~image() {};
};


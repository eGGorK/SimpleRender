#include <iostream>
#include <string.h>
#include "tgaimage.h"
#include <algorithm>

TGAColor::TGAColor(): val(0), bytespp(1){
}
TGAColor::TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A): r(R), g(G), b(B), a(A), bytespp(4) {
}
TGAColor::TGAColor(int v, int bpp): val(v), bytespp(bpp) {
}
TGAColor::TGAColor(const unsigned char* praw, int bpp):bytespp(bpp) {
    for (int i = 0; i < bytespp; i++) {
        raw[i] = praw[i];
    }
}
TGAColor::TGAColor(const TGAColor& other): val(other.val), bytespp(other.bytespp) {
}

TGAColor& TGAColor::operator=(const TGAColor& other) {
    if (this != &other) {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        val = other.val;
        bytespp = other.bytespp;
    }
    return *this;
}

TGAColor TGAColor::operator+ (const TGAColor& other) {
        TGAColor result;
        result.r = std::min(255, r + other.r);
        result.g = std::min(255, g + other.g);
        result.b = std::min(255, b + other.b);
        result.a = std::min(255, a + other.a);
        return result;
    }

TGAColor TGAColor::operator- (const TGAColor& other) {
    TGAColor result;
    result.r = r - other.r;
    result.g = g - other.g;
    result.b = b - other.b;
    return result;
}

TGAColor TGAColor::operator* (const float scalar) {
    TGAColor result;
    result.bytespp = bytespp;
    result.r = static_cast<unsigned char>(std::clamp(static_cast<int>(r * scalar), 0, 255));
    result.g = static_cast<unsigned char>(std::clamp(static_cast<int>(g * scalar), 0, 255));
    result.b = static_cast<unsigned char>(std::clamp(static_cast<int>(b * scalar), 0, 255));
    result.a = static_cast<unsigned char>(std::clamp(static_cast<int>(a * scalar), 0, 255));

    return result;
}

//End TGAColorRealization

TGAImage::TGAImage(): data(nullptr), width(0), height(0), bytespp(1) {
}
TGAImage::TGAImage(int w, int h, int bpp): width(w), height(h),bytespp(bpp) {
    data = new unsigned char[width*height*bytespp];
    memset(data, 0, width*height*bytespp);
}
TGAImage::TGAImage(const TGAImage& other): width(other.width), height(other.height), bytespp(other.bytespp) {
    if (data) delete [] data;
    data = new unsigned char[width*height*bytespp];
    memcpy(data, other.data, width*height*bytespp);
}
TGAImage& TGAImage::operator=(const TGAImage& other) {
    if (data) delete [] data;
    width = other.width;
    height = other.height;
    bytespp = other.bytespp;
    data = new unsigned char[width*height*bytespp];
    memcpy(data, other.data, width*height*bytespp);
    return *this;
}
TGAImage::~TGAImage() {
    if (data) delete [] data;
}
TGAColor TGAImage::get(int x, int y) {
    if (x < 0 || y  < 0 || !data || x> width || y >height){
        std::cerr<<"get error"<<std::endl;
        return TGAColor();
    }
    return TGAColor(data + (x+y*width)*bytespp,bytespp);
}

bool TGAImage::set(int x, int y, TGAColor color) {
    if (x < 0 || y  < 0 || x > width || y > height){
        std::cout << "x:" << x << " y: " << y << std::endl;
        std::cerr<<"set error"<<std::endl;
        return false;
    }
    memcpy(data + (x+y*width)*bytespp, color.raw, bytespp);
    return true;
}

bool TGAImage::read_TGA_file(const char* filename) {
    if (!data) delete[] data;
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Can't open file: "<< filename << std::endl;
        in.close();
        return false;
    }
    TGA_Header header;
    in.read((char*)&header, sizeof(header));
    if (!in.good()){
        std::cerr << "Can't read header" << std::endl;
        in.close();
        return false;
    }
    width = header.width;
    height = header.height;
    bytespp = header.bitsperpixel >> 3;
    if (width <0 || height < 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
        std::cerr << "Incorrect data" << std::endl;
        in.close();
        return false;
    }
    data = new unsigned char[width*height*bytespp];
    if (header.datatypecode == 3 || header.datatypecode == 2) {
        in.read((char*)data, width*height*bytespp);
        if (!in.good()){
            std::cerr << "Can't read data" << std::endl;
            in.close();
            return false;
        }
    } else if (header.datatypecode == 10 || header.datatypecode == 11) {
        if (!load_RLE_data(in)){
            std::cerr << "Can't load RLE data" << std::endl;
            in.close();
            return false;
        }
    } else {
        std::cerr << "Unknown format" << std::endl;
        in.close();
        return false;
    }
    if (! (header.imagedescriptor & 0x20)) {
        flip_horizontally();
    }
    if (!(header.imagedescriptor & 0x10)) {
        flip_vertically();
    }
    in.close();
    return true;
}
bool TGAImage::load_RLE_data(std::ifstream& in) {
    unsigned long pixels = width*height;
    unsigned long currentpixel = 0;
    unsigned long currentbyte = 0;
    TGAColor currentcolor;
    do {
        unsigned char chunk = in.get();
        if (!in.good()) {
            std::cerr << "load_RLE_data error" << std::endl;
            in.close();
            return false;
        }
        if (chunk < 128) {
            chunk++;
            for (int i =0; i < chunk; i++){
                in.read((char*)currentcolor.raw, bytespp);
                if (!in.good()) {
                    std::cerr << "load_RLE_data error" << std::endl;
                    in.close();
                    return false;
                }
                for (int i = 0; i < bytespp; i++) {
                    data[currentbyte++] = currentcolor.raw[i];
                }
                currentpixel++;
            }
        } else {
            chunk -= 127;
            in.read((char*)currentcolor.raw, bytespp);
            for (int i = 0; i < chunk; i++) {
                for (int j = 0; j < bytespp; j++)
                    data[currentbyte++] = currentcolor.raw[i];
                currentpixel++;
                if (currentpixel > pixels) {
                    std::cerr << "Too many pixels" << std::endl;
                    return false;
                }
            }
        }
    } while (currentpixel < pixels);
    return true;
}

bool TGAImage::flip_vertically() {
    if (!data) return false;

    unsigned long bytes_per_line = width * bytespp;
    unsigned char* line = new unsigned char[bytes_per_line];
    int half = height >> 1;

    for (int j = 0; j < half; ++j) {
        unsigned long l1 = j * bytes_per_line;
        unsigned long l2 = (height - 1 - j) * bytes_per_line;

        // Проверяем индексы перед memmove
        if (l1 >= height * bytes_per_line || l2 >= height * bytes_per_line) {
            delete[] line;
            return false; // Или выведите сообщение об ошибке
        }

        memcpy(line, data + l1, bytes_per_line);  // Используем memcpy вместо memmove
        memcpy(data + l1, data + l2, bytes_per_line);
        memcpy(data + l2, line, bytes_per_line);

    }

    delete[] line;
    return true;
}

bool TGAImage::flip_horizontally() {
    if (!data) return false;
    for (int i = 0; i < width>>1; i++) {
        for (int j = 0; j < height; j++) {
            TGAColor c1 = get(i, j);
			TGAColor c2 = get(width-1-i, j);
			set(i, j, c2);
			set(width-1-i, j, c1);
        }
    }
    return true;
}

bool TGAImage::write_TGA_file(const char *filename, bool rle) {
    unsigned char developer_area_ref[4] = {0, 0, 0, 0};
	unsigned char extension_area_ref[4] = {0, 0, 0, 0};
	unsigned char footer[18]= {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
    std::ofstream out;
    out.open(filename, std::ios::binary);
    if (!out.is_open()) {	
		std::cerr << "Can't open file " << filename << std::endl;
		out.close();
		return false;	
	}
    TGA_Header header;
    memset((void*)&header, 0, sizeof(header));
    header.height = height;
    header.width = width;
    header.bitsperpixel = bytespp<<3;
    header.imagedescriptor = 0x20;
	header.datatypecode = (bytespp==GRAYSCALE?(rle?11:3):(rle?10:2));
    out.write((char*)&header, sizeof(header));
    if (!out.good()) {
		out.close();
		std::cerr << "Can't write TGA header" << std::endl;
		return false;
	}
    if (!rle) {
        out.write((char*)data, width*height*bytespp);
        if (!out.good()) {
		    out.close();
		    std::cerr << "Can't write DATA" << std::endl;
		    return false;
	    }
    } else {
        if (!unload_RLE_data(out)) {
            out.close();
		    std::cerr << "Can't unload RLE data" << std::endl;
		    return false;
        }
    }
    out.write((char *)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write((char *)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write((char *)footer, sizeof(footer));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.close();
	return true;
}

bool TGAImage::unload_RLE_data(std::ofstream &out) {
	const unsigned char max_chunk_length = 128;
	unsigned long npixels = width*height;
	unsigned long curpix = 0;
	while (curpix<npixels) {
		unsigned long chunkstart = curpix*bytespp;
		unsigned long curbyte = curpix*bytespp;
		unsigned char run_length = 1;
		bool raw = true;
		while (curpix+run_length<npixels && run_length<max_chunk_length) {
			bool succ_eq = true;
			for (int t=0; succ_eq && t<bytespp; t++) {
				succ_eq = (data[curbyte+t]==data[curbyte+t+bytespp]);
			}
			curbyte += bytespp;
			if (1==run_length) {
				raw = !succ_eq;
			}
			if (raw && succ_eq) {
				run_length--;
				break;
			}
			if (!raw && !succ_eq) {
				break;
			}
			run_length++;
		}
		curpix += run_length;
		out.put(raw?run_length-1:run_length+127);
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write((char *)(data+chunkstart), (raw?run_length*bytespp:bytespp));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
	}
	return true;
}
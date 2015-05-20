/*
 * AlloyFileUtil.cpp
 *
 *  Created on: May 16, 2015
 *      Author: blake
 */
#include <iostream>
#include <algorithm>
#include <png.h>
#include <fstream>
#include <vector>
#include "stdint.h"
#include "AlloyFileUtil.h"
#include "stb_image.h"
using namespace std;

namespace aly {

std::string ReadTextFile(const std::string& str){
	std::ifstream myfile;
	myfile.open(str);
	if(myfile.is_open()){
		std::stringstream buffer;
		std::string line;
		while(getline(myfile,line)){
			buffer<<line<<std::endl;
		}
		return buffer.str();
	} else {
		throw std::runtime_error(MakeString()<<"Could not open "<<str);
	}
}
std::vector<char> ReadBinaryFile(const std::string& str){
	streampos size;
	  ifstream file (str, ios::in|ios::binary|ios::ate);
	  if (file.is_open())
	  {
	    size = file.tellg();
	    std::vector<char> memblock(size);
	    file.seekg (0, ios::beg);
	    file.read(memblock.data(), size);
	    file.close();
	    return memblock;
	} else throw std::runtime_error(MakeString()<<"Could not open "<<str);
}
bool FileExists(const std::string& name){
	std::ifstream file(name,ios::in|ios::binary);
	if(file.is_open()){
		file.close();
		return true;
	} return false;
}

std::string GetFileExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos)
		return fileName.substr(fileName.find_last_of(".") + 1);
	return string("");
}

std::string GetFileName(const std::string& fileName) {
	if (fileName.find_last_of(PATH_SEPARATOR) != string::npos)return fileName.substr(fileName.find_last_of(PATH_SEPARATOR) + 1);
	return fileName;
}
std::string RemoveTrailingSlash(const std::string& file) {
	if (file.find_last_of(PATH_SEPARATOR) != string::npos) {
		size_t end =file.find_last_of(PATH_SEPARATOR);
		return file.substr(0, end);
	}
	return file;
}
std::string ConcatPath(const std::string& dir, const std::string& file) {
	return RemoveTrailingSlash(dir) + PATH_SEPARATOR+file;
}
std::string GetFileNameWithoutExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos
			&& fileName.find_last_of(PATH_SEPARATOR) != string::npos) {
		size_t start = fileName.find_last_of(PATH_SEPARATOR) + 1;
		size_t end = fileName.find_last_of(".");
		return fileName.substr(start, end - start);
	}
	return fileName;
}

std::string GetFileWithoutExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos) {
		size_t end = fileName.find_last_of(".");
		return fileName.substr(0, end);
	}
	return fileName;
}
std::string ReplaceFileExtension(const std::string& file,const std::string& ext){
	return GetFileWithoutExtension(file)+"."+ext;
}
std::string GetFileDirectoryPath(const std::string& fileName) {
	if (fileName.find_last_of(PATH_SEPARATOR) != string::npos) {
		size_t end = fileName.find_last_of(PATH_SEPARATOR);
		return fileName.substr(0, end);
	}
	return fileName;
}

//Only works on Linux for NOW!
std::vector<std::string> GetDirectoryListing(const std::string& dirName, const std::string& ext, const std::string& mask) {
	std::vector<std::string> files;
	dirent* dp;
	std::string cleanPath = RemoveTrailingSlash(dirName) + PATH_SEPARATOR;
	DIR* dirp = opendir(cleanPath.c_str());
	while ((dp = readdir(dirp)) != NULL) {
		string fileName(dp->d_name);
		if (dp->d_type == DT_REG) {
			if (ext.length() == 0 || GetFileExtension(fileName) == ext) {
				if (mask.length() == 0
						|| (mask.length() > 0
								&& fileName.find(mask) < fileName.length()))
					files.push_back(cleanPath + fileName);
			}
		}
	}
	closedir(dirp);
	std::sort(files.begin(), files.end());
	return files;
}
void WriteImageToFile(const std::string& _file, const ImageRGB& image) {
	std::string file=ReplaceFileExtension(_file,"png");
	int width = image.width;
	int height = image.height;
	const char* file_name = file.c_str();
	png_byte color_type = PNG_COLOR_TYPE_RGB;
	png_byte bit_depth = 8;
	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	int x, y;
	int index = 0;

	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp) {
		throw std::runtime_error(
				MakeString() << "[write_png_file] File " << file_name
						<< " could not be opened for writing");
	}

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
	NULL, NULL);

	if (!png_ptr) {
		throw std::runtime_error(
				"[write_png_file] png_create_write_struct failed");
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error(
				"[write_png_file] png_create_info_struct failed");
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[write_png_file] Error during init_io");
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error(
				"[write_png_file] Error during writing header");
	}
	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
	PNG_INTERLACE_NONE,
	PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*) malloc(
				png_get_rowbytes(png_ptr, info_ptr));
		png_byte* ptr = row_pointers[y];
		for (x = 0; x < width; x++) {
			ubyte3 c = image[index++];
			ptr[0] = c[0];
			ptr[1] = c[1];
			ptr[2] = c[2];
			ptr += 3;
		}
	}
	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[write_png_file] Error during writing bytes");
	}
	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[write_png_file] Error during end of write");
	}
	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	fclose(fp);
}
void WriteImageToFile(const std::string& _file, const ImageRGBA& image) {
	std::string file=ReplaceFileExtension(_file,"png");
	int width = image.width;
	int height = image.height;
	const char* file_name = file.c_str();
	png_byte color_type = PNG_COLOR_TYPE_RGBA;
	png_byte bit_depth = 8;
	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	int x, y;
	int index = 0;

	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp) {
		throw std::runtime_error(
				MakeString() << "[write_png_file] File " << file_name
						<< " could not be opened for writing");
	}

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
	NULL, NULL);

	if (!png_ptr) {
		throw std::runtime_error(
				"[write_png_file] png_create_write_struct failed");
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error(
				"[write_png_file] png_create_info_struct failed");
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[write_png_file] Error during init_io");
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error(
				"[write_png_file] Error during writing header");
	}
	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
	PNG_INTERLACE_NONE,
	PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*) malloc(
				png_get_rowbytes(png_ptr, info_ptr));
		png_byte* ptr = row_pointers[y];
		for (x = 0; x < width; x++) {
			ubyte4 c = image[index++];
			ptr[0] = c[0];
			ptr[1] = c[1];
			ptr[2] = c[2];
			ptr[3] = c[3];
			ptr += 4;
		}
	}
	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[write_png_file] Error during writing bytes");
	}
	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[write_png_file] Error during end of write");
	}
	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	fclose(fp);
}
void ReadImageFromFile(const std::string& file, ImageRGBA& image) {
	std::string ext=GetFileExtension(file);
	if(ext=="jpg"||ext=="jpeg"){
		unsigned char* img;
		stbi_set_unpremultiply_on_load(1);
		stbi_convert_iphone_png_to_rgb(1);
		int w,h,n;
		img = stbi_load(file.c_str(), &w, &h, &n, 4);
		if (img == NULL) {
			throw std::runtime_error(
					MakeString() << "[read_jpg_file] File " << file << " is not recognized as a JPG file");
		}
		image.resize(w,h);
		image.set(img);
		stbi_image_free(img);
		return;
	}
	int x, y;
	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	unsigned char header[8];    // 8 is the maximum size that can be checked
	const char* file_name = file.c_str();
	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp) {
		throw std::runtime_error(
				MakeString() << "[read_png_file] File " << file_name
						<< " could not be opened for reading");
	}
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8)) {
		throw std::runtime_error(
				MakeString() << "[read_png_file] File " << file_name
						<< " is not recognized as a PNG file");
	}

	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		throw std::runtime_error(
				"[read_png_file] png_create_read_struct failed");

	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error(
				"[read_png_file] png_create_info_struct failed");

	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[read_png_file] Error during init_io");

	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[read_png_file] Error during read_image");

	}

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y = 0; y < height; y++)
		row_pointers[y] = (png_byte*) malloc(
				png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);
	fclose(fp);
	image.resize(width, height);
	int index = 0;
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGBA) {
		for (y = 0; y < height; y++) {
			png_byte* row = row_pointers[y];
			for (x = 0; x < width; x++) {
				png_byte* ptr = &(row[x * 4]);
				ubyte4 rgba = ubyte4(ptr[0], ptr[1], ptr[2], ptr[3]);
				image[index++] = rgba;
			}
		}
	} else if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB) {
		for (y = 0; y < height; y++) {
			png_byte* row = row_pointers[y];
			for (x = 0; x < width; x++) {
				png_byte* ptr = &(row[x * 3]);
				ubyte4 rgba = ubyte4(ptr[0], ptr[1], ptr[2], 255);
				image[index++] = rgba;
			}
		}
	} else {
		throw std::runtime_error("Color format not supported");
		for (y = 0; y < height; y++)
			free(row_pointers[y]);
		free(row_pointers);

	}
	image.width = width;
	image.height = height;
	/* cleanup heap allocation */
	for (y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);

}
void ReadImageFromFile(const std::string& file, ImageRGB& image) {
	std::string ext=GetFileExtension(file);
	if(ext=="jpg"||ext=="jpeg"){
		unsigned char* img;
		stbi_set_unpremultiply_on_load(1);
		stbi_convert_iphone_png_to_rgb(1);
		int w,h,n;
		img = stbi_load(file.c_str(), &w, &h, &n, 3);
		if (img == NULL) {
			throw std::runtime_error(
					MakeString() << "[read_jpg_file] File " << file << " is not recognized as a JPG file");
		}
		image.resize(w,h);
		image.set(img);
		stbi_image_free(img);
		return;
	}
	int x, y;
	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	unsigned char header[8];    // 8 is the maximum size that can be checked
	const char* file_name = file.c_str();
	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp) {
		throw std::runtime_error(
				MakeString() << "[read_png_file] File " << file_name
						<< " could not be opened for reading");
	}
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8)) {
		throw std::runtime_error(
				MakeString() << "[read_png_file] File " << file_name
						<< " is not recognized as a PNG file");
	}

	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		throw std::runtime_error(
				"[read_png_file] png_create_read_struct failed");

	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		throw std::runtime_error(
				"[read_png_file] png_create_info_struct failed");

	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[read_png_file] Error during init_io");

	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw std::runtime_error("[read_png_file] Error during read_image");

	}

	row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (y = 0; y < height; y++)
		row_pointers[y] = (png_byte*) malloc(
				png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);
	fclose(fp);
	image.resize(width, height);
	int index = 0;
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGBA) {
		for (y = 0; y < height; y++) {
			png_byte* row = row_pointers[y];
			for (x = 0; x < width; x++) {
				png_byte* ptr = &(row[x * 4]);
				ubyte3 rgb = ubyte3(ptr[0], ptr[1], ptr[2]);
				image[index++] = rgb;
			}
		}
	} else if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB) {
		for (y = 0; y < height; y++) {
			png_byte* row = row_pointers[y];
			for (x = 0; x < width; x++) {
				png_byte* ptr = &(row[x * 3]);
				ubyte3 rgb = ubyte3(ptr[0], ptr[1], ptr[2]);
				image[index++] = rgb;
			}
		}
	} else {
		throw std::runtime_error("Color format not supported");
		for (y = 0; y < height; y++)
			free(row_pointers[y]);
		free(row_pointers);

	}
	image.width = width;
	image.height = height;
	/* cleanup heap allocation */
	for (y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);

}
}


/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "AlloyImage.h"
#include "AlloyCommon.h"
#include "AlloyFileUtil.h"
#include "AlloyMath.h"
#include "stb_image.h"
#include <fstream>
#include <png.h>

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
namespace aly {
	void WriteImageToFile(const std::string& _file, const ImageRGB& image) {
		std::string file = ReplaceFileExtension(_file, "png");
		int width = image.width;
		int height = image.height;
		const char* file_name = file.c_str();
		png_byte color_type = PNG_COLOR_TYPE_RGB;
		png_byte bit_depth = 8;
		png_structp png_ptr;
		png_infop info_ptr;
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
		row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (y = 0; y < height; y++) {
			row_pointers[y] = (png_byte*)malloc(
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
		std::string file = ReplaceFileExtension(_file, "png");
		int width = image.width;
		int height = image.height;
		const char* file_name = file.c_str();
		png_byte color_type = PNG_COLOR_TYPE_RGBA;
		png_byte bit_depth = 8;
		png_structp png_ptr;
		png_infop info_ptr;
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
		row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (y = 0; y < height; y++) {
			row_pointers[y] = (png_byte*)malloc(
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
		std::string ext = GetFileExtension(file);
		if (ext == "jpg" || ext == "jpeg") {
			unsigned char* img;
			stbi_set_unpremultiply_on_load(1);
			stbi_convert_iphone_png_to_rgb(1);
			int w, h, n;
			img = stbi_load(file.c_str(), &w, &h, &n, 4);
			if (img == NULL) {
				throw std::runtime_error(
					MakeString() << "[read_jpg_file] File " << file
					<< " is not recognized as a JPG file");
			}
			image.resize(w, h);
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

		row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (y = 0; y < height; y++)
			row_pointers[y] = (png_byte*)malloc(
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
		}
		else if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB) {
			for (y = 0; y < height; y++) {
				png_byte* row = row_pointers[y];
				for (x = 0; x < width; x++) {
					png_byte* ptr = &(row[x * 3]);
					ubyte4 rgba = ubyte4(ptr[0], ptr[1], ptr[2], 255);
					image[index++] = rgba;
				}
			}
		}
		else {
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
		std::string ext = GetFileExtension(file);
		if (ext == "jpg" || ext == "jpeg") {
			unsigned char* img;
			stbi_set_unpremultiply_on_load(1);
			stbi_convert_iphone_png_to_rgb(1);
			int w, h, n;
			img = stbi_load(file.c_str(), &w, &h, &n, 3);
			if (img == NULL) {
				throw std::runtime_error(
					MakeString() << "[read_jpg_file] File " << file
					<< " is not recognized as a JPG file");
			}
			image.resize(w, h);
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

		row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (y = 0; y < height; y++)
			row_pointers[y] = (png_byte*)malloc(
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
		}
		else if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB) {
			for (y = 0; y < height; y++) {
				png_byte* row = row_pointers[y];
				for (x = 0; x < width; x++) {
					png_byte* ptr = &(row[x * 3]);
					ubyte3 rgb = ubyte3(ptr[0], ptr[1], ptr[2]);
					image[index++] = rgb;
				}
			}
		}
		else {
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

	void ReadImageFromFile(const std::string& file, ImageRGBAf& img) {
		ImageRGBA rgb;
		ReadImageFromFile(file, rgb);
		img.resize(rgb.width, rgb.height);
		img.id = rgb.id;
		img.x = rgb.x;
		img.y = rgb.y;
		size_t index = 0;
		for (RGBAf& ct : img.data) {
			RGBA cs = rgb[index++];
			ct = RGBAf(cs.x / 255.0f, cs.y / 255.0f, cs.z / 255.0f, cs.w / 255.0f);
		}
	}
	void ReadImageFromFile(const std::string& file, ImageRGBf& img) {
		ImageRGB rgb;
		ReadImageFromFile(file, rgb);
		img.resize(rgb.width, rgb.height);
		img.id = rgb.id;
		img.x = rgb.x;
		img.y = rgb.y;
		size_t index = 0;
		for (RGBf& ct : img.data) {
			RGB cs = rgb[index++];
			ct = RGBf(cs.x / 255.0f, cs.y / 255.0f, cs.z / 255.0f);
		}
	}
	void WriteImageToFile(const std::string& file, const ImageRGBAf& img) {
		ImageRGBA rgb;
		rgb.resize(img.width, img.height);
		rgb.id = img.id;
		rgb.x = img.x;
		rgb.y = img.y;
		size_t index = 0;
		for (RGBAf& ct : img.data) {
			rgb[index++] = RGBA(
				clamp((int)(ct.x * 255.0f), 0, 255),
				clamp((int)(ct.y * 255.0f), 0, 255),
				clamp((int)(ct.z * 255.0f), 0, 255),
				clamp((int)(ct.w * 255.0f), 0, 255));
		}
		WriteImageToFile(file, rgb);
	}
	void WriteImageToFile(const std::string& file, const ImageRGBf& img) {
		ImageRGB rgb;
		rgb.resize(img.width, img.height);
		rgb.id = img.id;
		rgb.x = img.x;
		rgb.y = img.y;
		size_t index = 0;
		for (RGBf& ct : img.data) {
			rgb[index++] = RGB(
				clamp((int)(ct.x * 255.0f), 0, 255),
				clamp((int)(ct.y * 255.0f), 0, 255),
				clamp((int)(ct.z * 255.0f), 0, 255));
		}
		WriteImageToFile(file, rgb);
	}

	void ConvertImage(const ImageRGBf& in, ImageRGB& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBf& ct : in.data) {
			out[index++] = RGB(
				clamp((int)(ct.x * 255.0f), 0, 255),
				clamp((int)(ct.y * 255.0f), 0, 255),
				clamp((int)(ct.z * 255.0f), 0, 255));
		}
	}
	void ConvertImage(const ImageRGBAf& in, ImageRGBA& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBAf& ct : in.data) {
			out[index++] = RGBA(
				clamp((int)(ct.x * 255.0f), 0, 255),
				clamp((int)(ct.y * 255.0f), 0, 255),
				clamp((int)(ct.z * 255.0f), 0, 255),
				clamp((int)(ct.w * 255.0f), 0, 255));
		}
	}
	void ConvertImage(const ImageRGBA& in, ImageRGBAf& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBAf& ct : out.data) {
			RGBA cs = in[index++];
			ct = RGBAf(cs.x / 255.0f, cs.y / 255.0f, cs.z / 255.0f, cs.w / 255.0f);
		}
	}
	void ConvertImage(const ImageRGB& in, ImageRGBf& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBf& ct : out.data) {
			RGB cs = in[index++];
			ct = RGBf(cs.x / 255.0f, cs.y / 255.0f, cs.z / 255.0f);
		}
	}
	void ConvertImage(const ImageRGB& in, ImageRGBA& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBA& ct : out.data) {
			RGB cs = in[index++];
			ct = RGBA(cs,255);
		}
	}
	void ConvertImage(const ImageRGBf& in, ImageRGBAf& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBAf& ct : out.data) {
			RGBf cs = in[index++];
			ct = RGBAf(cs, 1.0f);
		}
	}
	void ConvertImage(const ImageRGBAf& in, ImageRGBf& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGBf& ct : out.data) {
			RGBAf cs = in[index++];
			ct = cs.xyz();
		}
	}
	void ConvertImage(const ImageRGBA& in, ImageRGB& out) {
		out.resize(in.width, in.height);
		out.id = in.id;
		out.x = in.x;
		out.y = in.y;
		size_t index = 0;
		for (RGB& ct : out.data) {
			RGBA cs = in[index++];
			ct = cs.xyz();
		}
	}
}

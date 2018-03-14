#pragma once
#ifdef ACS_IOIMAGE_EXPORTS
#define ACS_IOIMAGE_API __declspec(dllexport)
#else
#define ACS_IOIMAGE_API __declspec(dllimport)
#endif
#include <exception>
#include <acs_def.h>
//use acs_ioImage.lib 
//			& acs_ioImage.dll

//
//画像をRGBA(32bit)で読み込みます。
//画像を任意の形式の方法で保存します
//

//
//ioImage--画像入出力
//
//
//ImageData--画像データ
//
namespace acs {
	namespace image {
		struct ExceptionImageLoader :std::exception {
			explicit ExceptionImageLoader() : std::exception("Failed to init ImageLoader") {

			}
		};

		enum struct IMAGE_FILE_FOMAT :unsigned int {
			AUTO = 0, BMP = 1, PNG = 2, JPEG = 3
		};
		enum struct IMAGE_PIXEL_FOMAT :unsigned int {
			AUTO = 0, RGBA32 = 1, RGB32 = 2, BGR32 = 3, CMYK32 = 4,
		};
		typedef struct __IMAGE_STATE {
			unsigned int Width = 0;
			unsigned int Height = 0;
			IMAGE_PIXEL_FOMAT PixelFomat = IMAGE_PIXEL_FOMAT::RGBA32;//1ピクセルの大きさ
			unsigned int DataByteLength = 0;//データの長さ
		}IMAGE_STATE;

		class IImageData :public IACS {
		public:
			virtual bool ACS_TCALL GetState(IMAGE_STATE* o_imgstate) = 0;
			virtual acs::byte* ACS_TCALL GetData() = 0;
			virtual bool ACS_TCALL Copy(IImageData**) = 0;
		};
		class IioImage :public IACS {
		public:
			virtual bool ACS_TCALL CleateImageFromFilename(const wchar_t* name, IImageData**) = 0;
			virtual bool ACS_TCALL CleateImage(const IMAGE_STATE&, IImageData**) = 0;
			virtual bool ACS_TCALL SaveImage(IImageData* __img, IMAGE_PIXEL_FOMAT pixelFomat, IMAGE_FILE_FOMAT fileFomat, const wchar_t* pass) = 0;
		};

		ACS_IOIMAGE_API bool ACS_SCALL CreateioImage(IioImage**);
	}
}
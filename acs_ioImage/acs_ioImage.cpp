// acs_ioImage.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "include\acs_ioImage.h"
#include "acs\include\def.h"

#include<wincodec.h>
#include<atlcomcli.h>
#include <memory>
#include <Shlwapi.h>
#pragma comment (lib,"Shlwapi.lib")

#define HR_CHECK(hr) if(FAILED(hr))return false;
namespace {
	class IOIMAGE_INIT_ERR :public std::exception {
	public:
		IOIMAGE_INIT_ERR() :std::exception("IOIMAGE_INIT_ERR") {

		}
	};
}

namespace acs {
	namespace image {
		class M__Image : public IImageData {
			IMAGE_STATE state;
			BYTE* data;
		public:
			_IACS_CHILD;
			explicit M__Image(const IMAGE_STATE& imgstate) {
				data = new BYTE[imgstate.DataByteLength];
				state = imgstate;
			}
			ACS_TCALL ~M__Image() {
				delete[] data;
			}
			bool ACS_TCALL GetState(IMAGE_STATE* o_imgstate) {
				*o_imgstate = state;
				return true;
			}
			BYTE* ACS_TCALL GetData() {
				return data;
			}
			bool ACS_TCALL Copy(IImageData** out) {
				*out = new M__Image(state);
				memcpy((*out)->GetData(), data, state.DataByteLength);
				return true;
			}
		};

		class M__ImageLoader :public IioImage {
			CComPtr<IWICImagingFactory> factory = nullptr;
		public:
			_IACS_CHILD;
			ACS_TCALL M__ImageLoader() {
				HRESULT hr;
				hr = CoCreateInstance(
					CLSID_WICImagingFactory,
					nullptr,
					CLSCTX_INPROC_SERVER,
					__uuidof(IWICImagingFactory),
					reinterpret_cast<void**>(&factory));
				if (FAILED(hr))throw(IOIMAGE_INIT_ERR());
			}
			ACS_TCALL ~M__ImageLoader() {
			}

			const GUID cvDec(IMAGE_FILE_FOMAT imf) {
				switch (imf)
				{
				case IMAGE_FILE_FOMAT::BMP:
					return CLSID_WICBmpEncoder;
					break;
				case IMAGE_FILE_FOMAT::PNG:
					return CLSID_WICPngEncoder;
					break;
				case IMAGE_FILE_FOMAT::JPEG:
					return CLSID_WICJpegEncoder;
					break;
					/*
				case IMAGE_FILE_FOMAT::GIF:
					return CLSID_WICGifEncoder;
					break;
				case IMAGE_FILE_FOMAT::TIFF:
					return CLSID_WICTiffEncoder;
					break;
				case IMAGE_FILE_FOMAT::WMP:
					return CLSID_WICWmpEncoder;
					break;
					*/
				case IMAGE_FILE_FOMAT::AUTO:
				default:
					return CLSID_WICPngEncoder;
					break;
				}
			}
			const GUID cvDefPF(IMAGE_FILE_FOMAT imf) {
				switch (imf)
				{
				case IMAGE_FILE_FOMAT::BMP:
					return GUID_WICPixelFormat32bppBGR;
					break;
				case IMAGE_FILE_FOMAT::PNG:
					return GUID_WICPixelFormat32bppRGBA;
					break;
				case IMAGE_FILE_FOMAT::JPEG:
					return GUID_WICPixelFormat32bppCMYK;
					break;
					/*
					case IMAGE_FILE_FOMAT::GIF:
					return CLSID_WICGifEncoder;
					break;
					case IMAGE_FILE_FOMAT::TIFF:
					return CLSID_WICTiffEncoder;
					break;
					case IMAGE_FILE_FOMAT::WMP:
					return CLSID_WICWmpEncoder;
					break;
					*/
				case IMAGE_FILE_FOMAT::AUTO:
				default:
					return GUID_WICPixelFormat32bppRGBA;
					break;
				}
			}
			const GUID cvPF(IMAGE_PIXEL_FOMAT imf) {
				switch (imf)
				{
				case IMAGE_PIXEL_FOMAT::RGBA32:
					return GUID_WICPixelFormat32bppRGBA;
					break;
				case IMAGE_PIXEL_FOMAT::RGB32:
					return GUID_WICPixelFormat32bppRGB;
					break;
				case IMAGE_PIXEL_FOMAT::BGR32:
					return GUID_WICPixelFormat32bppBGR;
					break;
				case IMAGE_PIXEL_FOMAT::CMYK32:
					return GUID_WICPixelFormat32bppCMYK;
					break;
				case IMAGE_PIXEL_FOMAT::AUTO:
				default:
					return GUID_WICPixelFormat32bppRGB;
					break;
				}
			}

			bool ACS_TCALL CleateImageFromFilename(const wchar_t* pass, IImageData** __img) {
				CComPtr<IWICBitmapDecoder>decoder;
				CComPtr<IWICBitmapFrameDecode> bitmapSource;
				CComPtr<IWICFormatConverter> converter;
				CComPtr<IWICBitmapSource> bitmap;

				HRESULT hr = factory->CreateDecoderFromFilename(
					pass,
					nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
				HR_CHECK(hr);
				hr = decoder->GetFrame(0, &bitmapSource);

				UINT width, height;
				WICPixelFormatGUID Format;

				if SUCCEEDED(hr)hr = bitmapSource->GetSize(&width, &height);
				if SUCCEEDED(hr)hr = bitmapSource->GetPixelFormat(&Format);

				if SUCCEEDED(hr) {
					if (Format != GUID_WICPixelFormat32bppRGBA) {
						hr = factory->CreateFormatConverter(&converter);
						if SUCCEEDED(hr)hr = converter->Initialize(
							bitmapSource, GUID_WICPixelFormat32bppRGBA,
							WICBitmapDitherTypeNone, nullptr, 0.f,
							WICBitmapPaletteTypeMedianCut);
						bitmap = converter;
					}
					else {
						bitmap = bitmapSource;
					}
					acs::SIACS<M__Image>c_img;
					IMAGE_STATE state;
					state.Width = width;
					state.Height = height;
					state.PixelFomat = IMAGE_PIXEL_FOMAT::RGBA32;
					state.DataByteLength = width * height * 4;
					if SUCCEEDED(hr) { c_img = new M__Image(state); }
					else return false;
					hr = bitmap->CopyPixels(0, width * 4, width * height * 4, c_img->GetData());
					if (FAILED(hr))return false;

					(*__img) = c_img;
				}
				return SUCCEEDED(hr);
			}
			bool ACS_TCALL CleateImage(const IMAGE_STATE& stat, IImageData**__img) {
				*__img = new M__Image(stat);
				return true;
			}
			bool ACS_TCALL SaveImage(IImageData* __img, IMAGE_PIXEL_FOMAT pixelFomat, IMAGE_FILE_FOMAT fileFomat, const wchar_t* pass) {
				CComPtr<IWICBitmapEncoder>encoder;
				CComPtr<IWICFormatConverter> converter;
				CComPtr<IWICBitmap>bitmap;
				CComPtr<IStream> stream;
				HRESULT hr;
				GUID _fileFomat = cvDec(fileFomat);
				GUID _pixelFomat;
				if (pixelFomat == IMAGE_PIXEL_FOMAT::AUTO)_pixelFomat = cvDefPF(fileFomat);
				else _pixelFomat = cvPF(pixelFomat);
				{
					CComPtr<IWICFormatConverter> converter;
					IMAGE_STATE ist;
					__img->GetState(&ist);
					hr = factory->CreateBitmapFromMemory(ist.Width, ist.Height, cvPF(ist.PixelFomat), ist.Width, ist.DataByteLength, __img->GetData(), &bitmap);
					HR_CHECK(hr);
					hr = factory->CreateFormatConverter(&converter);
					HR_CHECK(hr);
					if (_pixelFomat != cvPF(ist.PixelFomat)) {
						hr = converter->Initialize(
							bitmap, _pixelFomat,
							WICBitmapDitherTypeNone, nullptr, 0.f,
							WICBitmapPaletteTypeMedianCut);
						bitmap = converter;
						HR_CHECK(hr);
					}
				}
				hr = factory->CreateEncoder(
					_fileFomat,
					nullptr,
					&encoder);
				HR_CHECK(hr);
				hr = SHCreateStreamOnFileEx(pass, STGM_WRITE | STGM_FAILIFTHERE, 0, TRUE, NULL, &stream);
				HR_CHECK(hr);
				hr = encoder->Initialize(stream, WICBitmapEncoderNoCache);
				HR_CHECK(hr);
				{
					CComPtr<IWICBitmapFrameEncode> frame;
					CComPtr<IPropertyBag2> prop;
					hr = encoder->CreateNewFrame(&frame, &prop);
					HR_CHECK(hr);
					hr = frame->Initialize(prop);
					HR_CHECK(hr);
					IMAGE_STATE ist;
					__img->GetState(&ist);
					hr = frame->WriteSource(bitmap, nullptr);
					HR_CHECK(hr);
					hr = frame->Commit();
				}
				HR_CHECK(hr);
				hr = encoder->Commit();
				return SUCCEEDED(hr);
			}
		};

		ACS_IOIMAGE_API bool ACS_SCALL CreateioImage(IioImage** ol) {
			try {
				M__ImageLoader* loader = new M__ImageLoader();
				*ol = loader;
			}
			catch (const IOIMAGE_INIT_ERR&) {
				return false;
			}
			return true;
		}
	}
}

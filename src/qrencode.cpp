#include "qrencode.h"
#include "QRCodeGenerator.h"
#include <png.h>
#include <string.h>
#include <stdlib.h>
#include <string>

#define RETURN(val) return(val)

#define QR_CONTENT_MAX_SIZE 512
#define QR_CHANNEL_COUNT    3       // RGB

#define COLOR_GET_R(color) (((color) & 0x00FF0000) >> 16)
#define COLOR_GET_G(color) (((color) & 0x0000FF00) >> 8 )
#define COLOR_GET_B(color) (((color) & 0x000000FF) >> 0 )

#define CONSOLE(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#if !WIN32
#ifndef memcpy_s
#define memcpy_s(dest, destSize, src, srcSize) memcpy(dest, src, srcSize)
#endif // !memcpy_s
#endif // !WIN32

struct qrencode_t {
    CQR_Encode  core;
    int         width;
    int         version;
    int         quality;
    int         color;
    int         background_color;
};

static int  to_inner_qr_version(unsigned int version);
static int  to_inner_qr_quality(unsigned int quality);
static void stream_write_callback(png_structrp png_ptr, png_bytep bytes, size_t size);
static void stream_flush_callback(png_structrp png_ptr);
static void png_error_callback(png_structrp png_ptr, png_const_charp message);
static void png_warning_callback(png_structrp png_ptr, png_const_charp message);
static void fill_pixels(png_bytep pixels, unsigned int x, unsigned int rgb, unsigned int pixel_size);
static int  get_default_qrcode_image_width(int qrcode_width);

qrencode_ptr_t QRENCODE_CAPI qrencode_new_encoder()
{
    qrencode_ptr_t ptr = new qrencode_t();
    if (NULL == ptr)
    {
        // ERROR
        return NULL;
    }
    ptr->width = -1;
    ptr->version = QRENCODE_VERSION_S;
    ptr->quality = QRENCODE_LEVEL_L;
    ptr->color = 0x000000;
    ptr->background_color = 0xffffff;
    return ptr;
}

int QRENCODE_CAPI qrencode_set_version(qrencode_ptr_t encoder, unsigned int version)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }

    if (
        version != QRENCODE_VERSION_S &&
        version != QRENCODE_VERSION_M &&
        version != QRENCODE_VERSION_L
        )
    {
        RETURN(QRENCODE_UNSUPPORTED_VERSION);
    }

    encoder->version = version;

    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_set_width(qrencode_ptr_t encoder, unsigned int width)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }

    if (width == 0 || width >= 0xFFFFFFFF)
    {
        RETURN(QRENCODE_INVALID_PARAM);
    }

    encoder->width = width;

    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_set_color(qrencode_ptr_t encoder, unsigned int rgb)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }
    encoder->color = rgb & 0X00FFFFFF;
    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_set_background_color(qrencode_ptr_t encoder, unsigned int rgb)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }
    encoder->background_color = rgb & 0X00FFFFFF;
    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_set_quality(qrencode_ptr_t encoder, unsigned int quality)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }

    if (
        quality != QRENCODE_LEVEL_L &&
        quality != QRENCODE_LEVEL_M &&
        quality != QRENCODE_LEVEL_Q &&
        quality != QRENCODE_LEVEL_H
        )
    {
        RETURN(QRENCODE_UNSUPPORTED_QUALITY);
    }

    encoder->quality = quality;

    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_encode(qrencode_ptr_t encoder, const char* content)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }

    if (NULL == content)
    {
        RETURN(QRENCODE_INVALID_PARAM);
    }

    int contentLength = ::strlen(content);
    if (contentLength == 0 || contentLength > QR_CONTENT_MAX_SIZE)
    {
        RETURN(QRENCODE_INVALID_PARAM);
    }

    CQR_Encode& core = encoder->core;

    bool bSuccess = core.EncodeData(
        to_inner_qr_quality(encoder->quality),
        to_inner_qr_version(encoder->version),
        true,
        QR_MODE_8BIT,
        content
    );
    if (!bSuccess)
    {
        RETURN(QRENCODE_FAIL);
    }

    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_get_encoded_matrix(qrencode_ptr_t encoder, unsigned char ** outBytes, unsigned int * width)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }

    if (NULL == outBytes || NULL == width)
    {
        RETURN(QRENCODE_INVALID_PARAM);
    }

    int qrcodeWidth = encoder->core.m_nSymbleSize;
    if (qrcodeWidth == 0)
    {
        RETURN(QRENCODE_NEED_ENCODE);
    }

    unsigned int remain = qrcodeWidth * qrcodeWidth;
    *outBytes = new unsigned char[remain]();
    *width = qrcodeWidth;
    for (unsigned int i = 0; i < qrcodeWidth; i++)
    {
        ::memcpy_s(*outBytes + i * qrcodeWidth, remain, encoder->core.m_byModuleData[i], qrcodeWidth);
        remain -= qrcodeWidth;
    }

    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_get_encoded_minimum_width(qrencode_ptr_t encoder, unsigned int * width)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }
    if (NULL == width)
    {
        RETURN(QRENCODE_INVALID_PARAM);
    }
    if (encoder->core.m_nSymbleSize == 0)
    {
        RETURN(QRENCODE_NEED_ENCODE);
    }
    *width = encoder->core.m_nSymbleSize;
    RETURN(QRENCODE_SUCCESS);
}

int QRENCODE_CAPI qrencode_get_encoded_image(qrencode_ptr_t encoder, unsigned int width, unsigned char ** outBytes, unsigned int * outSize)
{
    if (NULL == encoder)
    {
        RETURN(QRENCODE_INVALID_HANDLE);
    }

    if (NULL == outBytes || NULL == outSize)
    {
        RETURN(QRENCODE_INVALID_PARAM);
    }

    if (width == 0)
    {
        width = get_default_qrcode_image_width(encoder->core.m_nSymbleSize);
    }

    int qrcodeWidth = encoder->core.m_nSymbleSize;
    if (qrcodeWidth == 0)
    {
        RETURN(QRENCODE_NEED_ENCODE);
    }
    if (width < qrcodeWidth)
    {
        RETURN(QRENCODE_WIDTH_TOO_SMALL);
    }
    int qrcodePixelSize = width / qrcodeWidth;

    // Recalculate image width.
    width = qrcodeWidth * qrcodePixelSize;

    // =================== Generate QR Code PNG ===================
    // Initialize write structure
    png_structp png_ptr = NULL;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        RETURN(QRENCODE_FAIL);
    }

    // Initialize info structure
    png_infop info_ptr = NULL;
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        RETURN(QRENCODE_FAIL);
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        RETURN(QRENCODE_FAIL);
    }

    png_set_error_fn(png_ptr, NULL, png_error_callback, png_warning_callback);

    // Write header (8 bit colour depth)
    png_set_IHDR(
        png_ptr,
        info_ptr,
        width,
        width,
        8,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE
    );

    std::string container;
    png_set_write_fn(png_ptr, &container, stream_write_callback, stream_flush_callback);

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    size_t rowSize = width * QR_CHANNEL_COUNT;
    png_bytep row = new png_byte[rowSize]();

    // Write image data
    for (int y = 0; y < qrcodeWidth; y++)
    {
        // Set all pixel to white color defaultly.
        for (int x = 0; x < qrcodeWidth; x++)
        {
            unsigned char qrcodeValue = encoder->core.m_byModuleData[y][x];
            if (qrcodeValue & 0x01)
            {
                fill_pixels(row, x, encoder->color, qrcodePixelSize);
            }
            else
            {
                fill_pixels(row, x, encoder->background_color, qrcodePixelSize);
            }
        }
        for (int i = 0; i < qrcodePixelSize; i++)
        {
            png_write_row(png_ptr, row);
        }
    }

    // End write.
    png_write_end(png_ptr, NULL);

    // Free memories.
    if (NULL != info_ptr)
    {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    }
    if (NULL != png_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    }
    if (NULL != row)
    {
        delete[] row;
        row = NULL;
    }
    // ============================================================

    *outBytes = new unsigned char[container.length()]();
    memcpy_s(*outBytes, container.length(), container.data(), container.length());
    *outSize = container.length();

    RETURN(QRENCODE_SUCCESS);
}

void QRENCODE_CAPI qrencode_free_memory(void * memory)
{
    if (NULL != memory)
    {
        delete[] memory;
    }
}

void QRENCODE_CAPI qrencode_free_encoder(qrencode_ptr_t encoder)
{
    if (NULL != encoder)
    {
        delete encoder;
    }
}

int to_inner_qr_version(unsigned int version)
{
    switch (version)
    {
    case QRENCODE_VERSION_S: return QR_VERSION_S;
    case QRENCODE_VERSION_M: return QR_VERSION_M;
    case QRENCODE_VERSION_L: return QR_VERSION_L;
    default:                 return QR_VERSION_S;
    }
}

int to_inner_qr_quality(unsigned int quality)
{
    switch (quality)
    {
    case QRENCODE_LEVEL_L: return QR_LEVEL_L;
    case QRENCODE_LEVEL_M: return QR_LEVEL_M;
    case QRENCODE_LEVEL_Q: return QR_LEVEL_Q;
    case QRENCODE_LEVEL_H: return QR_LEVEL_H;
    default:               return QR_LEVEL_L;
    }
}

void stream_write_callback(png_structrp png_ptr, png_bytep bytes, size_t size)
{
    std::string& container = *(static_cast<std::string*>(png_get_io_ptr(png_ptr)));
    container.append(bytes, bytes + size);
}

void stream_flush_callback(png_structrp png_ptr)
{
}

void png_error_callback(png_structrp png_ptr, png_const_charp message)
{
    CONSOLE("[ERROR]   %s", message);
}

void png_warning_callback(png_structrp png_ptr, png_const_charp message)
{
    CONSOLE("[WARNING] %s", message);
}

void fill_pixels(png_bytep row, unsigned int x, unsigned int rgb, unsigned int pixel_size)
{
    png_byte r = COLOR_GET_R(rgb);
    png_byte g = COLOR_GET_G(rgb);
    png_byte b = COLOR_GET_B(rgb);
    for (unsigned int i = 0; i < pixel_size; i++)
    {
        unsigned int pixel = ((x * pixel_size) + i) * QR_CHANNEL_COUNT;
        row[pixel + 0] = r;
        row[pixel + 1] = g;
        row[pixel + 2] = b;
    }
}

int get_default_qrcode_image_width(int qrcode_width)
{
    return qrcode_width * 20;
}

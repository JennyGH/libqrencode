#ifndef __LIB_QR_ENCODE_H__
#define __LIB_QR_ENCODE_H__
#ifndef QRENCODE_CAPI
#if WIN32
#define QRENCODE_CAPI __stdcall
#else
#define QRENCODE_CAPI
#endif // WIN32
#endif // !QRENCODE_CAPI

// Quality levels
#define QRENCODE_LEVEL_L    0
#define QRENCODE_LEVEL_M    1
#define QRENCODE_LEVEL_Q    2
#define QRENCODE_LEVEL_H    3

// Versions
#define QRENCODE_VERSION_S  0 
#define QRENCODE_VERSION_M  1 
#define QRENCODE_VERSION_L  2 

// Error codes
#define QRENCODE_SUCCESS                1
#define QRENCODE_FAIL                   0
#define QRENCODE_INVALID_HANDLE         -1
#define QRENCODE_INVALID_PARAM          -2
#define QRENCODE_NO_ENOUGH_MEMORY       -3
#define QRENCODE_UNSUPPORTED_QUALITY    -4
#define QRENCODE_UNSUPPORTED_VERSION    -5
#define QRENCODE_WIDTH_TOO_SMALL        -6
#define QRENCODE_NEED_ENCODE            -7

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    struct  qrencode_t;
    typedef qrencode_t* qrencode_ptr_t;

    qrencode_ptr_t
        QRENCODE_CAPI
        qrencode_new_encoder();

    int
        QRENCODE_CAPI
        qrencode_set_version(qrencode_ptr_t encoder, unsigned int quality);

    int
        QRENCODE_CAPI
        qrencode_set_color(qrencode_ptr_t encoder, unsigned int rgb);

    int
        QRENCODE_CAPI
        qrencode_set_background_color(qrencode_ptr_t encoder, unsigned int rgb);

    int
        QRENCODE_CAPI
        qrencode_set_quality(qrencode_ptr_t encoder, unsigned int quality);

    int
        QRENCODE_CAPI
        qrencode_encode(qrencode_ptr_t encoder, const char* content);

    int
        QRENCODE_CAPI
        qrencode_get_encoded_minimum_width(qrencode_ptr_t encoder, unsigned int * width);

    int
        QRENCODE_CAPI
        qrencode_get_encoded_image(qrencode_ptr_t encoder, unsigned int width, unsigned char** outBytes, unsigned int * outSize);

    void
        QRENCODE_CAPI
        qrencode_free_memory(void* memory);

    void
        QRENCODE_CAPI
        qrencode_free_encoder(qrencode_ptr_t encoder);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !__LIB_QR_ENCODE_H__

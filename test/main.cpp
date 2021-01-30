#include <qrencode.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

static char* hex_encode(const unsigned char bytes[], unsigned int size);

static inline char hex_mapping(unsigned char val)
{
    if (0 <= val && val <= 9)
    {
        return val + '0';
    }
    if (10 <= val && val <= 15)
    {
        return (val - 10) + 'a';
    }
    return ' ';
}

static inline void byte_to_hex(unsigned char byte, char* hex)
{
    hex[0] = hex_mapping((byte & 0xF0) >> 4);
    hex[1] = hex_mapping((byte & 0x0F) >> 0);
}

int main()
{
    int rv = QRENCODE_SUCCESS;
    unsigned char* data = 0;
    unsigned int   size = 0;
    qrencode_ptr_t encoder = qrencode_new_encoder();
    rv = qrencode_set_color(encoder, 0x000000);
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    rv = qrencode_set_background_color(encoder, 0xe2e2e2);
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    rv = qrencode_set_quality(encoder, QRENCODE_LEVEL_L);
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    rv = qrencode_set_version(encoder, QRENCODE_VERSION_S);
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    rv = qrencode_encode(encoder, "123");
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    unsigned int minWidth = 0;
    rv = qrencode_get_encoded_minimum_width(encoder, &minWidth);
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    rv = qrencode_get_encoded_image(encoder, minWidth * 20, &data, &size);
    if (QRENCODE_SUCCESS != rv)
    {
        return rv;
    }
    char* hex = hex_encode(data, size);
    printf("hex: %s\n", hex);
    delete[] hex;
    hex = NULL;
    qrencode_free_memory(data);
    qrencode_free_encoder(encoder);
    return 0;
}

char * hex_encode(const unsigned char bytes[], unsigned int size)
{
    char* hex = new char[size * 2 + 1]();
    for (int i = 0; i < size; i++)
    {
        byte_to_hex(bytes[i], &hex[i * 2]);
    }
    hex[size * 2] = '\0';
    return hex;
}

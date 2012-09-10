//
//  vpu.c
//  
//
//  Created by Tom on 20/04/2011.
//  Copyright 2011 Tom Butterworth. All rights reserved.
//

#include "vpu.h"
#include <stdlib.h>
#include <stdint.h>
#include <errno.h> // For lzf
#include <string.h> // For memcpy for uncompressed frames
#include <zlib.h>
#include "snappy-c.h"
#include "lzf.h"

/*
 VPU Constants
 First four bits represent the compressor
 Second four bits represent the format
 */
#define kVPUCompressorNone 0xA
#define kVPUCompressorSnappy 0xB
#define kVPUCompressorLZF 0xC
#define kVPUCompressorZLIB 0xD

#define kVPUFormatRGBDXT1 0xB
#define kVPUFormatRGBADXT1 0xC
#define kVPUFormatRGBADXT3 0xD
#define kVPUFormatRGBADXT5 0xE
#define kVPUFormatYCoCgDXT5 0xF

/*
 Packed byte values for VPU
 
 Format         Compressor      Byte Code
 ----------------------------------------
 RGB_DXT1       None            0xAB
 RGB_DXT1       Snappy          0xBB
 RGB_DXT1       LZF             0xCB
 RGB_DXT1       ZLIB            0xDB
 RGBA_DXT1      None            0xAC
 RGBA_DXT1      Snappy          0xBC
 RGBA_DXT1      LZF             0xCC
 RGBA_DXT1      ZLIB            0xDC
 RGBA_DXT3      None            0xAD
 RGBA_DXT3      Snappy          0xBD
 RGBA_DXT3      LZF             0xCD
 RGBA_DXT3      ZLIB            0xDD
 RGBA_DXT5      None            0xAE
 RGBA_DXT5      Snappy          0xBE
 RGBA_DXT5      LZF             0xCE
 RGBA_DXT5      ZLIB            0xDE
 YCoCg_DXT5     None            0xAF
 YCoCg_DXT5     Snappy          0xBF
 YCoCg_DXT5     LZF             0xCF
 YCoCg_DXT5     ZLIB            0xDF
 */


// TODO: rename the defines we use for codes used in stored frames
// to better differentiate them from the enums used for the API

// These read and write little-endian values on big or little-endian architectures
static unsigned int vpu_read_3_byte_uint(const void *buffer)
{
    return (*(uint8_t *)buffer) + ((*(uint8_t *)(buffer + 1)) << 8) + ((*(uint8_t *)(buffer + 2)) << 16);
}

static void vpu_write_3_byte_uint(void *buffer, unsigned int value)
{
    *(uint8_t *)buffer = value & 0xFF;
    *(uint8_t *)(buffer + 1) = (value >> 8) & 0xFF;
    *(uint8_t *)(buffer + 2) = (value >> 16) & 0xFF;
}

static unsigned int vpu_read_top_4_bits(const void *buffer)
{
    return ((*(unsigned char *)buffer) & 0xF0) >> 4;
}

static unsigned int vpu_read_bottom_4_bits(const void *buffer)
{
    return (*(unsigned char *)buffer) & 0x0F;
}

static void vpu_write_4_bit_values(const void *buffer, unsigned int top_bits, unsigned int bottom_bits)
{
    *(unsigned char *)buffer = (top_bits << 4) | (bottom_bits & 0x0F);
}


// Sets output_texture_format and output_compressor to VPUFormat and VPUCompressor constants
// buffer must be at least 4 bytes long, check before calling
static void vpu_read_frame_header(const void *buffer, size_t *out_compressed_length, unsigned int *out_texture_format, unsigned int *out_compressor)
{
    /*
     The first three bytes are the length of the compressed frame (not including the four byte header)
     */
    *out_compressed_length = vpu_read_3_byte_uint(buffer);
    
    /*
     The fourth byte stores the constant to describe texture format and compressor
     VPU compressor/format constants can be unpacked by reading the top and bottom four bits.
     */
        
    *out_compressor = vpu_read_top_4_bits(buffer + 3U);
    *out_texture_format = vpu_read_bottom_4_bits(buffer + 3U);
}

static void vpu_write_frame_header(void *buffer, size_t compressed_length, unsigned int texture_format, unsigned int compressor)
{
    /*
     The first three bytes are the length of the compressed frame (not including the four byte header)
     */
    vpu_write_3_byte_uint(buffer, (unsigned int)compressed_length);
    
    /*
     The fourth byte stores the constant to describe texture format and compressor
     */
    vpu_write_4_bit_values(buffer + 3, compressor, texture_format);
}

// Returns an API texture format constant or 0 if not recognised
static unsigned int vpu_texture_format_constant_for_format_identifier(unsigned int identifier)
{
    switch (identifier)
    {
        case kVPUFormatRGBDXT1:
            return VPUTextureFormat_RGB_DXT1;
        case kVPUFormatRGBADXT1:
            return VPUTextureFormat_RGBA_DXT1;
        case kVPUFormatRGBADXT3:
            return VPUTextureFormat_RGBA_DXT3;
        case kVPUFormatRGBADXT5:
            return VPUTextureFormat_RGBA_DXT5;
        case kVPUFormatYCoCgDXT5:
            return VPUTextureFormat_YCoCg_DXT5;
        default:
            return 0;
            
    }
}

// Returns a frame identifier or 0 if not recognised
static unsigned int vpu_texture_format_identifier_for_format_constant(unsigned int constant)
{
    switch (constant)
    {
        case VPUTextureFormat_RGB_DXT1:
            return kVPUFormatRGBDXT1;
        case VPUTextureFormat_RGBA_DXT1:
            return kVPUFormatRGBADXT1;
        case VPUTextureFormat_RGBA_DXT3:
            return kVPUFormatRGBADXT3;
        case VPUTextureFormat_RGBA_DXT5:
            return kVPUFormatRGBADXT5;
        case VPUTextureFormat_YCoCg_DXT5:
            return kVPUFormatYCoCgDXT5;
        default:
            return 0;
    }
}

unsigned long VPUMaxEncodedLength(unsigned long inputBytes)
{
    /*
     Actually our max encoded length is inputBytes + 4U but snappy may produce longer output
     and the only way we can find out is by trying with a suitably-sized buffer
     */
    unsigned long compressedLength = snappy_max_compressed_length(inputBytes);
    if (compressedLength < inputBytes) compressedLength = inputBytes;
    return compressedLength + 4U;
}

unsigned int VPUEncode(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int textureFormat,
                       unsigned int compressor, void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed)
{
    /*
     Check arguments
     */
    if (inputBuffer == NULL
        || inputBufferBytes == 0
        || (textureFormat != VPUTextureFormat_RGB_DXT1
            && textureFormat != VPUTextureFormat_RGBA_DXT1
            && textureFormat != VPUTextureFormat_RGBA_DXT3
            && textureFormat != VPUTextureFormat_RGBA_DXT5
            && textureFormat != VPUTextureFormat_YCoCg_DXT5
            )
        || (compressor != VPUCompressorSnappy
            && compressor != VPUCompressorLZF
            && compressor != VPUCompressorZLIB
            )
        )
    {
        return VPUResult_Bad_Arguments;
    }
    
    size_t maxCompressedLength = compressor == VPUCompressorSnappy ? snappy_max_compressed_length(inputBufferBytes) : inputBufferBytes;
    if (maxCompressedLength < inputBufferBytes)
    {
        // Sanity check in case a future Snappy promises to always compress
        maxCompressedLength = inputBufferBytes;
    }
    size_t maxOutputBufferLength = maxCompressedLength + 4U;
    if (outputBufferBytes < maxOutputBufferLength
        || outputBuffer == NULL)
    {
        return VPUResult_Buffer_Too_Small;
    }
    void *compressedStart = outputBuffer + 4U;
    size_t storedLength;
    unsigned int storedCompressor;
    if (compressor == VPUCompressorSnappy)
    {
        storedLength = outputBufferBytes;
        snappy_status result = snappy_compress(inputBuffer, inputBufferBytes, compressedStart, &storedLength);
        if (result != SNAPPY_OK)
        {
            return VPUResult_Internal_Error;
        }
        storedCompressor = kVPUCompressorSnappy;
    }
    else if (compressor == VPUCompressorLZF)
    {
        // TODO: we should probably just take unsigned int for size arguments
        storedLength = lzf_compress(inputBuffer, (unsigned int)inputBufferBytes, compressedStart, (unsigned int)outputBufferBytes - 4U);
        if (storedLength == 0 && errno != E2BIG)
        {
            return VPUResult_Internal_Error;
        }
        storedCompressor = kVPUCompressorLZF;
    }
    else
    {
        int z_result = Z_OK;
        
        z_stream z_stream;
        
        z_stream.zalloc = Z_NULL;
        z_stream.zfree = Z_NULL;
        z_stream.opaque = Z_NULL;
        
        z_result = deflateInit(&z_stream, Z_DEFAULT_COMPRESSION);
        if (z_result != Z_OK) return VPUResult_Internal_Error;
        
        z_stream.avail_in = inputBufferBytes;
        z_stream.next_in = (Bytef *)inputBuffer;
        z_stream.avail_out = outputBufferBytes - 4U;
        z_stream.next_out = compressedStart;
        
        z_result = deflate(&z_stream, Z_FINISH);
        
        deflateEnd(&z_stream);
        
        if (z_result == Z_STREAM_END)
        {
            // Compression succeeded
            storedLength = z_stream.total_out;
        }
        else if (z_result == Z_OK)
        {
            // We get Z_OK if the output buffer was too small
            storedLength = 0;
        }
        else
        {
            return VPUResult_Internal_Error;
        }
        
        storedCompressor = kVPUCompressorZLIB;
    }
    
    /*
     If our "compressed" frame is no smaller than our input frame then store the input uncompressed.
     lzf_compress will have returned 0 for storedLength if output was too big for the provided buffer.
     */
    if (storedLength == 0 || storedLength >= inputBufferBytes)
    {
        memcpy(compressedStart, inputBuffer, inputBufferBytes);
        storedLength = inputBufferBytes;
        storedCompressor = kVPUCompressorNone;
    }
    
    unsigned int storedFormat = vpu_texture_format_identifier_for_format_constant(textureFormat);
    
    vpu_write_frame_header(outputBuffer, storedLength, storedFormat, storedCompressor);
    
    if (outputBufferBytesUsed != NULL)
    {
        *outputBufferBytesUsed = storedLength + 4U;
    }
    
    return VPUResult_No_Error;
}

unsigned int VPUDecode(const void *inputBuffer, unsigned long inputBufferBytes,
                       void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed,
                       unsigned int *outputBufferTextureFormat)
{
    /*
     Check arguments
     */
    if (inputBuffer == NULL
        || inputBufferBytes < 4U
        || outputBufferTextureFormat == NULL
        )
    {
        return VPUResult_Bad_Arguments;
    }
    
    size_t storedLength;
    unsigned int textureFormat;
    unsigned int compressor;
    vpu_read_frame_header(inputBuffer, &storedLength, &textureFormat, &compressor);
    
    if (storedLength + 4U > inputBufferBytes)
    {
        return VPUResult_Bad_Frame;
    }
    
    /*
     Pass the texture format out
     */
    *outputBufferTextureFormat = vpu_texture_format_constant_for_format_identifier(textureFormat);
    if (*outputBufferTextureFormat == 0)
    {
        return VPUResult_Bad_Frame;
    }
    
    /*
     After the header comes the compressed frame, so now we decompress it into the output buffer
     */
    const void *storedStart = inputBuffer + 4U;
    unsigned long bytesUsed = 0;
    if (compressor == kVPUCompressorSnappy)
    {
        snappy_status result = snappy_uncompressed_length(storedStart, storedLength, &bytesUsed);
        if (result != SNAPPY_OK)
        {
            return VPUResult_Internal_Error;
        }
        if (bytesUsed > outputBufferBytes
            || outputBuffer == NULL)
        {
            return VPUResult_Buffer_Too_Small;
        }
        result = snappy_uncompress(storedStart, storedLength, outputBuffer, &bytesUsed);
        if (result != SNAPPY_OK)
        {
            return VPUResult_Internal_Error;
        }
    }
    else if (compressor == kVPUCompressorLZF)
    {
        // TODO: we should probably just take unsigned int for size arguments
        unsigned int decompressedLength = lzf_decompress(storedStart, (unsigned int)storedLength, outputBuffer, (unsigned int)outputBufferBytes);
        if (decompressedLength == 0)
        {
            if (errno == E2BIG)
            {
                return VPUResult_Buffer_Too_Small;
            }
            else
            {
                return VPUResult_Internal_Error;
            }
        }
        else
        {
            bytesUsed = decompressedLength;
        }
    }
    else if (compressor == kVPUCompressorZLIB)
    {
        int z_result = Z_OK;
        
        z_stream z_stream;
        
        z_stream.zalloc = Z_NULL;
        z_stream.zfree = Z_NULL;
        z_stream.opaque = Z_NULL;
        z_stream.avail_in = 0;
        z_stream.next_in = NULL;
        
        z_result = inflateInit(&z_stream);
        if (z_result != Z_OK) return VPUResult_Internal_Error;
        
        z_stream.avail_in = storedLength;
        z_stream.next_in = (Bytef *)storedStart;
        z_stream.avail_out = outputBufferBytes;
        z_stream.next_out = outputBuffer;
        
        z_result = inflate(&z_stream, Z_FINISH);
        
        inflateEnd(&z_stream);
        
        if (z_result == Z_STREAM_END) bytesUsed = z_stream.total_out;
        else if (z_result == Z_BUF_ERROR) return VPUResult_Buffer_Too_Small;
        else return VPUResult_Internal_Error;
    }
    else if (compressor == kVPUCompressorNone)
    {
        bytesUsed = storedLength;
        if (storedLength > outputBufferBytes
            || outputBuffer == NULL)
        {
            return VPUResult_Buffer_Too_Small;
        }
        memcpy(outputBuffer, storedStart, storedLength);
    }
    else
    {
        return VPUResult_Bad_Frame;
    }
    
    /*
     Fill out the remaining return values
     */
    if (outputBufferBytesUsed != NULL)
    {
        *outputBufferBytesUsed = bytesUsed;
    }
    
    return VPUResult_No_Error;
}

unsigned int VPUGetFrameTextureFormat(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int *outputBufferTextureFormat)
{
    /*
     Check arguments
     */
    if (inputBuffer == NULL
        || inputBufferBytes < 4U
        || outputBufferTextureFormat == NULL
        )
    {
        return VPUResult_Bad_Arguments;
    }
    /*
    Read the frame header
     */
    size_t compressedLength;
    unsigned int textureFormat;
    unsigned int compressor;
    vpu_read_frame_header(inputBuffer, &compressedLength, &textureFormat, &compressor);
    /*
     Pass the API enum value to match the constant out
     */
    *outputBufferTextureFormat = vpu_texture_format_constant_for_format_identifier(textureFormat);
    /*
     Check a valid format was present
     */
    if (*outputBufferTextureFormat == 0)
    {
        return VPUResult_Bad_Frame;
    }
    return VPUResult_No_Error;
}

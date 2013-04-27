/*
 hap.c
 
 Copyright (c) 2011-2013, Tom Butterworth and Vidvox LLC. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hap.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // For memcpy for uncompressed frames
#include "snappy-c.h"

#define kHapUInt24Max 0x00FFFFFF

/*
 Hap Constants
 First four bits represent the compressor
 Second four bits represent the format
 */
#define kHapCompressorNone 0xA
#define kHapCompressorSnappy 0xB

#define kHapFormatRGBDXT1 0xB
#define kHapFormatRGBADXT5 0xE
#define kHapFormatYCoCgDXT5 0xF

/*
 Packed byte values for Hap
 
 Format         Compressor      Byte Code
 ----------------------------------------
 RGB_DXT1       None            0xAB
 RGB_DXT1       Snappy          0xBB
 RGBA_DXT5      None            0xAE
 RGBA_DXT5      Snappy          0xBE
 YCoCg_DXT5     None            0xAF
 YCoCg_DXT5     Snappy          0xBF
 */


// TODO: rename the defines we use for codes used in stored frames
// to better differentiate them from the enums used for the API

// These read and write little-endian values on big or little-endian architectures
static unsigned int hap_read_3_byte_uint(const void *buffer)
{
    return (*(uint8_t *)buffer) + ((*(((uint8_t *)buffer) + 1)) << 8) + ((*(((uint8_t *)buffer) + 2)) << 16);
}

static void hap_write_3_byte_uint(void *buffer, unsigned int value)
{
    *(uint8_t *)buffer = value & 0xFF;
    *(((uint8_t *)buffer) + 1) = (value >> 8) & 0xFF;
    *(((uint8_t *)buffer) + 2) = (value >> 16) & 0xFF;
}

static unsigned int hap_read_4_byte_uint(const void *buffer)
{
    return (*(uint8_t *)buffer) + ((*(((uint8_t *)buffer) + 1)) << 8) + ((*(((uint8_t *)buffer) + 2)) << 16) + ((*(((uint8_t *)buffer) + 3)) << 24);
}

static void hap_write_4_byte_uint(const void *buffer, unsigned int value)
{
    *(uint8_t *)buffer = value & 0xFF;
    *(((uint8_t *)buffer) + 1) = (value >> 8) & 0xFF;
    *(((uint8_t *)buffer) + 2) = (value >> 16) & 0xFF;
    *(((uint8_t *)buffer) + 3) = (value >> 24) & 0xFF;
}

static unsigned int hap_read_top_4_bits(const void *buffer)
{
    return ((*(unsigned char *)buffer) & 0xF0) >> 4;
}

static unsigned int hap_read_bottom_4_bits(const void *buffer)
{
    return (*(unsigned char *)buffer) & 0x0F;
}

static void hap_write_4_bit_values(const void *buffer, unsigned int top_bits, unsigned int bottom_bits)
{
    *(unsigned char *)buffer = (top_bits << 4) | (bottom_bits & 0x0F);
}


// Sets output_texture_format and output_compressor to HapFormat and HapCompressor constants
// buffer must be at least 8 bytes long, check before calling
static void hap_read_frame_header(const void *buffer, size_t *out_header_length, size_t *out_compressed_length, unsigned int *out_texture_format, unsigned int *out_compressor)
{
    /*
     The first three bytes are the length of the compressed frame (not including the header) or zero
     if the length is stored in the last four bytes of an eight-byte header
     */
    *out_compressed_length = hap_read_3_byte_uint(buffer);
    
    /*
     If the first three bytes are zero, the size is in the following four bytes
     */
    if (*out_compressed_length == 0U)
    {
        *out_compressed_length = hap_read_4_byte_uint(((uint8_t *)buffer) + 4U);
        *out_header_length = 8U;
    }
    else
    {
        *out_header_length = 4U;
    }
    
    /*
     The fourth byte stores the constant to describe texture format and compressor
     Hap compressor/format constants can be unpacked by reading the top and bottom four bits.
     */
        
    *out_compressor = hap_read_top_4_bits(((uint8_t *)buffer) + 3U);
    *out_texture_format = hap_read_bottom_4_bits(((uint8_t *)buffer) + 3U);
}

static void hap_write_frame_header(void *buffer, size_t header_length, size_t compressed_length, unsigned int texture_format, unsigned int compressor)
{
    /*
     The first three bytes are the length of the compressed frame (not including the header) or zero
     if using an eight-byte header
     */
    if (header_length == 4U)
    {
        hap_write_3_byte_uint(buffer, (unsigned int)compressed_length);
    }
    else
    {
        /*
         For an eight-byte header, the length is in the last four bytes
         */
        hap_write_3_byte_uint(buffer, 0U);
        hap_write_4_byte_uint(((uint8_t *)buffer) + 4U, compressed_length);
    }
    
    /*
     The fourth byte stores the constant to describe texture format and compressor
     */
    hap_write_4_bit_values(((uint8_t *)buffer) + 3, compressor, texture_format);
}

// Returns an API texture format constant or 0 if not recognised
static unsigned int hap_texture_format_constant_for_format_identifier(unsigned int identifier)
{
    switch (identifier)
    {
        case kHapFormatRGBDXT1:
            return HapTextureFormat_RGB_DXT1;
        case kHapFormatRGBADXT5:
            return HapTextureFormat_RGBA_DXT5;
        case kHapFormatYCoCgDXT5:
            return HapTextureFormat_YCoCg_DXT5;
        default:
            return 0;
            
    }
}

// Returns a frame identifier or 0 if not recognised
static unsigned int hap_texture_format_identifier_for_format_constant(unsigned int constant)
{
    switch (constant)
    {
        case HapTextureFormat_RGB_DXT1:
            return kHapFormatRGBDXT1;
        case HapTextureFormat_RGBA_DXT5:
            return kHapFormatRGBADXT5;
        case HapTextureFormat_YCoCg_DXT5:
            return kHapFormatYCoCgDXT5;
        default:
            return 0;
    }
}

unsigned long HapMaxEncodedLength(unsigned long inputBytes)
{
    /*
     Actually our max encoded length is inputBytes + 8U but snappy may produce longer output
     and the only way we can find out is by trying with a suitably-sized buffer
     */
    unsigned long compressedLength = snappy_max_compressed_length(inputBytes);
    if (compressedLength < inputBytes) compressedLength = inputBytes;
    return compressedLength + 8U;
}

unsigned int HapEncode(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int textureFormat,
                       unsigned int compressor, void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed)
{
    size_t maxCompressedLength;
    size_t maxOutputBufferLength;
    size_t headerLength;
    void *compressedStart;
    size_t storedLength;
    unsigned int storedCompressor;
    unsigned int storedFormat;

    /*
     Check arguments
     */
    if (inputBuffer == NULL
        || inputBufferBytes == 0
        || (textureFormat != HapTextureFormat_RGB_DXT1
            && textureFormat != HapTextureFormat_RGBA_DXT5
            && textureFormat != HapTextureFormat_YCoCg_DXT5
            )
        || (compressor != HapCompressorNone
            && compressor != HapCompressorSnappy
            )
        )
    {
        return HapResult_Bad_Arguments;
    }
    
    maxCompressedLength = compressor == HapCompressorSnappy ? snappy_max_compressed_length(inputBufferBytes) : inputBufferBytes;
    if (maxCompressedLength < inputBufferBytes)
    {
        // Sanity check in case a future Snappy promises to always compress
        maxCompressedLength = inputBufferBytes;
    }
    
    /*
     To store frames of length greater than can be expressed in three bytes, we use an eight byte header (the last four bytes are the
     frame size). We don't know the compressed size until we have performed compression, but we know the worst-case size
     (the uncompressed size), so choose header-length based on that.
     
     A simpler encoder could always use the eight-byte header variation.
     */
    if (inputBufferBytes > kHapUInt24Max)
    {
        headerLength = 8U;
    }
    else
    {
        headerLength = 4U;
    }
    
    maxOutputBufferLength = maxCompressedLength + headerLength;
    if (outputBufferBytes < maxOutputBufferLength
        || outputBuffer == NULL)
    {
        return HapResult_Buffer_Too_Small;
    }
    compressedStart = ((uint8_t *)outputBuffer) + headerLength;
    
    if (compressor == HapCompressorSnappy)
    {
        snappy_status result;
        storedLength = outputBufferBytes;
        result = snappy_compress((const char *)inputBuffer, inputBufferBytes, (char *)compressedStart, &storedLength);
        if (result != SNAPPY_OK)
        {
            return HapResult_Internal_Error;
        }
        storedCompressor = kHapCompressorSnappy;
    }
    else
    {
        // HapCompressorNone
        // Setting storedLength to 0 causes the frame to be used uncompressed
        storedLength = 0;
    }
    
    /*
     If our "compressed" frame is no smaller than our input frame then store the input uncompressed.
     */
    if (storedLength == 0 || storedLength >= inputBufferBytes)
    {
        memcpy(compressedStart, inputBuffer, inputBufferBytes);
        storedLength = inputBufferBytes;
        storedCompressor = kHapCompressorNone;
    }
    
    storedFormat = hap_texture_format_identifier_for_format_constant(textureFormat);
    
    hap_write_frame_header(outputBuffer, headerLength, storedLength, storedFormat, storedCompressor);
    
    if (outputBufferBytesUsed != NULL)
    {
        *outputBufferBytesUsed = storedLength + headerLength;
    }
    
    return HapResult_No_Error;
}

unsigned int HapDecode(const void *inputBuffer, unsigned long inputBufferBytes,
                       void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed,
                       unsigned int *outputBufferTextureFormat)
{
    size_t headerLength;
    size_t storedLength;
    unsigned int textureFormat;
    unsigned int compressor;
    const void *storedStart;
    size_t bytesUsed = 0;
    
    /*
     Check arguments
     */
    if (inputBuffer == NULL
        || inputBufferBytes < 8U
        || outputBufferTextureFormat == NULL
        )
    {
        return HapResult_Bad_Arguments;
    }
    
    hap_read_frame_header(inputBuffer, &headerLength, &storedLength, &textureFormat, &compressor);
    
    if (storedLength + headerLength > inputBufferBytes)
    {
        return HapResult_Bad_Frame;
    }
    
    /*
     Pass the texture format out
     */
    *outputBufferTextureFormat = hap_texture_format_constant_for_format_identifier(textureFormat);
    if (*outputBufferTextureFormat == 0)
    {
        return HapResult_Bad_Frame;
    }
    
    /*
     After the header comes the compressed frame, so now we decompress it into the output buffer
     */
    storedStart = ((uint8_t *)inputBuffer) + headerLength;
    if (compressor == kHapCompressorSnappy)
    {
        snappy_status result = snappy_uncompressed_length((const char *)storedStart, storedLength, &bytesUsed);
        if (result != SNAPPY_OK)
        {
            return HapResult_Internal_Error;
        }
        if (bytesUsed > outputBufferBytes
            || outputBuffer == NULL)
        {
            return HapResult_Buffer_Too_Small;
        }
        result = snappy_uncompress((const char *)storedStart, storedLength, (char *)outputBuffer, &bytesUsed);
        if (result != SNAPPY_OK)
        {
            return HapResult_Internal_Error;
        }
    }
    else if (compressor == kHapCompressorNone)
    {
        bytesUsed = storedLength;
        if (storedLength > outputBufferBytes
            || outputBuffer == NULL)
        {
            return HapResult_Buffer_Too_Small;
        }
        memcpy(outputBuffer, storedStart, storedLength);
    }
    else
    {
        return HapResult_Bad_Frame;
    }
    
    /*
     Fill out the remaining return values
     */
    if (outputBufferBytesUsed != NULL)
    {
        *outputBufferBytesUsed = bytesUsed;
    }
    
    return HapResult_No_Error;
}

unsigned int HapGetFrameTextureFormat(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int *outputBufferTextureFormat)
{
    size_t headerLength;
    size_t compressedLength;
    unsigned int textureFormat;
    unsigned int compressor;
    /*
     Check arguments
     */
    if (inputBuffer == NULL
        || inputBufferBytes < 8U
        || outputBufferTextureFormat == NULL
        )
    {
        return HapResult_Bad_Arguments;
    }
    /*
    Read the frame header
     */
    hap_read_frame_header(inputBuffer, &headerLength, &compressedLength, &textureFormat, &compressor);
    /*
     Pass the API enum value to match the constant out
     */
    *outputBufferTextureFormat = hap_texture_format_constant_for_format_identifier(textureFormat);
    /*
     Check a valid format was present
     */
    if (*outputBufferTextureFormat == 0)
    {
        return HapResult_Bad_Frame;
    }
    return HapResult_No_Error;
}

//
//  hap.h
//  
//
//  Created by Tom on 20/04/2011.
//  Copyright 2011 Tom Butterworth. All rights reserved.
//

#ifndef hap_h
#define hap_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 These match the constants defined by GL_EXT_texture_compression_s3tc
 */

enum HapTextureFormat {
    HapTextureFormat_RGB_DXT1 = 0x83F0,
    HapTextureFormat_RGBA_DXT1 = 0x83F1,
    HapTextureFormat_RGBA_DXT3 = 0x83F2,
    HapTextureFormat_RGBA_DXT5 = 0x83F3,
    HapTextureFormat_YCoCg_DXT5 = 0x01
};

/*
 TODO: 
 We'll probably just pick a compressor and stick with it based on performance
 (CPU-time versus disk-space tradeoff) so these will go
 */
enum HapCompressor {
    HapCompressorNone,
    HapCompressorSnappy,
    HapCompressorLZF,
    HapCompressorZLIB
};

enum HapResult {
    HapResult_No_Error = 0,
    HapResult_Bad_Arguments,
    HapResult_Buffer_Too_Small,
    HapResult_Bad_Frame,
    HapResult_Internal_Error
};

/*
 Returns the maximum size of an output buffer for an input buffer of inputBytes length.
 */
unsigned long HapMaxEncodedLength(unsigned long inputBytes);

/*
 Encodes inputBuffer which is a buffer containing texture data of format textureFormat, or returns an error.
 Use HapMaxEncodedLength() to discover the minimal value for outputBufferBytes
 If this returns HapResult_No_Error and outputBufferBytesUsed is not NULL then outputBufferBytesUsed will be set
 to the actual encoded length of the output buffer.
 */
unsigned int HapEncode(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int textureFormat,
                       unsigned int compressor, void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed);

/*
 Decodes inputBuffer which is a Hap frame.
 If outputBufferBytesUsed is not NULL then it will be set to the decoded length of the output buffer.
 outputBufferTextureFormat must be non-NULL, and will be set to one of the HapTextureFormat constants.
 */
unsigned int HapDecode(const void *inputBuffer, unsigned long inputBufferBytes,
                       void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed,
                       unsigned int *outputBufferTextureFormat);

/*
 On return sets outputBufferTextureFormat to a HapTextureFormat constant describing the texture format of the frame.
 */
unsigned int HapGetFrameTextureFormat(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int *outputBufferTextureFormat);

#ifdef __cplusplus
}
#endif

#endif

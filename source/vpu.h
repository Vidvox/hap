//
//  vpu.h
//  
//
//  Created by Tom on 20/04/2011.
//  Copyright 2011 Tom Butterworth. All rights reserved.
//

#ifndef vpu_h
#define vpu_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 These match the constants defined by GL_EXT_texture_compression_s3tc
 */

enum VPUTextureFormat {
    VPUTextureFormat_RGB_DXT1 = 0x83F0,
    VPUTextureFormat_RGBA_DXT1 = 0x83F1,
    VPUTextureFormat_RGBA_DXT3 = 0x83F2,
    VPUTextureFormat_RGBA_DXT5 = 0x83F3,
    VPUTextureFormat_YCoCg_DXT5 = 0x01
};

/*
 TODO: 
 We'll probably just pick a compressor and stick with it based on performance
 (CPU-time versus disk-space tradeoff) so these will go
 */
enum VPUCompressor {
    VPUCompressorSnappy,
    VPUCompressorLZF,
    VPUCompressorZLIB
};

enum VPUResult {
    VPUResult_No_Error = 0,
    VPUResult_Bad_Arguments,
    VPUResult_Buffer_Too_Small,
    VPUResult_Bad_Frame,
    VPUResult_Internal_Error
};

/*
 Returns the maximum size of an output buffer for an input buffer of inputBytes length.
 */
unsigned long VPUMaxEncodedLength(unsigned long inputBytes);

/*
 Encodes inputBuffer which is a buffer containing texture data of format textureFormat, or returns an error.
 Use VPUMaxEncodedLength() to discover the minimal value for outputBufferBytes
 If this returns VPUResult_No_Error and outputBufferBytesUsed is not NULL then outputBufferBytesUsed will be set
 to the actual encoded length of the output buffer.
 */
unsigned int VPUEncode(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int textureFormat,
                       unsigned int compressor, void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed);

/*
 Decodes inputBuffer which is a VPU frame.
 If outputBufferBytesUsed is not NULL then it will be set to the decoded length of the output buffer.
 outputBufferTextureFormat must be non-NULL, and will be set to one of the VPUTextureFormat constants.
 */
unsigned int VPUDecode(const void *inputBuffer, unsigned long inputBufferBytes,
                       void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed,
                       unsigned int *outputBufferTextureFormat);

/*
 On return sets outputBufferTextureFormat to a VPUTextureFormat constant describing the texture format of the frame.
 */
unsigned int VPUGetFrameTextureFormat(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int *outputBufferTextureFormat);

#ifdef __cplusplus
}
#endif

#endif

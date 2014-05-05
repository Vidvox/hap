/*
 hap.h
 
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
    HapTextureFormat_RGBA_DXT5 = 0x83F3,
    HapTextureFormat_YCoCg_DXT5 = 0x01
};

enum HapCompressor {
    HapCompressorNone,
    HapCompressorSnappy
};

enum HapResult {
    HapResult_No_Error = 0,
    HapResult_Bad_Arguments,
    HapResult_Buffer_Too_Small,
    HapResult_Bad_Frame,
    HapResult_Internal_Error
};

/*
 See HapDecode for descriptions of these function types.
 */
typedef void (*HapDecodeWorkFunction)(void *p, unsigned int index);
typedef void (*HapDecodeCallback)(HapDecodeWorkFunction function, void *p, unsigned int count, void *info);

/*
 Returns the maximum size of an output buffer for an input buffer of inputBytes length.
 */
unsigned long HapMaxEncodedLength(unsigned long inputBytes);

/*
 Encodes inputBuffer which is a buffer containing texture data of format textureFormat, or returns an error.
 Use HapMaxEncodedLength() to discover the minimal value for outputBufferBytes.
 If this returns HapResult_No_Error and outputBufferBytesUsed is not NULL then outputBufferBytesUsed will be set
 to the actual encoded length of the output buffer.
 */
unsigned int HapEncode(const void *inputBuffer, unsigned long inputBufferBytes, unsigned int textureFormat,
                       unsigned int compressor, void *outputBuffer, unsigned long outputBufferBytes,
                       unsigned long *outputBufferBytesUsed);

/*
 Decodes inputBuffer which is a Hap frame.

 If the frame permits multithreaded decoding, callback will be called once for you to invoke a platform-appropriate
 mechanism to assign work to threads, and trigger that work by calling the function passed to your callback the number
 of times indicated by the count argument, usually from a number of different threads. This callback must not return
 until all the work has been completed.

 void MyHapDecodeCallback(HapDecodeWorkFunction function, void *p, unsigned int count, void *info)
 {
     int i;
     for (i = 0; i < count; i++) {
         // Invoke your multithreading mechanism to cause this function to be called
         // on a suitable number of threads.
         function(p, i);
     }
 }
 info is an argument for your own use to pass context to the callback.
 If the frame does not permit multithreaded decoding, callback will not be called.
 If outputBufferBytesUsed is not NULL then it will be set to the decoded length of the output buffer.
 outputBufferTextureFormat must be non-NULL, and will be set to one of the HapTextureFormat constants.
 */
unsigned int HapDecode(const void *inputBuffer, unsigned long inputBufferBytes,
                       HapDecodeCallback callback, void *info,
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

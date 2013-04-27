Hap Video
=========


Introduction
============

Hap is an open video codec. It stores frames in a format that can be decoded in part by dedicated graphics hardware on modern computer systems. The aim of Hap is to enable playback of a greater number of simultaneous streams of higher resolution video than is possible using alternative codecs.


Scope
=====

This document describes the content of Hap frames. It makes no recommendation on container format or the practical details of implementing an encoder or decoder.


External References
===================

The correct encoding and decoding of Hap frames depends on compression schemes defined outside of this document. Adherence to these schemes is required to produce conforming Hap frames.

1. S3 Texture Compression: described in the [OpenGL S3TC Extension][1]
2. Snappy Compression: described in the [Snappy Format Description][2]
3. Scaled YCoCg DXT5 Texture Compression: described in [Real-Time YCoCg-DXT Compression][3], JMP van Waveren and Ignacio Castaño, September 2007


Hap Frames
==========

A Hap frame is formed of a header of four or eight bytes immediately followed by a frame data section.

Frame Header
------------

The header will be four or eight bytes in size and records the size and format of the frame data. The size of the header is determined by the value of the first three bytes.

When the first three bytes of the header each have the value zero, the header is eight bytes in size. The fifth, sixth, seventh and eighth bytes are an unsigned integer stored in little-endian byte order. This is the size of the frame data section in bytes.

When any of the first three bytes of the header have a non-zero value, the header is four bytes in size. The first three bytes are an unsigned integer stored in little-endian byte order. This is the size of the frame data section in bytes.

The fourth byte of the header is an unsigned integer denoting the S3 and second-stage compression formats in which the data is stored. Its value and meaning will be one of the following:

|Hexadecimal Byte Value |S3 Format         |Second-Stage Compressor |
|-----------------------|------------------|------------------------|
|0xAB                   |RGB DXT1          |None                    |
|0xBB                   |RGB DXT1          |Snappy                  |
|0xAE                   |RGBA DXT5         |None                    |
|0xBE                   |RGBA DXT5         |Snappy                  |
|0xAF                   |Scaled YCoCg DXT5 |None                    |
|0xBF                   |Scaled YCoCg DXT5 |Snappy                  |

Frame Data
----------

The remainder of the frame is the frame data, starting immediately after the header. The data is to be treated as indicated by the header. If a second-stage compressor is indicated then the frame data is to be decompressed accordingly. The result of that decompression will be data in the indicated S3 format. If no second-stage compressor is indicated, the frame data is in the indicated S3 format.

[1]: http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt
[2]: http://snappy.googlecode.com/svn/trunk/format_description.txt
[3]: http://developer.download.nvidia.com/whitepapers/2007/Real-Time-YCoCg-DXT-Compression/Real-Time%20YCoCg-DXT%20Compression.pdf

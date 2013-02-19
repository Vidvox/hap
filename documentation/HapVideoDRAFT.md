Hap Video
=========


*DRAFT DOCUMENT: All details of the format described may change.*


Introduction
------------

Hap stores video frames encoded using S3 Texture Compression such that they meet the requirements of the OpenGL [texture_compression_s3tc][1] extension. Hap permits a second stage of compression. The only permitted second-stage compression method is [Snappy][2].

A frame consists of a header and data part. The header will be four bytes in length and records the size and format of the frame data. Frame data will immediately follow the frame header.


Frame Header
------------

The first three bytes of the header are an unsigned integer stored in little-endian byte order. This is the size of the frame data in bytes.

The fourth byte of the header is an unsigned integer denoting the S3 and second-stage compression formats in which the data is stored. Its value and meaning will be one of the following:

|Hexadecimal Byte Value |S3 Format |Second-Stage Compressor |
|-----------------------|----------|------------------------|
|0xAB                   |RGB DXT1  |None                    |
|0xBB                   |RGB DXT1  |Snappy                  |
|0xAC                   |RGBA DXT1 |None                    |
|0xBC                   |RGBA DXT1 |Snappy                  |
|0xAD                   |RGBA DXT3 |None                    |
|0xBD                   |RGBA DXT3 |Snappy                  |
|0xAE                   |RGBA DXT5 |None                    |
|0xBE                   |RGBA DXT5 |Snappy                  |

Frame Data
----------

The remainder of the frame is the frame data, starting immediately after the header (four bytes after the start of the frame). The data is to be treated as indicated by the header. If a second-stage compressor is indicated then the frame data is to be decompressed accordingly. The result of that decompression will be data in the indicated S3 format. If no second-stage compressor is indicated, the frame data is in the indicated S3 format.

[1]: http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt
[2]: http://code.google.com/p/snappy/

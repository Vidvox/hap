#Hap Video



##Introduction


Hap is an open video codec. It stores frames in a format that can be decoded in part by dedicated graphics hardware on modern computer systems. The aim of Hap is to enable playback of a greater number of simultaneous streams of higher resolution video than is possible using alternative codecs.


##Scope


This document describes the content of Hap frames. It makes no recommendation on container format or the practical details of implementing an encoder or decoder.


##External References


The correct encoding and decoding of Hap frames depends on compression schemes defined outside of this document. Adherence to these schemes is required to produce conforming Hap frames.

1. S3 Texture Compression: described in the [OpenGL S3TC Extension][1]
2. Snappy Compression: described in the [Snappy Format Description][2]
3. Scaled YCoCg DXT5 Texture Compression: described in [Real-Time YCoCg-DXT Compression][3], JMP van Waveren and Ignacio Castaño, September 2007
4. BC7 Texture Compression: described in the [OpenGL BPTC Extension][4]

##Hap Frames


A Hap frame is stored in a sectioned layout, where each section is preceded by a header indicating that section's size and type. A section may itself contain other sections, or data to inform decoding, or frame data. Section data immediately follows the section header.

Decoders encountering a section of an unknown type should attempt to continue decoding the frame if other sections provide adequate information to do so. Some sections are only permitted inside other sections, but at any given hierarchical level sections may occur in any order.

###Section Header

A section header will be four or eight bytes in size and records the type and size of the section. The recorded size of the section excludes the size of the header. The size of the header is determined by the value of the first three bytes.

When the first three bytes of the header each have the value zero, the header is eight bytes in size. The fifth, sixth, seventh and eighth bytes are an unsigned integer stored in little-endian byte order. This is the size of the section in bytes, excluding the size of the header.

When any of the first three bytes of the header have a non-zero value, the header is four bytes in size. The first three bytes are an unsigned integer stored in little-endian byte order. This is the size of the section in bytes, excluding the size of the header.

The fourth byte of the header is an unsigned integer denoting the type of that section.

###Top-Level Sections

The following are the only section types permitted at the top level of a frame. Only one top-level will be present per frame. The type of these sections indicates the Image Format(s) and second-stage compression formats in which the data is stored. In the case of a multi-image section, multiple sections containing each image's exact format/data will be present within the frame's top level section.

|Type Field Byte Value |Image Format      |Second-Stage Compressor      |
|----------------------|------------------|-----------------------------|
|0xAB                  |RGB DXT1          |None                         |
|0xBB                  |RGB DXT1          |Snappy                       |
|0xCB                  |RGB DXT1          |Consult decode instructions  |
|0xAE                  |RGBA DXT5         |None                         |
|0xBE                  |RGBA DXT5         |Snappy                       |
|0xCE                  |RGBA DXT5         |Consult decode instructions  |
|0xAF                  |Scaled YCoCg DXT5 |None                         |
|0xBF                  |Scaled YCoCg DXT5 |Snappy                       |
|0xCF                  |Scaled YCoCg DXT5 |Consult decode instructions  |
|0xAC                  |RGBA BC7          |None                         |
|0xBC                  |RGBA BC7          |Snappy                       |
|0xCC                  |RGBA BC7          |Consult decode instructions  |
|0xAA                  |8-bit Uncompressed Alpha |None                         |
|0xBA                  |8-bit Uncompressed Alpha |Snappy                       |
|0xCA                  |8-bit Uncompressed Alpha |Consult decode instructions  |
|0x0D                  |Multi-Image Section| Not Applicable |

####Simple Top-Level Sections

If the top-level section type indicates a single or no second-stage compressor, the section data is to be treated as indicated by the type. If a second-stage compressor is indicated then the section data is to be decompressed accordingly. The result of that decompression will be data in the indicated Image format. If no second-stage compressor is indicated, the section data is in the indicated Image format.

####Multi-Image Section
A multi-image top-level section will contain multiple image sections, each with a portion of the data required to create the full final image. For example a 'Scaled YCoCg DXT5 with separate Alpha' section will contain two image section, a Scaled YCoCg DXT5 section followed by an Alpha section. The RGB result from the Scaled YCoCg DXT5 section should be combined with the Alpha from the Alpha section to create a final RGBA image. Each image section follows the same rules as if they were a standalone top-level section.

#####Legal Multi-Image Section combinations
* Scaled YCoCg DXT5 + 8-bit Uncompressed Alpha

####Decode Instructions

If the top-level section type indicates decode instructions, the section data is a single section containing instructions for decoding, immediately followed by the frame data. The result of decoding the frame data using the given instructions will be data in the indicated S3 format.

|Type Field Byte Value |Meaning                       |
|----------------------|------------------------------|
|0x01                  |Decode Instructions Container |

#####Decode Instructions Container

The Decode Instructions Container may contain the following sections which dictate the steps to decode the frame data.

|Type Field Byte Value |Meaning                             |
|----------------------|------------------------------------|
|0x02                  |Chunk Second-Stage Compressor Table |
|0x03                  |Chunk Size Table                    | 
|0x04                  |Chunk Offset Table                  |

A Chunk Second-Stage Compressor Table must be accompanied by a Chunk Size Table. The Chunk Offset Table may be omitted.

The presence of any of these sections indicates that frame data is split into chunks, which are to be passed to their second-stage decompressor independently.

The number of chunks is indicated by the number of entries in these tables, which must be the same for each table.

In the absence of a Chunk Offset Table the offset from the start of the frame data to the start of each chunk is calculated by summing the sizes of the preceding chunks.

#####Chunk Second-Stage Compressor Table

The section data is a series of single-byte fields indicating the second-stage compressor for each chunk, with one of the following values:

|Hexadecimal Byte Value |Compressor   |
|-----------------------|-------------|
|0x0A                   |Uncompressed |
|0x0B                   |Snappy       |

#####Chunk Size Table

The section data is a series of four-byte fields being unsigned integers stored in little-endian byte order, and indicating the byte size of each chunk.

#####Chunk Offset Table

The section data is a series of four-byte fields being unsigned integers stored in little-endian byte order, indicating the offset in bytes of each chunk from the start of the frame data. 

[1]: http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt
[2]: http://snappy.googlecode.com/svn/trunk/format_description.txt
[3]: http://developer.download.nvidia.com/whitepapers/2007/Real-Time-YCoCg-DXT-Compression/Real-Time%20YCoCg-DXT%20Compression.pdf
[4]: http://www.opengl.org/registry/specs/ARB/texture_compression_bptc.txt

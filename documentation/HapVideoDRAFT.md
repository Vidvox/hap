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


##Hap Frames


A Hap frame is always formed of a header of four or eight bytes and a frame data section. Frames may also have further sections that provide extra information which may or may not be required to properly decode the frame. The presence of these extra sections and their type(s) will be indicated by the type field of the frame header.

###Frame Header

The header will be four or eight bytes in size and records the format and combined size of any decode instructions and the frame data. The size of the header is determined by the value of the first three bytes, or the last four bytes, depending on the vaues of the first three bytes.

When the first three bytes of the header each have the value zero, the header is eight bytes in size. The fifth, sixth, seventh and eighth bytes are an unsigned integer stored in little-endian byte order. This is the combined size of any sections and the frame data in bytes.

When any of the first three bytes of the header have a non-zero value, the header is four bytes in size. The first three bytes are an unsigned integer stored in little-endian byte order. This is the combined size of any sections and the frame data in bytes.

The fourth byte of the header is an unsigned integer called the type field, denoting the S3 and second-stage compression formats in which the data is stored, as well as indicating the presence of decode instructions. Its value and meaning will be one of the following:

|Hexadecimal Byte Value |S3 Format         |Second-Stage Compressor      |
|-----------------------|------------------|-----------------------------|
|0xAB                   |RGB DXT1          |None                         |
|0xBB                   |RGB DXT1          |Snappy                       |
|0xCB                   |RGB DXT1          |Consult decode instructions Section |
|0xAE                   |RGBA DXT5         |None                         |
|0xBE                   |RGBA DXT5         |Snappy                       |
|0xCE                   |RGBA DXT5         |Consult decode instructions Section |
|0xAF                   |Scaled YCoCg DXT5 |None                         |
|0xBF                   |Scaled YCoCg DXT5 |Snappy                       |
|0xCF                   |Scaled YCoCg DXT5 |Consult decode instructions Section |

### Sections 

A section starts with the Section Header, and is followed by the section data which may be other sub-sections, or the data as described for that particular section.

#### Section Header

A section header follows the same format as the Frame Header.

The header will be four or eight bytes in size and records the format and combined size of the section. The size of the header is determined by the value of the first three bytes, or the last four bytes, depending on the vaues of the first three bytes.

When the first three bytes of the header each have the value zero, the header is eight bytes in size. The fifth, sixth, seventh and eighth bytes are an unsigned integer stored in little-endian byte order. This is the size of the section in bytes.

When any of the first three bytes of the header have a non-zero value, the header is four bytes in size. The first three bytes are an unsigned integer stored in little-endian byte order. This is the size of the section in bytes.

The fourth byte is a code denoting the type of that section, and will be one of the following:

|Hexadecimal Byte Value | Meaning                       |
|-----------------------|-------------------------------|
| 0x01	                | Decode Instructions Container |
| 0x02	                | Chunk Second-Stage Compressor Table |
| 0x03	                | Chunk Size Table  | 
| 0x04                  | Chunk Offset Table |

#### Section Types

##### Decode Instructions Container

The Decode Instructions Container is the only permitted top-level section. Following the size and type fields it will contain any other required sections. The size indicates the total size of all the decode instruction sub-sections.

##### Chunk Second-Stage Compressor Table

The presence of this section indicates that frame data is split into chunks. Following the size and type fields comes a series of single-byte fields indicating the second-stage compressor for each chunk, with one of the following values:

| Hexadecimal Byte Value | Compressor     |
|-------------------|---------------------|
| 0x0A               | Uncompressed        |
| 0x0B               | Snappy              |

The number of chunks can be calculated from the size of this section, that is to say, the size in bytes of this section is equal to the number of chunks. If second-stage compression is indicated, each chunk is to be passed to the second-stage decompressor independently. This section, if present, must be accompanied by a Chunk Size Table.

##### Chunk Size Table

The presence of this section indicates that frame data is split into chunks. Following the size and type fields comes a series of four-byte fields being unsigned integers stored in little-endian byte order, and indicating the byte size of each chunk. The number of chunks is equal to the size of his section divided by four. If second-stage compression is used, each chunk should be passed to the second-stage decompressor independently. This section, if present, must be accompanied by a Chunk Second-Stage Compressor Table. The offset from the start of the frame data to the beginning of each chunk can be calcualated by summing the chunk size of all previous chunks.

##### Chunk Offset Table

The presence of this section indicates that each chunk has a custom offset. These offsets must be used to find the location of the data for each chunk, it can not be inferred from simply the Size Table. Following the size and type fields comes a series of four-byte fields indicating the offset in bytes of each chunk from the start of the frame data. Chunks can use the same offset. Both the Chunk Size Table and the Chunk Second-Stage Compressor Table sections must be present for this section to be present.

### Frame Data

The remainder of the frame is the frame data, starting immediately after the frame header and optional sections. The data is to be treated as indicated by the header and sections. If a second-stage compressor is indicated then the frame data is to be decompressed accordingly. The result of that decompression will be data in the indicated S3 format. If no second-stage compressor is indicated, the frame data is in the indicated S3 format.

[1]: http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt
[2]: http://snappy.googlecode.com/svn/trunk/format_description.txt
[3]: http://developer.download.nvidia.com/whitepapers/2007/Real-Time-YCoCg-DXT-Compression/Real-Time%20YCoCg-DXT%20Compression.pdf

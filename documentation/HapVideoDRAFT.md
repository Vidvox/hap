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


A Hap frame is formed of a header of at least four bytes (but possibly more) immediately followed by a frame data section.

###Frame Header

All integers are stored in little-endian byte order.

The header starts with a 'Simple Header' section, which will be either four or eight bytes in size, depending on the values in the first three bytes. The Simple Header records the format of the frame data, as well as the size of the frame data and the Extended Header, if present.

When the first three bytes of the Simple Header each have the value zero, the Simple Header is eight bytes in size. The fifth, sixth, seventh and eighth bytes are an unsigned integer. This is the size of the frame and Extended Header sections, in bytes.

When any of the first three bytes of the SImple Header have a non-zero value, the Simple Header is four bytes in size. The first three bytes are an unsigned integer. This is the size of the frame data and Extended Header sections, in bytes.

The fourth byte of the Simple Header is an unsigned integer denoting the S3 and second-stage compression formats in which the data is stored. Its value and meaning will be one of the following:

|Hexadecimal Byte Value |S3 Format         |Second-Stage Compressor      |
|-----------------------|------------------|-----------------------------|
|0xAB                   |RGB DXT1          |None                         |
|0xBB                   |RGB DXT1          |Snappy                       |
|0xCB                   |RGB DXT1          |Specified by Extended Header |
|0xAE                   |RGBA DXT5         |None                         |
|0xBE                   |RGBA DXT5         |Snappy                       |
|0xCE                   |RGBA DXT5         |Specified by Extended Header |
|0xAF                   |Scaled YCoCg DXT5 |None                         |
|0xBF                   |Scaled YCoCg DXT5 |Snappy                       |
|0xCF                   |Scaled YCoCg DXT5 |Specified by Extended Header |



#### Extender Header

Must only be present if the Second-Stage Compressor says it is present. It will immediately follow the Simple Header.
The data in the Extended Header is as follows:
The first 4 bytes are an unsigned integer denoting the total size of the Extended Header, including any block(s) of extra information required. The actual frame data must start this number of bytes after the Extended Header's starting byte.
The next 4 bytes are a bitfield of potential options that describe how the frame data is stored. An option may or may not have an extra block of data required to further describe the stored frame data. If such a block is required it will always start with a 4 byte unsigned integer denoting the size of that block. If multiple blocks are present due to multiple options, the order that these blocks appear will be determined by the order that the options are listed in the below table. At least one option must be specified. Options can potentially be mutually exclusive.

|Hexadecimal bit Value |Option                      |
|----------------------|----------------------------|
|0x1                   |Data is partitioned         |

#####Data is partitioned (0x1):

When this option is enabled the frame data has been split up into partitions, each of which will have it's own compression format (or possibly remain uncompressed). Each partition should be decompressed using a seperate call to it's particular decompressor. The block describing the partioning is as laid out as follows:
The first 4 bytes are an unsigned integer denoting the total size of the block in bytes.
The next 4 bytes are an unsigned integer denoting the number of partitions. 
Following those first 8 bytes, there is a table with one entry for each patition. Each entry will be at least 12 bytes long, and possibly longer depending on the value of the first 4 bytes of the entry. 
The first 4 bytes of the entry are an unsigned integer denoting the compression format of the partition. Possible values are:

| Value             | Compressor          |
|-------------------|---------------------|
| 0x1               | Uncompressed        |
| 0x2               | Snappy              |

The next 4 bytes are an unsigned integer denoting the total size of the entry. This must be at least 12, and can be larger depending on the compressor. Currently none of the compressors will use more than 12 bytes per entry. 
The final 4 guaranteed bytes of the entry are the compressed size of the partition.

Each partition's entry will come directly after the previous partition's entry.

###Frame Data


The remainder of the frame is the frame data, starting immediately after the header(s). The data is to be treated as indicated by the header(s). If a second-stage compressor is indicated then the frame data is to be decompressed accordingly. The result of that decompression will be data in the indicated S3 format. If no second-stage compressor is indicated, the frame data is in the indicated S3 format.

[1]: http://www.opengl.org/registry/specs/EXT/texture_compression_s3tc.txt
[2]: http://snappy.googlecode.com/svn/trunk/format_description.txt
[3]: http://developer.download.nvidia.com/whitepapers/2007/Real-Time-YCoCg-DXT-Compression/Real-Time%20YCoCg-DXT%20Compression.pdf

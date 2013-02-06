Hap
====

Hap is a video codec that performs decompression on a computer's video card, substantially reducing the CPU usage necessary to play back a movie- this is useful in situations where CPU power is a limiting factor, such as when working with high resolution movies.

When supported natively by a host application, Hap has a number of distinct advantages over other codecs commonly used for real-time video playback.
- Very low CPU usage, even at high resolutions
- Support for an optional alpha channel
- Options for adjusting compression quality

There are three different Hap codecs: <b>Hap</b>, <b>Hap Alpha</b>, and <b>Hap Q</b>.
<br>
• <b>Hap</b> and <b>Hap Alpha</b> include an additional compression option for adjusting the image quality level.
<br>
• <b>Hap Alpha</b> supports an Alpha channel.
<br>
• <b>Hap Q</b> has noticeably more accurate image quality (at the expense of larger file sizes).
<br>


To play and export movies in the Hap format on your Mac, download and install the Quicktime component:
< xxxx link to installer for Quicktime Component xxxx >



<b>Important Note:</b>
Host applications that don't natively support Hap can still open and play back Hap movies, but CPU use will be much higher and this is not recommended (Hap exists to lower CPU use- there's no point in using it if the host application doesn't support Hap).  If you aren't sure if the program you are using supports Hap, it is a good idea to ask before transcoding your clips.

The Hap codec is open-source and completely free to use- taking full advantage of Hap decompression in a QuickTime-based application requires minimal additions, so if you'd like to see it included in your favorite video packages please be sure to link their developers to this page, which contains sample code:
https://github.com/bangnoise/hap



The Pros and Cons of supporting Hap (for other Developers)
====

The Hap codec is optimized to reduce CPU usage during playback of high-resolution movie files.  This is accomplished by using OpenGL's DXT/S3TC to decompress frames as OpenGL textures, thereby skipping QuickTime's frequently CPU-intensive decompression process.

There are several implications to this design choice:

• Hap is most appropriate for applications that use OpenGL-based technologies to display or manipulate frames as GL textures.  If you don't work with frames as GL textures, you may not want to support Hap.
<br>
• Hap movies can be quite large, which means they require a fast drive to play back- fortunately this is becoming more common, but if your users generally have slow drives or require low bitrates, Hap may not be an appropriate choice.
<br>
• DXT/S3TC were originally written to speed up texture loading in OpenGL applications: this is not a lossless technology, and image quality may be inferior to more mainstream, CPU-based codecs.  "Hap Q" is substantially higher-quality, but this comes at the expense of larger file sizes.
<br>
• DXT/S3TC produces GL_TEXTURE_2D textures, as opposed to the more generally useful GL_TEXTURE_RECTANGLE_EXT.  This may require additional support in your software.



How does Hap work?
====

Video codecs compress/decompress video data- they translate the files on your disk into pixels. Normally, the CPU in the computer does this work- GPU accelerated codecs perform this task on the computer's GPU (video card). This is done by using S3 texture compression (also known as DXT which is part of OpenGL) to encode frames, which allows still-compressed frames to be uploaded to your computer's video card for decompression. Since the video card is doing most of the work, this results in lower CPU usage than traditional codecs.

The Hap codec comes in three different variations that each correspond to a different form of DXT texture compression: 
<b>Hap</b> (DXT1), <b>Hap Alpha</b> (DXT3) and <b>Hap Q</b> (DXT5).

When using Hap or Hap Alpha, a secondary lossy compression of DXT frames is made on textures using the 'squish' algorithm to further reduce file size at the expense of image quality.

Finally, since all images encoded with DXT texture compression are still extremely large, Hap uses an additional lightweight lossless compression pass with Snappy to reduce the overall data rate to a manageable size.



Sample Code For Developers
====

This github repository hosts the specification of the compression format, along with the actual source code for encoding and decoding textures to Hap.

Developers simply interested in using Hap for GPU accelerated movie decompression in their own applications can download a sample project from this github page:
https://github.com/bangnoise/hap-quicktime-playback-demo


Developers looking for the Quicktime component used for encoding / decoding Hap files can find the code for that here:
https://github.com/bangnoise/hap-qt-codec



Open-Source
====

The Hap codec project is Open-Source with the FreeBSD license which means that you can use it in your commercial or noncommercial applications completely free of charge.

We do like to keep track of software that supports Hap so if you are using it for a project please send us an email.

You can read more about the FreeBSD license here:
http://www.freebsd.org/copyright/freebsd-license.html


This project was originally written by Tom 'Bangnoise' Butterworth and commissioned by VIDVOX, 2012











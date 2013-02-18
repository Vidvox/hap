Hap
====

Hap is a video codec that performs decompression using a computer's graphics hardware, substantially reducing the CPU usage necessary to play video - this is useful in situations where CPU power is a limiting factor, such as when working with high resolution video.

When supported natively by a host application, Hap has a number of distinct advantages over other codecs commonly used for real-time video playback.
- Very low CPU usage, even at high resolutions
- Support for an optional alpha channel
- Reduced data through-put to graphics hardware

There are three different Hap codecs: <b>Hap</b>, <b>Hap Alpha</b>, and <b>Hap Q</b>.
- <b>Hap</b> has the lowest data-rate and reasonable image quality.
- <b>Hap Alpha</b> has similar image quality to Hap, and supports an Alpha channel.
- <b>Hap Q</b> has improved image quality, at the expense of larger file sizes.

A <a href="http://github.com/bangnoise/hap-qt-codec/">Hap QuickTime component</a> allows encoding and non-accelarated playback of Hap video in any application on Mac OS X.

Fast playback of Hap requires native support in a host application.


Developers: Supporting Hap In Your Applications
====

The Hap codec is intended to reduce CPU usage during playback of high-resolution movie files. This is accomplished by storing frames using S3 Texture Compression (S3TC, also known as DXT). Compressed frames can be passed directly to graphics hardware for decompression using OpenGL or DirectX.

There are several factors to consider before deciding to adopt Hap:

- Hap is most appropriate for applications that already use OpenGL or DirectX technologies to display or manipulate frames.
- Hap movies can be quite large, requiring a fast drive for playback - if your users expect to be able to use slow drives or require low bitrates, Hap may not be an appropriate choice.
- S3 Texture Compression is not a lossless technology, and image quality can be inferior to other, CPU-based, codecs. Hap Q is substantially higher-quality, but this comes at the expense of larger file sizes.

<br>
The simplest way to add Hap support to your application is to use QuickTime with the Hap QuickTime component installed, making a custom request to receive S3TC frames. Discussion and sample code is available at:
<br>https://github.com/bangnoise/hap-quicktime-playback-demo


How does Hap work?
====

Video codecs compress/decompress video data - they translate the files on your disk into pixels. Normally, your computer's processors do this work - GPU-accelerated codecs perform this task on the computer's graphics hardware. With Hap, this is done by using S3 Texture Compression to encode frames, which allows still-compressed frames to be uploaded to your computer's video card for decompression. Since the graphics hardware is doing most of the work, this results in lower CPU usage than traditional codecs, and because the graphics hardware is designed to do this sort of task very quickly, it still remains available for other image processing you may wish to apply to the decompressed frames.

The Hap codec comes in three different variations, each corresponding to a different form of S3TC texture compression: 
<b>Hap</b> (DXT1), <b>Hap Alpha</b> (DXT5) and <b>Hap Q</b> (Scaled YCoCg  DXT5).

Since images encoded with S3TC are still extremely large, Hap uses an additional lightweight lossless compression pass using <a href="http://code.google.com/p/snappy/">Snappy</a> to reduce the overall data-rate to a manageable size.


Open-Source
====

The Hap codec project is open-source, licensed under a New BSD license, meaning you can use it in your commercial or noncommercial applications completely free of charge.

You can read more about the New BSD license here:
<br>
http://opensource.org/licenses/BSD-3-Clause

We like to know about software that supports Hap, so if you are using it for a project, please get in touch.

This project was originally written by Tom Butterworth and commissioned by <a href="http://www.vidvox.net">VIDVOX</a>, 2012.

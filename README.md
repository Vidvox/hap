Hap
====

Hap is a video codec that performs decompression using a computer's graphics hardware, substantially reducing the CPU usage necessary to play video — this is useful in situations where CPU power is a limiting factor, such as when working with multiple high resolution videos in real-time.

When supported natively by a host application, Hap has a number of distinct advantages over other codecs commonly used for real-time video playback.

- Very low CPU usage, even at high resolutions
- Support for an optional alpha channel
- Reduced data through-put to graphics hardware

There are four different Hap codecs: **Hap**, **Hap Alpha**, **Hap Q** and **Hap Q Alpha**.

- **Hap** has the lowest data-rate and reasonable image quality.
- **Hap Alpha** has similar image quality to Hap, and supports an Alpha channel.
- **Hap Q** has improved image quality, at the expense of larger file sizes.
- **Hap Q Alpha** has improved image quality and an Alpha channel, at the expense of larger file sizes.

Hap has some characteristics which you should consider before using it:

- Hap files are usually large and require a fast hard drive or solid state drive for playback.
- Image quality can be inferior to other, CPU-based, codecs. Hap Q gives substantially higher quality, but this comes at the expense of even larger file sizes.

Why Did We Make Hap?
---

This new codec is designed with the goal of playing back as many movies as possible on hardware with fast hard drives, particularly in situations where existing codecs reach the limits of the CPU to decode frames.

In particular this addresses three recent trends we've noticed in Macs currently being used for real-time visuals:

1. Upgrading to a SSD is a great way to get more use out of an older, slower machine, where the CPU is often difficult or flat out impossible to swap out.

2. Many of the newer high-end Macs (which are already recommended for their better graphics cards) come standard with SSD drives now, plus the SSDs options are widely more affordable than they were just 3 or 4 years ago.

3. For very high-end situations, it's usually possible to build a faster / bigger hard drive array, whereas you can't just stick more processors into a computer to play more movies at once.

How Does Hap Work?
----

Video codecs compress/decompress video data — they translate the files on your disk into pixels. Normally, your computer's processors do this work — GPU-accelerated codecs perform this task on the computer's graphics hardware. With Hap, this is done by using S3 Texture Compression to encode frames, which allows still-compressed frames to be passed directly to your computer's graphics hardware for decompression. Since the graphics hardware is designed to do this sort of task very quickly it still remains available for other image processing you may wish to apply to the decompressed frames, and the load on the CPU is minimal.

The Hap codec comes in three different variations, each corresponding to a different form of S3TC texture compression: 
**Hap** (DXT1), **Hap Alpha** (DXT5) and **Hap Q** (Scaled YCoCg DXT5).

Since images encoded with S3TC are still extremely large, Hap uses an additional lightweight lossless compression pass using [Snappy](http://code.google.com/p/snappy/) to reduce the overall data-rate to a manageable size.

Apps With Support For Hap
----

The following applications, environments and hardware have support for GPU-accelerated playback of Hap movies:

- [Blendy VJ](http://www.blendyvj.com/)
- [CoGe](http://cogevj.hu)
- [d3 Media Servers](http://www.d3technologies.com/)
- [Dataton Watchout](http://www.dataton.com/watchout)
- [Fugio](http://www.bigfug.com/software/fugio/)
- [GrandVJ](http://vj-dj.arkaos.net/grandvj/about)
- [GRoK](http://techlife.sg/GRoK/)
- [Isadora](http://troikatronix.com)
- [MadMapper](http://www.madmapper.com/)
- [Millumin](http://www.millumin.com)
- [Mitti](http://imimot.com/mitti/)
- [MixEmergency](http://www.inklen.com/mixemergency/)
- [modul8](http://www.garagecube.com/)
- [Modulo Player](http://modulo-pi.com/en/)
- [Painting With Light](http://www.bigfug.com/software/painting-with-light/)
- [Smode](http://smode.fr)
- [TouchDesigner](http://www.derivative.ca)
- [VDMX](http://www.vidvox.net)
- [Ventuz](http://www.ventuz.com)
- [VPT 7](http://hcgilje.wordpress.com/vpt/)
- Cinder via [Cinder-Hap](http://github.com/rsodre/Cinder-Hap)
- Max via [Hap Video Engine](https://cycling74.com/forums/topic/announcing-hap-video-engine/) or [jit.gl.hap](http://cycling74.com/toolbox/jit-gl-hap/)
- OpenFrameworks via [ofxHapPlayer](http://github.com/bangnoise/ofxHapPlayer)
- Quartz Composer via [CoGeHapPlayer](https://github.com/lov/CoGeHapPlayer)
- Unity via [AVPro Video](https://www.assetstore.unity3d.com/en/#!/content/56355) and [Demolition Media Hap](https://www.assetstore.unity3d.com/en/#!/content/78908)
- [vvvv](https://vvvv.org/) via [Demolition Media Hap](https://vvvv.org/contribution/demolition-media-hap-player)
- [Vuo](http://vuo.org/)


The following applications can play (and/or encode) Hap movies with varying or no GPU-accelaration:

- [AVF Batch Converter](https://github.com/Vidvox/hap-in-avfoundation/releases)
- [FFMPEG](https://ffmpeg.org)
- [MPlayer](http://www.mplayerhq.hu)
- [SMPlayer](http://smplayer.sourceforge.net/)

Developers: Supporting Hap In Your Applications
----

To add Hap support in your application, there are a number possible routes:

- On OS X, the [Hap AVFoundation framework](http://github.com/Vidvox/hap-in-avfoundation) enables encoding and decoding of Hap video on Mac OS X through AVFoundation.
- For 32-bit applications on OS X and Windows, the [Hap QuickTime codec](http://github.com/vidvox/hap-qt-codec/) allows encoding and non-accelerated playback of Hap video in any application, and assists accelerated playback in applications which support it. Discussion and sample code is available in the [Hap QuickTime Playback Demo](https://github.com/vidvox/hap-quicktime-playback-demo).
- A [DirectShow codec](http://www.renderheads.com/portfolio/HapDirectShow/) is also available.
- Use any demuxer (eg libavformat from FFMPEG) to parse encoded frames from a file and then use the source code from this project to decode the Hap frames to their compressed-texture formats to pass to OpenGL or DirectX.

If you need to parse raw Hap frames yourself, source code and a specification document are available as part of this project.

Open-Source
----

The Hap codec project is open-source, licensed under a [Free BSD License](https://github.com/vidvox/hap/blob/master/LICENSE), meaning you can use it in your commercial or noncommercial applications free of charge.

We like to know about software that supports Hap, so if you are using it for a project, please get in touch.

This project was originally written by [Tom Butterworth](http://kriss.cx/tom/) and commissioned by [VIDVOX](http://www.vidvox.net), 2012.

For [more information and performance stats read our post](http://vdmx.vidvox.net/blog/hap) on the VIDVOX blog.

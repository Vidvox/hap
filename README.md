Hap
====

Hap is a video codec that performs decompression using a computer's graphics hardware, substantially reducing the CPU usage necessary to play video — this is useful in situations where CPU power is a limiting factor, such as when working with multiple high resolution videos in real-time.

When supported natively by a host application, Hap has a number of distinct advantages over other codecs commonly used for real-time video playback.

- Very low CPU usage, even at high resolutions
- Support for an optional alpha channel
- Reduced data through-put to graphics hardware

There are three different Hap codecs: **Hap**, **Hap Alpha**, and **Hap Q**.

- **Hap** has the lowest data-rate and reasonable image quality.
- **Hap Alpha** has similar image quality to Hap, and supports an Alpha channel.
- **Hap Q** has improved image quality, at the expense of larger file sizes.


To add Hap support in your application, there are currently three possible routes:

- A [Hap AVFoundation framework](http://github.com/Vidvox/hap-in-avfoundation) demonstrates encoding and decoding of Hap video on Mac OS X through AVFoundation.
- A [Hap QuickTime codec](http://github.com/vidvox/hap-qt-codec/) allows encoding and non-accelerated playback of Hap video in any application on Mac OS X and Windows, and assists accelerated playback in applications which support it.
- A [DirectShow codec](http://www.renderheads.com/portfolio/HapDirectShow/) is also available.

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

The following applications and environments have support for GPU-accelerated playback of Hap movies:

- [VDMX](http://www.vidvox.net)
- [CoGe](http://cogevj.hu)
- [Millumin](http://www.millumin.com)
- [Blendy VJ](http://www.blendyvj.com/)
- [TouchDesigner088](http://www.derivative.ca)
- [d3 Media Servers](http://www.d3technologies.com/)
- [modul8](http://www.garagecube.com/)
- [MadMapper](http://www.madmapper.com/)
- [Isadora](http://troikatronix.com)
- [MixEmergency](http://www.inklen.com/mixemergency/)
- [VPT 7](http://hcgilje.wordpress.com/vpt/)
- [GRoK](http://techlife.sg/GRoK/)
- Cinder via [Cinder-Hap](http://github.com/rsodre/Cinder-Hap)
- Max via [jit.gl.hap](http://cycling74.com/toolbox/jit-gl-hap/)
- OpenFrameworks via [ofxHapPlayer](http://github.com/bangnoise/ofxHapPlayer)
- Unity via [AV PRO QUICKTIME](http://www.renderheads.com/portfolio/UnityAVProQuickTime/)
- Quartz Composer via [CoGeHapPlayer](https://github.com/lov/CoGeHapPlayer)

Developers: Supporting Hap In Your Applications
----

The simplest way to add Hap support to your application is to use QuickTime with the Hap QuickTime codec installed, making a custom request to receive S3TC frames which you then process with OpenGL or DirectX. Discussion and sample code is available in the [Hap QuickTime Playback Demo](https://github.com/vidvox/hap-quicktime-playback-demo).

If you need to parse raw Hap frames yourself, source code and a specification document are available as part of this project.

Open-Source
----

The Hap codec project is open-source, licensed under a [Free BSD License](https://github.com/vidvox/hap/blob/master/LICENSE), meaning you can use it in your commercial or noncommercial applications free of charge.

We like to know about software that supports Hap, so if you are using it for a project, please get in touch.

This project was originally written by [Tom Butterworth](http://kriss.cx/tom/) and commissioned by [VIDVOX](http://www.vidvox.net), 2012.

For [more information and performance stats read our post](http://vdmx.vidvox.net/blog/hap) on the VIDVOX blog.

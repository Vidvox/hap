HAP
====

HAP is a family of video codecs which perform decompression using a computer's graphics hardware, substantially reducing the CPU usage necessary to play video — this is useful in situations where CPU power is a limiting factor, such as when working with multiple high resolution videos in real-time.


Using HAP
----

Please visit the [HAP site](http://hap.video) for links to encoders and more information about HAP as well as resources for developers.


Specification and Reference Source
----

This repository hosts the [HAP specification](https://github.com/Vidvox/hap/tree/master/documentation) and [reference source code](https://github.com/Vidvox/hap/tree/master/source) for encoding and decoding HAP frames. It will be of interest to those implementing their own decoder or encoder.


Test Materials
----

Developers of decoders may find the following packs of material useful for testing:

 * [Odd Dimensions](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_Odd_Dimensions.zip): Samples with irregular dimensions (2 MB zip)
 * [16K](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_16K.zip): Samples at high resolution (35 MB zip)
 * [FFmpeg](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_FFmpeg.zip): Samples encoded using FFmpeg (24 MB zip)
 * [TouchDesigner](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_Derivative_TouchDesigner.zip): Samples encoded using Derivative TouchDesigner (1.5 GB zip)
 * [AVF Batch Converter](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_Vidvox_AVF_Batch_Converter.zip): Samples encoded using Vidvox AVF Batch Converter (35 MB zip)
 * [QuickTime Codec](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_Vidvox_QuickTime_Codec.zip): Samples encoded using the Vidvox QuickTime codec (20 MB zip)
 * [DirectShow Codec](https://d3omao0uy1rjjh.cloudfront.net/hap/Hap_Test_RenderHeads_DirectShow_Codec.zip): Samples encoded using the RenderHeads DirectShow codec (5 MB zip)

 Vidvox also have [a selection of sample media](https://docs.vidvox.net/vdmx_sample_media.html#media-files).


Open-Source
----

The HAP codec project is open-source, licensed under a [Free BSD License](https://github.com/vidvox/hap/blob/master/LICENSE), meaning you can use it in your commercial or noncommercial applications free of charge.

We like to know about software that supports HAP, so if you are using it for a project, please get in touch.

This project was originally written by [Tom Butterworth](http://kriss.cx/tom/) and commissioned by [VIDVOX](http://www.vidvox.net), 2012.

# Introduction

**NEWS: A new maintainer is wanted**

OpenCVX is a project to provide tweaked functions or extensional functions for OpenCV
which is a well known open source Computer Vision C library. 

Currently, for example, cvGaussPdf (**Gaussian Probability density** function), cvGmmPdf (**Mixture of Gaussian** Probability function), cvCropImageROI (Crop an affine transformed region of interest from an image), cvPutImageROI (Put an sub-image on an affine transformed region of interest of an image), several **skin-color pixel extraction** methods, and the **particle filtering** method (typically, used for object tracking), and so on, are available.

OpenCVX is a **C header-only** library. 
Thus, it requires no installation step. You only need to #include a file which you want.  

I am glad if the original OpenCV developers like to get my functions into the OpenCV itself. Let me know sonots(at)gmail.com. 

# Documentation

See http://sonots.github.io/opencvx/index_functions.html to know what functions are available. 

Click function links and see their descriptions. You can find a filename to #include at the bottom of the Description page, and you may see Source tab if you feel the description has lack of explanations. 

# How to Install

This is a header-only library. To use, you only need to write

```c
#include "opencvx/function.h"
```

in your file where "opencvx" is the path to the opencvx library.  

This library is basically ruled as one file - one function. 

# Derivatives

* [imageclipper](http://code.google.com/p/imageclipper/) A tool to clip images manually fast
* [facetrack](./docs/ParticleFilter.md) Object Tracking using Particle filtering. 

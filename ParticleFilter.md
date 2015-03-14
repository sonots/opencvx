Object Tracking using Particle Filtering

# Introduction #

Particle filter is a method to implement a Bayesian inference filter using Monte Carlo Simulation. It is well known that the Kalman filter provides an analytically optimal Bayesian solution for the linear/Gaussian case. Particle filter is more general, and can model non-linear/non-Gaussian case. Particle filter gained popularity for object tracking  because it was introduced as the Condensation algorithm for object tracking in the computer vision community.

<a href='http://www.youtube.com/watch?feature=player_embedded&v=xkSphkpA2Ps' target='_blank'><img src='http://img.youtube.com/vi/xkSphkpA2Ps/0.jpg' width='425' height=344 /></a>

# Table of Contents #

  * [Download](#Download.md)
  * [How To Use](#How_To_Use.md)
    * [templatetrack.exe](#templatetrack.exe.md)
    * [facetrack.exe](#facetrack.exe.md)
    * [facetrain.exe](#facetrain.exe.md)
  * [How To Compile](#How_To_Compile.md)
    * [Brief Description of Files](#Brief_Description_of_Files.md)
  * [Demo](#Demo.md)
  * [References](#References.md)

# Download #

This is a part of the [OpenCVX](http://code.google.com/p/opencvx/) library.

Download **facetrack`*`.zip** from http://code.google.com/p/opencvx/downloads/list

This has two tracking methods, templatetrack.exe and facetrack.exe. Source codes are also available.

# How To Use #

## templatetrack.exe ##

```
$ templatetrack.exe -sx 10 -sy 10 -sw 10 -sh 10 -sr 10 -p 100 <video_filename>
```

Select an initial region and use the region as a template for template matching.

```
Object tracking using particle filtering with simple template matching
Usage: templatetrack
 --featsize <width = 24> <height = 24>
     All image patches (template too) are resized into this size to compare.
 -p <num_particles = 100>
     Number of particles (generated tracking states).
 -sx <noise_std_for_x = 3.000000>
     The standard deviation sigma of the Gaussian window for the x coord.
     The Gaussian distribution has a good property called 68-95-99.7 rule.
     Intuitively, the searching window range is +-2sigma in 95%.
 -sy <noise_std_for_y = 3.000000>
     sigma for image patch y coord.
 -sw <noise_std_for_width = 2.000000>
     sigma for image patch width.
 -sh <noise_std_for_height = 2.000000>
     sigma for image patch height.
 -sr <noise_std_for_rotate = 1.000000>
     sigma for image patch rotation angle in degree.
 -o
     Export resulting frames
 --export_format <export_format = %s_%04d.png>
     Determine the exported filenames as sprintf.
 -h
     Show this help
 <vid_file = 02291vFF.avi | camera_index>
```

## facetrack.exe ##

```
$ facetrack.exe <video_filename>
```

Track objects modeld by PCA. Use facetrain.exe to train PCA subspace. Although the name is facetrack.exe, this can be used for any kinds of object tracking. Tracking works well if the kinds of objects can be modeled with a PCA subspace well.

```
Object tracking using particle filtering with PCA DIFS + DFFS method.
Usage: facetrack
 --pcaval <pcaval_xml_path = pcaval.xml>
     Specify trained PCA model data.
 --pcavec <pcavec_xml_path = pcavec.xml>
     Specify trained PCA model data.
 --pcaavg <pcaavg_xml_path = pcaavg.xml>
     Specify trained PCA model data.
 --featsize <width> <height> (Default: (int)sqrt(pcaavg->rows) both)
     Preprocessing image size.
 -p <num_particles = 100>
     Number of particles (generated tracking states).
 -sx <noise_std_for_x = 3.000000>
     The standard deviation sigma of the Gaussian window for the x coord.
     The Gaussian distribution has a good property called 68-95-99.7 rule.
     Intuitively, the searching window range is +-2sigma in 95%.
 -sy <noise_std_for_y = 3.000000>
     sigma for image patch y coord.
 -sw <noise_std_for_width = 2.000000>
     sigma for image patch width.
 -sh <noise_std_for_height = 2.000000>
     sigma for image patch height.
 -sr <noise_std_for_rotate = 1.000000>
     sigma for image patch rotation angle in degree.
 -o
     Export resulting frames
 --export_format <export_format = %s_%04d.png>
     Determine the exported filenames as sprintf.
 -h
     Show this help
 <vid_file = 02291vFF.avi | camera_index>
```


## facetrain.exe ##

```
$ facetrain.exe -featsize 24 24 -M 20 samples.txt
```
Train PCA subpsace. Although the name is facetrain.exe, this is available for any types of objects to train a PCA subspace.
```
Train PCA subpsace.
Usage: facetrain
 --pcaval <pcaval_xml_path = pcaval.xml>
     Specify trained PCA model data.
 --pcavec <pcavec_xml_path = pcavec.xml>
     Specify trained PCA model data.
 --pcaavg <pcaavg_xml_path = pcaavg.xml>
     Specify trained PCA model data.
 --featsize <width> <height> (Default: (int)sqrt(pcaavg->rows) both)
     Preprocessing image size.
 -M <number_of_reduced_dimension = 24>
     Number of reduced feature dimensions.
 <samples_collection_file>
     A file listing image filenames
FYI:The collection file can be created as
 $ find dir/ -name '*.png' > samples.txt
```

# How To Compile #

I included MS Visual C++ project files and Makefile for Linux.

Install OpenCV and learn how to compile a sample program.

Now, you should have no difficulty to compile.

```
$ make 
```

on Linux and Build using MS Visual C++ on Windows.

## Brief Description of Files ##


  * templatetrack.cpp - template tracking
  * facetrack.cpp - PCA DIFS + DFFS tracking
  * facetrain.cpp - Train PCA subspace
  * observetemplate.h - Template matching observation model
  * observepca.h - PCA DIFS + DFFS observation model
  * state.h - State transition model (x, y, width, height, angle)
  * pcaavg.xml - Average of object samples. A PCA parameter.
  * pcaval.xml - Eigenvalues of object samples. A PCA parameter.
  * pcavec.xml - Eigenvectors of object samples. A PCA parameter.
  * opencvx/ - OpenCVX library. Please see [OpenCVX](http://code.google.com/p/opencvx/). The particle filtering (cvparticle.h) is a part of OpenCVX library.

# Demo #

Before

<a href='http://www.youtube.com/watch?feature=player_embedded&v=krA8gI6oMUo' target='_blank'><img src='http://img.youtube.com/vi/krA8gI6oMUo/0.jpg' width='425' height=344 /></a>

After

<a href='http://www.youtube.com/watch?feature=player_embedded&v=xkSphkpA2Ps' target='_blank'><img src='http://img.youtube.com/vi/xkSphkpA2Ps/0.jpg' width='425' height=344 /></a>

# References #

  * 1) [The ConDensation Algorithm Homepage](http://www.robots.ox.ac.uk/~misard/condensation.html)
    * Another origin of particle filter named ConDensation proposed in Computer Vision community.
  * 2) [A tutorial on particle filters for online nonlinear/non-Gaussian Bayesian tracking](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.16.4200)
```
@ARTICLE{Maskell02atutorial,
    author = {Simon Maskell and Neil Gordon},
    title = {A tutorial on particle filters for online nonlinear/non-Gaussian Bayesian tracking},
    journal = {IEEE Transactions on Signal Processing},
    year = {2002},
    volume = {50},
    pages = {174--188}
}
```
  * 3) [Probabilistic Visual Learning for Object Representation](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.53.869)
```
@INPROCEEDINGS{Moghaddam95probabilisticvisual,
    author = {Baback Moghaddam and Alex Pentl},
    title = {Probabilistic visual learning for object detection},
    booktitle = {},
    year = {1995},
    pages = {786--793}
}
```
  * 4) [OpenCV](http://sourceforge.net/projects/opencvlibrary/)
    * OpenCV library itself also has an implementation for ConDensation, but not easy to tweak.
  * 5) [Rob Hess - School of EECS @ Oregon State](http://web.engr.oregonstate.edu/~hess/)
    * He provides an article to get introduction [Course Project Presentation by Rob Hess](http://web.engr.oregonstate.edu/~hess/downloads/track.pdf) and C source codes for particle filtering.
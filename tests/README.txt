Using CxxTest. See
http://cxxtest.sourceforge.net/guide.html

HOW TO INSTALL CxxTest

CxxTest is a header-only library. 
You do not need to compile the library to install.
(It has *.cpp files, but they are also #included)

Linux + GNU g++

1. Install perl (skip if you already have perl)
2. Download cxxtest-x.xx.x.zip
3. $ unzip cxxtest-x.xx.x.zip
4. $ mv cxxtest ~/usr/cxxtest
Try to compile ~/usr/cxxtest/sample/SimpleTest.h
5. perl ~/usr/cxxtest/cxxtestgen.pl --error-printer SimpleTest.h -o SimpleTest.cxx
6. g++ -I~/usr/cxxtest SimpleTest.cxx -o SimpleTest

Windows + MinGW

1. Install perl
2. Download cxxtest-x.xx.x.zip
3. Unzip cxxtest-x.xx.x.zip
4. Move cxxtest e.g., C:\cxxtest
Try to compile C:\cxxtest\sample\SimpleTest.h
5. perl C:\cxxtest\cxxtestgen.pl --error-printer SimpleTest.h -o SimpleTest.cxx
6. g++ -I"C:\cxxtest" SimpleTest.cxx -o SimpleTest.exe

Windows + MS Visual Studio

Just how to set up MS Visual Studio is different. See
http://wuhrr.wordpress.com/2008/01/15/how-to-set-up-cxxtest/

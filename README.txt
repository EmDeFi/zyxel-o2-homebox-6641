ReadMe for Homebox 6641 V1.00(AAJG.0)b14
0. Introduction

  This file will show you how to build the  Homebox 6641 linux system, please 
note, the download image will overwrite the original image existed in the flash
memory of EV board.


1. Package file 

   A. HomeBox6641_consumer_release.tar.gz            - HomeBox6641 open source code
 
   B. uclibc-crosstools-gcc_source-4.4.2.tar.bz2     - Toolchains open source code

   C. ReadMe_for_Homebox_6641(V1.00(AAJG.0)b14).txt  - This file

   D. HowToBuildToolChains.txt                       - Compile toolchains source code SOP

   E. HomeBox_6641_tool.sh                           - Shell script for install necessary tools 


2. Build up compiler environment.

   A. We suggest you use the Linux distribution to setup your environment for reducing compatible issue.
      Please Install Ubuntu 12.04 Desktop 32bit 

   B. Please also change your shell command to bash ,you can follow below steps to modify it.

	$ sudo dpkg-reconfigure dash
	choice "no" and pass enter.

   C. Please check below necessary tools on your environment.

      links / make / gcc / bison / gawk / automake / flex / zlib1g-dev / g++

      Or used command to install these tools.

	$ sudo apt-get -y install links make gcc bison gawk automake flex zlib1g-dev g++
      Or
	$ source HomeBox_6641_tool.sh

       If you can't install package you can use gui interface to update system or use below command
	$ sudo apt-get update && sudo apt-get upgrade


3. Build the firmware for Web-GUI upgrade using
   NOTE: You can't do following things as "root"

   A. Untar the source code.

	# tar zxvf HomeBox6641_consumer_release.tar.gz

   After decompressing, you will see three files in directory:

   a. HomeBox6641_consumer.tar.gz            - Open source code
   b. consumer_install                       - Installing script
   c. uclibc-crosstools-gcc-4.4.2-1.tar.bz2  - Pre-build binary package for toolchain


   B. Build image 

	# ./consumer_install
        Press "y" 3 time
	# cd HomeBox6641_router
	# make PROFILE=DSL-2492GNAUID-B3CC

    The firmware image will locate at "HomeBox6641_router/images".


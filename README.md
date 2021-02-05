# sail7

This rep is mirroring the sail7 code at
[http://www.xflr5.tech/sail7/sail7.html](http://www.xflr5.tech/sail7/sail7.html)

---


## Source code download
	The source code can be downloaded from the SVN repository. 
		https://svn.code.sf.net/p/sail7/code/trunk

	To keep up-to-date with the latest source code modifications, use an SVN client to check out the code from this same repository. 
		- In Windows this can be done for instance with the TortoiseSVN client (http://tortoisesvn.tigris.org/)
		- In Linux, the command line would be:  svn co https://svn.code.sf.net/p/sail7/code/trunk sail7-code

 

## Compilation instructions
	- To compile on Windows, Mac, and Linux, the easiest way is to download QtCreator from Nokia.
		In Windows, QtCreator comes with the MinGW compiler.
		In Linux, you may also need to install the Mesa-devel package

	Other possibilities:
        - To compile under Ubuntu/Debian Linux, please do the following:
			go to the directory, type dpkg-buildpackage -rfakeroot.

        - To compile manually on any Linux distro, please follow the following steps:
			1. Get the libqt4-devel, Mesa and Mesa-devel packages
			2. su
			3. qmake -makefile sail7.pro target.path=/usr/bin
			4. make
			5. sudo make install 

        - To compile on MacOSX, please follow the following steps:
			1. qmake -makefile
			2. qmake -spec macx-g++
			3. make -j3
			4. ./mac/makedist	

# Link
An app creation/GUI library in C++.

Currently, Link only works for Linux.

# Goals
* Have all basic controls (Panel, Button, Label, Progressbar, etc..)
* Rendered using OpenGL
* Linux and Windows versions

# Linux build instructions
*Script instructions:*

Run the Build.sh script like so: ```./Build.sh```

*Manual build instructions:*

Run CMake to generate the makefile like so: ```cmake -S . -B Build```

-S Specifies the source directory, which in this case is the root project directory.

-B Specifies where liblink.a will be built, in this case the ```Build``` directory.

Then, run make to build the project like so: ```make -C Build all```

You can also use the ```-j8``` option where ```8``` is the number of cores your CPU has. It makes building go a bit faster.

You can then find a test app in the ```Tests``` directory and build it the same way. They are of course executables though, so to run it use ```./{BuildDirectory}/{TestAppName}```. 

Example (for the TestApp test): ```./Build/TestApp```

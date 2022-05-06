# Tether
Tether is a app creation/GUI library created in C++. 

# How to contribute
Message SawtoothGabe#6139 on discord that you would like to work on Tether (and some prior experience in C++ would help too).

*Naming conventions*

* **Class/Enums: CamelCase**
* **Variable: lowerCamelCase**
* **Function: CamelCase**
* **Enum value: SNAKE_CASE**

# CMake options
* TETHER_PREVIEW_FEATURES : Defined if tether should have preview features enabled.
* TETHER_OPENGL_API : Defined if tether should have OpenGL support.
* TETHER_VULKAN_API : Defined if tether should have Vulkan support.

# Linux build instructions
Run CMake to generate the makefile like so: ```cmake -S . -B Build```

-S Specifies the source directory, which in this case is the root project directory.

-B Specifies where libtether.a will be built, in this case the ```Build``` directory.

Then, run make to build the project like so: ```make -C Build all```

You can also use the ```-j8``` option where ```8``` is the number of cores your CPU has. It makes building go a bit faster.

You can then find a test app in the ```Tests``` directory and build it the same way. They are of course executables though, so to run it use ```./{BuildDirectory}/{TestAppName}```. 

Example (for the TestApp test): ```./Build/TestApp```

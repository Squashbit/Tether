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
* TETHER_BUILD_TESTS : Enable if the tests should be built
* TETHER_BUILD_AS_SHARED : Enabled by default. Disable if tether should be built as a
	static library.

# Preprocessor defs
* TETHER_STATIC : Define this before including tether if you are using tether 
	from a static library (.lib or .a)

# Build instructions
Just run cmake in the root repo directory and the built files will be in the Build directory.

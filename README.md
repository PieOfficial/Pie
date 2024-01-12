# ***Pie***
Pie is a free and open source build system designed for power, speed, and productivity. Pie is a general purpose build system that is both easy to use and efficient. It is simple in terms of syntax and provides quick build times! 

Pie is written in C and C++, it's designed to be easy to use, extensible, and fast. Pie's syntax is similar to CMake (Another build system).
- [***Pie***](#pie)
  - [Building](#building)
  - [Licensing ](#licensing-)
  - [Credit](#credit)

[![C/C++ CI](https://github.com/PieOfficial/Pie/actions/workflows/c-cpp.yml/badge.svg?branch=main&event=push)](https://github.com/PieOfficial/Pie/actions/workflows/c-cpp.yml)

<div id="badges">
 <img src="https://img.shields.io/github/license/PieOfficial/Pie"/>  
</div>

##  Building
Pie can only be built and run on Windows at the moment.
For compiling from source, the dependencies are:
- make
- cmake
- GNU G++ (>= 11.2.0)

Follow these steps to compile:
1. Clone the github Pie repository with the following command:
   ```
   git clone https://github.com/PieOfficial/Pie Pie --recursive
   ``` 
   Make sure you use the *`--recursive`* flag since the code has some submodule dependencies
    
2. Move into the *Pie* directory using `cd Pie`

3. Compile the source code using *make*. If your *g++* version is newer or equal to the one listed above, simply run `make`
        
        
##  Licensing </p>
Pie uses the MIT license. Please read the [license](https://github.com/PieOfficial/Pie/blob/main/LICENSE) carefully.
## Credit
- [CatCareTaker/carescript-api](https://github.com/CatCareTaker/carescript-api "CatCareTaker/carescript-api") For both the lexer and the parser

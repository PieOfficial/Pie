# ***Pie***
 Pie is a build system designed for the purpose of power, speed and productivity. general purpose build tool that combines ease of use with high efficiency. It is very easy to use syntactically, and also provides great build times!
 - [Introduction](#Pie)
- [Building](#Building)
- [Licensing](#licensing)
- [Credit](#Credit)

[![C/C++ CI](https://github.com/PieOfficial/Pie/actions/workflows/c-cpp.yml/badge.svg?branch=main&event=push)](https://github.com/PieOfficial/Pie/actions/workflows/c-cpp.yml)

<div id="badges">
 <img src="https://img.shields.io/github/license/PieOfficial/Pie"/>  
</div>

##  Building
Pie can only be built and run on Windows at the moment.
For compiling the source, you would require:
- make
- GNU G++ (>= 11.2.0)

Follow these steps to compile:
1. Clone the github Pie repository with the following command:
   ```
   git clone https://github.com/PieOfficial/Pie Pie --recursive
   ``` 
   Make sure you use the *--recursive* flag since the code has some submodule dependencies
    
2. Move into the *Pie* directory using 

        cd Pie
3. Now we will compile the source code using *make*. If your *g++* points to one of the suitable compilers listed above the simply run 
    
        make
        
        
##  Licensing </p>
Pie uses the MIT license. Please read the [license](https://github.com/PieOfficial/Pie/blob/main/LICENSE) carefully.
## Credit
- [LabRicecat/CatCaretaker](https://github.com/LabRicecat/CatCaretaker "LabRicecat/CatCaretaker") For both the lexer and the parser

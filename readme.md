# 2D Shadows
A Project for implementing 2D Shadow in OpenGL and C++

## Build
Dependencies:
* cmake 3.2  
* gcc 5.1.0 / clang 3.7.1
* OpenGL 3.3

Git Clone this Project.  
`cd 2DPixelPerfectShadows`   
Then update the checkout the submodules.  
`git submodule update --init --recursive`  
Make a build folder.  
`mkdir build`   
`cd build`   
Run Cmake.  
`cmake -DBUILD_SHARED_LIBS=OFF -G "Unix Makefiles" ..`  
Run Make.  
`make 2DShadows`  
Run The Program.  
Have fun :)  

## Usage

With left click you can create light sources.  
If there is already a light source it will be highlighted for selection.  
With pressed left click you can move the light around. 
To deselect simply use right click.  
  
There is also a Simple Toolbox to modify a selected light or to change the default settings.  

## Credits

Thanks to [Matt DesLauriers](https://github.com/mattdesl "Matt DesLauriers") who has made
a great tutorial on which my program is heavily based on.
https://github.com/mattdesl/lwjgl-basics/wiki/2D-Pixel-Perfect-Shadows

## License

Copyright (c) 2016 Jeremias Boos

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

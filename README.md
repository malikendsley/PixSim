* Does not require SDL2, as it's bundled with and CMake will copy it into the output directory for you. If the included copy of SDL2 becomes too outdated, feel free to swap it out.
* Requires CMake.
* Installation:
* From root, run `cmake -B build -S .`
* Then run `cmake --build build`
* I use Visual Studio's compiler, so you may need to adjust if you are using a different one.

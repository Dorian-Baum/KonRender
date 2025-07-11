# KonRender

Konheader seeks to make 3D rendering in the console as easy as it can be.
Ease of use is what this project is all about.

## Features

  - Functional rendering
  - Textures 
  - Input (invisible SDL2 window)
  - OBJ importer
  - PPM importer

## Dependencies

  - C++ compiler
  - FTXUI library
  - SDL2 library
  - Boost library

## Compilation

these are my compiler flags, that I use:
  - g++ -O2 example.cpp -lftxui-component -lftxui-dom -lftxui-screen -lSDL2 -pthread -march=native -msse2 -fexpensive-optimizations -fomit-frame-pointer -flto  

## License Considerations

License compliance:

  - Possible license incompatibilities
  - Rendering techniques that might be patented

Attribution:

  - Missing credits for referenced resources
  - Incomplete documentation of influences

How You Can Help
If you notice:

  - Files that resemble known licensed implementations
  - Potential license conflicts

I'm considering using MIT License for this project but need to ensure all content is properly attributed and compatible.

## Future Improvements

  - Implement proper lighting
  - Support animated textures
  - Create simple scene format
  - Improve error handling
  - Add comprehensive license documentation

Thank you for reading

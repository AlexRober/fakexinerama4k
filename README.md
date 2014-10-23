fakexinerama4k
==============

A version of fakexinerama that sits between the actual libXinerama and clients and sticks together 2 1920x2160 tiled screens into a 3840x2160 screen, useful for 4k monitors that use MST

Based on https://gist.github.com/phillipberndt/7688785 so check there for instructions

#Known Issues:
1) Might not stick together the right 1920x2160 tiles if run with mutliple 4k monitors, espcially  if Xinerama screen numbers aren't sequential

2) Doesn't attempt to support rotated 4k screens and their 2160x1920 tiles, although would be trival to all

#!/bin/sh
i486-mingw32-g++ play.cpp draw.cpp `/usr/i486-mingw32/bin/sdl-config --cflags --static-libs` -std=gnu++11 -lopengl32 -lglu32 -DNONET -static

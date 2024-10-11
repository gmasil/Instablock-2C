#! /bin/bash

magick terrain.png -interpolate Integer -filter point -resize "800%" output.png

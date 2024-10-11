#! /bin/bash

magick "$1" -type truecolor -depth 8 "$1.bmp"

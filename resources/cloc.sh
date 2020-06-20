#!/bin/bash

cloc --exclude-dir=libjapan,libkansai --not-match-f='(state-test\d.c)' ./source/
cloc --exclude-dir=glad ./source/engine-sdl/libjapan/source ./source/engine-sdl/libkansai/source

#!/bin/sh

cd "$(dirname "$0")"

../../tools/bin/linux/sokol-shdc -i unlit.glsl -o include/unlit.h --slang=glsl330


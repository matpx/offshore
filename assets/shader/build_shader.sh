#!/bin/sh

cd "$(dirname "$0")"

../../tools/ShaderMake/bin/ShaderMake -c shader.cfg -o include --header -p SPIRV --compiler ${VULKAN_SDK}/bin/dxc --tRegShift 0 --sRegShift 128 --bRegShift 256 --uRegShift 384

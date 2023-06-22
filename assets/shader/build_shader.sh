#!/bin/sh

cd "$(dirname "$0")"

../../tools/ShaderMake/bin/ShaderMake -c shader.cfg -o include --header -p SPIRV --compiler ${VULKAN_SDK}/bin/dxc
../../tools/ShaderMake/bin/ShaderMake -c shader.cfg -o include --header -p SPIRV --compiler ${VULKAN_SDK}/bin/dxc

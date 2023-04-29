@ctype mat4 float4x4

@vs vs

layout(binding = 0) uniform VP {
  mat4 vp;
};

in vec3 position;

void main() {
  gl_Position = vp * vec4(position, 1);
}

@end

@fs fs

out vec4 frag_color;

void main() {
  frag_color = vec4(1,0,0,1);
}

@end

@program unlit vs fs

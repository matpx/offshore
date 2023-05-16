@ctype mat4 mat4

@vs vs

layout(binding = 0) uniform VP {
  mat4 vp;
};

in vec3 position;

out vec3 global_position;

void main() {
  global_position = position;
  gl_Position = vp * vec4(position, 1);
}

@end

@fs fs

in vec3 global_position;

out vec4 frag_color;

void main() {
  frag_color = vec4(global_position * 0.4, 1);
}

@end

@program unlit vs fs

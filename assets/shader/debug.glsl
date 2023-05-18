@ctype mat4 mat4

@vs vs

layout(binding = 0) uniform MVP {
  mat4 mvp;
};

in vec3 position;

void main() {
  gl_Position = mvp * vec4(position, 1);
}

@end

@fs fs

out vec4 frag_color;

void main() {
  frag_color = vec4(1,0,0,1);
}

@end

@program debug vs fs

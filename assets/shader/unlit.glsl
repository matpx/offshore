@vs vs

in vec3 position;

void main() {
  gl_Position = vec4(position, 1);
}

@end

@fs fs

out vec4 frag_color;

void main() {
  frag_color = vec4(1,0,0,1);
}

@end

@program unlit vs fs

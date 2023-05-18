@ctype mat4 mat4

@vs vs

layout(binding = 0) uniform MVP {
  mat4 mvp;
};

in vec3 a_position;
in vec3 a_normal;

out vec3 v_normal;

void main() {
  v_normal = a_normal;
  gl_Position = mvp * vec4(a_position, 1);
}

@end

@fs fs

in vec3 v_normal;

out vec4 frag_color;

void main() {
  frag_color = vec4(v_normal, 1);
}

@end

@program unlit vs fs

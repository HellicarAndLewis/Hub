const float poolHeight = 1.0;
varying vec3 position;

void main() {
  position = gl_Vertex.xyz;
  position.y = ((1.0 - position.y) * (7.0 / 12.0) - 1.0) * poolHeight;
  gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1.0);
}

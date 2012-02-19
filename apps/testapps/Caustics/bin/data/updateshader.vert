#version 120
#extension GL_ARB_texture_rectangle : enable

void main() {
//  coord = gl_Vertex.xy * 0.5 + 0.5;
//  gl_Position = vec4(gl_Vertex.xyz, 1.0);
  gl_Position = ftransform();
  gl_TexCoord[0] = gl_MultiTexCoord0;
}
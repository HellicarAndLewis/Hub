#version 120

uniform sampler2DRect texture;
uniform vec2 delta;

void main() {

  /* get vertex info */
  vec4 info = texture2DRect(texture, gl_TexCoord[0].st);
  
  vec3 dx = vec3(delta.x, texture2DRect(texture, vec2(gl_TexCoord[0].s + delta.x, gl_TexCoord[0].t)).r - info.r, 0.0);
  vec3 dy = vec3(0.0, texture2DRect(texture, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t + delta.y)).r - info.r, delta.y);
  info.ba = normalize(cross(dy, dx)).xz;

  gl_FragColor = info;


}
uniform sampler2DRect texture;
uniform vec2 delta;
uniform float drag;

void main() {
  /* get vertex info */
  vec4 info = texture2DRect(texture, gl_TexCoord[0].st);
  
  /* calculate average neighbor height */
  vec2 dx = vec2(delta.x, 0.0);
  vec2 dy = vec2(0.0, delta.y);
  float average = (
	texture2DRect(texture, gl_TexCoord[0].st - dx).r +
	texture2DRect(texture, gl_TexCoord[0].st - dy).r +
	texture2DRect(texture, gl_TexCoord[0].st + dx).r +
	texture2DRect(texture, gl_TexCoord[0].st + dy).r
  ) * 0.25;
  
  info.g += (average - info.r)*2.0;
  info.g *= drag;
  
  /* move the vertex along the velocity */
  info.r += info.g;

  gl_FragColor = info;
}
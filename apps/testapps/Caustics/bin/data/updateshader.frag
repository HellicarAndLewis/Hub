#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect texture;
uniform vec2 delta;
//varying vec2 coord;

void main() {
  /* get vertex info */
  vec4 info = texture2DRect(texture, gl_TexCoord[0].st);
  
  /* calculate average neighbor height */
  vec2 dx = vec2(delta.x*1., 0.0);
  vec2 dy = vec2(0.0, delta.y*1.);
  float average = (
	texture2DRect(texture, gl_TexCoord[0].st - dx).r +
	texture2DRect(texture, gl_TexCoord[0].st - dy).r +
	texture2DRect(texture, gl_TexCoord[0].st + dx).r +
	texture2DRect(texture, gl_TexCoord[0].st + dy).r
  ) * 0.25;
  
  info.g += (average - info.r)*2.0;

  info.r += (info.g - .5);
  
  info.g += (.5 - info.g) * .01;
  
//  //WORKING
//  /* change the velocity to move toward the average */
//  info.g += (average - info.r) * 2.0;
//  
//  /* attenuate the velocity a little so waves do not last forever */
//  //info.g += (.5 - info.g) * .01;
//  info.g *= .9;
//  
//  /* move the vertex along the velocity */
//  info.r += info.g;
//  info.r = clamp(info.r,0.,1.);

  gl_FragColor = info;
  //gl_FragColor = vec4(gl_TexCoord[0].s, gl_TexCoord[0].t, 0., 1.0);
}
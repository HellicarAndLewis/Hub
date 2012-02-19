#version 120
#extension GL_ARB_texture_rectangle : enable

const float PI = 3.141592653589793;
uniform sampler2DRect texture;
uniform vec2 center;
uniform float radius;
uniform float strength;
//varying vec2 coord;

void main() {
	/* get vertex info */
	vec4 info = texture2DRect(texture, gl_TexCoord[0].st);

	/* add the drop to the height */
	float drop = max(0.0, 1.0 - length(center - gl_TexCoord[0].st) / radius);
	drop = 0.5 - cos(drop * PI) * 0.5;
	info.r = clamp(info.r + drop * strength, 0., 1.0);

	
	gl_FragColor = info;
}
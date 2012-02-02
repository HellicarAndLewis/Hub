#version 110

uniform sampler2DRect base;
uniform sampler2DRect warp;
uniform float warpScale;

void main()
{
	vec2 offset  = texture2DRect(warp, gl_TexCoord[0].st).rg - vec2(.5, .5);
	gl_FragColor = texture2DRect(base, gl_TexCoord[0].st + offset * warpScale );	
}
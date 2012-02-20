#version 110

uniform sampler2DRect base;
uniform sampler2DRect caustics;
uniform float glowScale;
void main()
{
	float glow = (texture2DRect(caustics , gl_TexCoord[1].st).r) * glowScale;
	//gl_FragColor = vec4(texture2DRect(base, gl_TexCoord[0].st ).rgb + glow, 1.0);	
	gl_FragColor = vec4(texture2DRect(base, gl_TexCoord[0].st).rgb + glow, 1.0);
}
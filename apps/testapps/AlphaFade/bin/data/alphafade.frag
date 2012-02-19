#version 110

uniform sampler2DRect self;
void main()
{

	vec4 samp = texture2DRect(self, gl_TexCoord[0].st);
	samp.a -= .002;
	gl_FragColor = samp;
}
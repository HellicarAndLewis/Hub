uniform sampler2DRect tex;


uniform vec2 centre;
uniform float radius;
uniform float amount;
void main()
{
	
	float amount = 1.0;

	vec2 coord = gl_TexCoord[0].st;
	
	vec4 color = texture2DRect(tex,coord);

	vec2 dist = coord - centre;
	
	
	float far = length(dist);
	float amt = clamp((radius - far)/radius, 0.0, 1.0);
	
	
	
	
	
	
	
	
	
	amt = amt * amount * 2.0 + 1.0*(1.0-amount);
	gl_FragColor = color*vec4(1.0, 1.0, 1.0, amt);
}
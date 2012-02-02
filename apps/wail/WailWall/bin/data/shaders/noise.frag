uniform sampler2DRect permTexture;			// Permutation texture
const float permTexUnit = 1.0;		// Perm texture texel-size
const float permTexUnitHalf = 0.5;	// Half perm texture texel-size
uniform float flow;
uniform float wobbleX;
uniform float wobbleY;
uniform vec2 scale;

//uniform vec3 colorA;
//uniform vec3 colorB;
 
float fade(in float t) {
	return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float pnoise2D(in vec2 p)
{
	// Integer part, scaled and offset for texture lookup
	vec2 pi = permTexUnit*floor(p) + permTexUnitHalf;
	// Fractional part for interpolation
	vec2 pf = fract(p);
	
	// Noise contribution from lower left corner
	vec2 grad00 = texture2DRect(permTexture, pi).rg * 4.0 - 1.0;
	float n00 = dot(grad00, pf);
	
	// Noise contribution from lower right corner
	vec2 grad10 = texture2DRect(permTexture, pi + vec2(permTexUnit, 0.0)).rg * 4.0 - 1.0;
	float n10 = dot(grad10, pf - vec2(1.0, 0.0));
	
	// Noise contribution from upper left corner
	vec2 grad01 = texture2DRect(permTexture, pi + vec2(0.0, permTexUnit)).rg * 4.0 - 1.0;
	float n01 = dot(grad01, pf - vec2(0.0, 1.0));
	
	// Noise contribution from upper right corner
	vec2 grad11 = texture2DRect(permTexture, pi + vec2(permTexUnit, permTexUnit)).rg * 4.0 - 1.0;
	float n11 = dot(grad11, pf - vec2(1.0, 1.0));
	
	// Blend contributions along x
	vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(pf.x));
	
	// Blend contributions along y
	float n_xy = mix(n_x.x, n_x.y, fade(pf.y));
	
	// We're done, return the final noise value.
	return n_xy;
}

/*
float pattern( in vec2 p ){
  vec2 q = vec2( pnoise2D( p + vec2(0.0,0.0) ),
				 pnoise2D( p + vec2(10.2,1.3+wobble2) ) );
  return pnoise2D( p + 4.0*q + flow/100.0 );
}
*/

float pattern( in vec2 p, out vec2 q, out vec2 r )
{
	q.x = pnoise2D( p + vec2(0.0,0.0) );
	q.y = pnoise2D( p + vec2(5.3,1.3) );

	r.x = pnoise2D( p + 4.0*q + vec2(1.7,9.2 + wobbleX) );
	r.y = pnoise2D( p + 4.0*q + vec2(8.3,2.8 + wobbleY) );

	return pnoise2D( p + 4.0*r + flow );
}


void main()
{
	//vec3 colorA = vec3(158./255.0,76./255.0,22.0/255.0);
	//vec3 colorB = vec3(77./255.0, 30.0/255.0, 13.0/255.0);
	
	vec2 q,r;
	pattern(vec2(gl_TexCoord[0].s, gl_TexCoord[0].t) / scale, q, r);
	gl_FragData[0] = vec4(r.x+.5,r.y+.5,0.0,1.0);
	
//	float alpha = pattern(vec2( flow + gl_TexCoord[0].s, gl_TexCoord[0].t) / scale) +.5;
	//gl_FragData[0] = vec4( colorA*alpha + colorB*(1.-alpha), 1.0 );
//	gl_FragData[0].rg = vec2(alpha, );
	
	//gl_FragData[0] = vec4( pnoise2D( vec2(pnoise2D( gl_TexCoord[0].st/100. )) ) +.5);
	//gl_FragData[0] = texture2DRect(permTexture,gl_TexCoord[0].st);
	//gl_FragData[0] = vec4(0.0,1.0,0.0,1.0);
}

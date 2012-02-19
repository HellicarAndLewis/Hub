const float IOR_AIR = 1.0;
const float IOR_WATER = 1.333;
const float poolHeight = 1.0;

//uniform vec3 light;
//varying vec3 oldPos;
//varying vec3 newPos;
//varying vec3 ray;
uniform vec3 light;
//varying vec3 normal;
uniform sampler2DRect water;

vec2 intersectCube(vec3 origin, vec3 ray, vec3 cubeMin, vec3 cubeMax) {
	vec3 tMin = (cubeMin - origin) / ray;
	vec3 tMax = (cubeMax - origin) / ray;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);
	return vec2(tNear, tFar);
}

/* project the ray onto the plane */
vec3 project(vec3 origin, vec3 ray, vec3 refractedLight) {
  vec2 tcube = intersectCube(origin, ray, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
  origin += ray * tcube.y;
  float tplane = (-origin.y - 1.0) / refractedLight.y;
  return origin + refractedLight * tplane;
}

void main() {

	vec4 info = texture2DRect(water, gl_TexCoord[0].st);
	info.ba *= 0.5;
	vec3 normal = vec3(info.b, sqrt(1.0 - dot(info.ba, info.ba)), info.a);
	//vec3 normal = vec3(info.b, info.b, info.a);
	
	/* project the vertices along the refracted vertex ray */
	vec3 refractedLight = refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
	vec3 ray = refract(-light, normal, IOR_AIR / IOR_WATER);
//	oldPos = project(gl_Vertex.xzy, refractedLight, refractedLight);
//	newPos = project(gl_Vertex.xzy + vec3(0.0, info.r, 0.0), ray, refractedLight);
	vec3 oldPos = project(vec3(gl_TexCoord[0].st,0), refractedLight, refractedLight);
	vec3 newPos = project(vec3(gl_TexCoord[0].st,0) + vec3(0.0, info.r, 0.0), ray, refractedLight);

	/* if the triangle gets smaller, it gets brighter, and vice versa */
	float oldArea = length(dFdx(oldPos)) * length(dFdy(oldPos));
	float newArea = length(dFdx(newPos)) * length(dFdy(newPos));
	
	gl_FragColor = vec4(oldArea / newArea * .2, 0.0, 0.0, 0.0);	
	//gl_FragColor = vec4(normal,1.0);
	
//	vec4 info = texture2DRect(water, gl_TexCoord[0].st);
//	info.ba *= 0.5;
	//normal = vec3(info.b, sqrt(1.0 - dot(info.ba, info.ba)), info.a);
//	vec3 normal2 = vec3(info.b, info.b, info.a);
//	gl_FragColor = vec4(normal2,1.0);
	
//	gl_FragColor = texture2DRect(water,gl_TexCoord[0].st).baba;

//	vec3 refractedLight = refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
	/* compute a blob shadow and make sure we only draw a shadow if the player is blocking the light */
	
	
//	vec3 dir = (sphereCenter - newPos) / sphereRadius;
//	vec3 area = cross(dir, refractedLight);
//	float shadow = dot(area, area);
//	float dist = dot(dir, -refractedLight);
//	shadow = 1.0 + (shadow - 1.0) / (0.05 + dist * 0.025);
//	shadow = clamp(1.0 / (1.0 + exp(-shadow)), 0.0, 1.0);
//	shadow = mix(1.0, shadow, clamp(dist * 2.0, 0.0, 1.0));
//	gl_FragColor.g = shadow;
//
//	/* shadow for the rim of the pool */
//	vec2 t = intersectCube(newPos, -refractedLight, vec3(-1.0, -poolHeight, -1.0), vec3(1.0, 2.0, 1.0));
//	gl_FragColor.r *= 1.0 / (1.0 + exp(-200.0 / (1.0 + 10.0 * (t.y - t.x)) * (newPos.y - refractedLight.y * t.y - 2.0 / 12.0)));

}
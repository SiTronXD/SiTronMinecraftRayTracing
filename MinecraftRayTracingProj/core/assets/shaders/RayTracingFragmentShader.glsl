#ifdef GL_ES
precision highp float;
#endif

varying vec2 uvCoords;

struct Ray
{
	vec3 origin;
	vec3 direction;
};

Ray CreateRay(vec3 origin, vec3 direction)
{
	Ray r;
	r.origin = origin;
	r.direction = direction;
	
	return r;
}

void RayHitSphere(inout Ray r, vec3 position, float radius)
{
	float t = dot(position - r.origin, r.direction);
}

vec4 RayTraceObjects(Ray r)
{
	RayHitSphere(r, vec3(0.0), 0.5);
	
	return vec4(1.0);
}

vec4 RayTrace(vec2 uv)
{
	// Camera inputs
	vec3 camPos = vec3(0.0, 0.0, -2.0);
	vec3 camLookAt = vec3(0.0);
	
	// Create camera matrix
	vec3 camForward = normalize(camLookAt - camPos);
	vec3 camRight = normalize(cross(camForward, vec3(0.0, 1.0, 0.0)));
	vec3 camUp = normalize(cross(camRight, camForward));
	
	float zoom = 0.1;
	
	vec3 rayOrigin = camPos + camForward * zoom + camRight * uv.x + camUp * uv.y;
	vec3 rayDirection = normalize(rayOrigin - camPos);

	// Create ray structure
	Ray r = CreateRay(rayOrigin, rayDirection);
	
	return RayTraceObjects(r);
}

void main()
{
	vec4 rayTraceColor = RayTrace(uvCoords);

    gl_FragColor = rayTraceColor;
}




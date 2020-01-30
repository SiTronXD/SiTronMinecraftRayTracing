#ifdef GL_ES
precision highp float;
#endif

varying vec2 v_uvCoords;
varying vec2 v_resolution;

uniform vec3 u_cameraPosition;
uniform vec3 u_cameraViewDirection;

struct Material
{
	vec4 color;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
	Material mat;
};

Material CreateDefaultMaterial()
{
	Material mat;
	mat.color = vec4(0.1, 0.1, 0.1, 1.0);
	
	return mat;
}

Ray CreateRay(vec3 origin, vec3 direction)
{
	Ray r;
	r.origin = origin;
	r.direction = direction;
	r.mat = CreateDefaultMaterial();
	
	return r;
}

float SqrLength(vec3 p)
{
	return dot(p, p);
}

void RayHitSphere(inout Ray r, vec3 position, float radius)
{
	float t = dot(position - r.origin, r.direction);
	vec3 p = r.origin + r.direction * t;
	float y = length(position - (p));
	float x = sqrt(radius*radius - y*y);
	float t1 = t - x;
	float t2 = t + x;
	
	if(t >= 0.0 && SqrLength(position - p) <= radius*radius)
	{
		Material mat;
		mat.color = vec4(0.9, 0.0, 0.0, 1.0);
		
		r.mat = mat;
	}
}

vec4 RayTraceObjects(Ray r)
{
	RayHitSphere(r, vec3(0.0), 1.9);
	
	return r.mat.color;
}

vec4 RayTrace(vec2 uv)
{
	// Camera inputs
	vec3 camPos = u_cameraPosition;
	vec3 camDir = u_cameraViewDirection;
	
	// Create camera matrix
	vec3 camForward = normalize(camDir);
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
	vec2 uv = (v_uvCoords*v_resolution - v_resolution.xy*0.5) / v_resolution.y;
	vec4 rayTraceColor = RayTrace(uv);

    gl_FragColor = rayTraceColor;
}




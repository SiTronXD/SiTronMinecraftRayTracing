#version 130

// Constants
const int LIGHTMAP_SIZE = 1024;
const int NUM_MAX_BLOCKS = 256;
const int CHUNK_WIDTH_LENGTH = 8;
const int CHUNK_HEIGHT = 4;
const int MAX_RAY_BOUNCES = 8;

const float MAX_RAY_DISTANCE = 64.0;
const float TWO_PI = 3.141592f * 2.0f;

// Uniforms
uniform int u_numValidBlocks;
uniform int u_currentIteration;

uniform vec3 u_blocks[NUM_MAX_BLOCKS];

uint wang_hash(inout uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);

	return seed;
}

float randomFloat01(inout uint state)
{
	return float(wang_hash(state)) / 4294967296.0;
}

vec3 randomUnitVector(inout uint state)
{
	float z = randomFloat01(state) * 2.0f - 1.0f;
	float angle = randomFloat01(state) * TWO_PI;
	float radius = sqrt(1.0f - z*z);
	float x = radius * cos(angle);
	float y = cos(angle);

	return vec3(x, y, z);
}

struct Hit
{
	vec3 currentNormal;

	vec3 albedo;
	vec3 emissive;
};

Hit createHit()
{
	Hit h;

	h.currentNormal = vec3(0.0);
	h.albedo = vec3(0.0);
	h.emissive = vec3(0.0);

	return h;
}

struct Ray
{
	vec3 position;
	vec3 direction;
	vec3 oneOverRayDirection;

	float currentT;

	Hit hit;
};

Ray createRay(vec3 pos, vec3 dir)
{
	Ray r;
	r.position = pos;
	r.direction = dir;
	r.oneOverRayDirection = vec3(1.0) / dir;

	r.currentT = MAX_RAY_DISTANCE;

	r.hit = createHit();

	return r;
}


void rayBoxAABBIntersection(inout Ray r, vec3 minCorner, vec3 maxCorner, 
	int loopIndex)
{
	float t1 = (minCorner.x - r.position.x) * r.oneOverRayDirection.x;
	float t2 = (maxCorner.x - r.position.x) * r.oneOverRayDirection.x;
	float t3 = (minCorner.y - r.position.y) * r.oneOverRayDirection.y;
	float t4 = (maxCorner.y - r.position.y) * r.oneOverRayDirection.y;
	float t5 = (minCorner.z - r.position.z) * r.oneOverRayDirection.z;
	float t6 = (maxCorner.z - r.position.z) * r.oneOverRayDirection.z;

	float tMin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tMax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// Box is behind ray, the ray doesn't intersect the box at all, 
	// or there is something closer to the ray
	if(tMax < 0.0 || tMin > tMax || tMin > r.currentT)
	{
		return;
	}

	// Ray hit box!
	float t = tMin;

	vec3 intersectionPoint = r.position + r.direction*t;
	vec3 worldIntersectionPoint = intersectionPoint;

	intersectionPoint += vec3(0.5);
	intersectionPoint = fract(intersectionPoint);
	intersectionPoint.y = 1.0 - intersectionPoint.y;
	
	// Horizontal sides
	if(t == t1 || t == t2)
	{
		r.hit.currentNormal = vec3(t == t1 ? -1.0 : 1.0, 0.0, 0.0);
	}
	// Top or bottom
	else if(t == t3 || t == t4)
	{
		if(worldIntersectionPoint.y - minCorner.y >= 0.5)
		{
			r.hit.currentNormal = vec3(0.0, 1.0, 0.0);
		}
		else
		{
			r.hit.currentNormal = vec3(0.0, -1.0, 0.0);
		}
	}
	// Other sides
	else if(t == t5 || t == t6)
	{
		r.hit.currentNormal = vec3(0.0, 0.0, t == t5 ? -1.0 : 1.0);
	}

	// Transparency test
	/*if(isTransparent(u_blockInfo[loopIndex].z, normalizedUV))
	{
		return;
	}*/

	r.currentT = t;

	r.hit.albedo = vec3(1.0f, 0.1f, 0.1f);
	r.hit.emissive = vec3(0.0f, 0.0f, 0.0f);
}

void raySphereIntersection(inout Ray r, vec3 spherePos, float sphereRadius)
{
	vec3 rayToSphere = spherePos - r.rayPosition;

	// Project and find the closest point on the ray to the sphere
	float projectedPointT = dot(rayToSphere, r.rayDirection); 
	vec3 closestPoint = r.rayPosition + r.rayDirection * projectedPointT;

	// Check if the point is inside the sphere
	float squaredLengthToSphere = squaredLength(closestPoint - spherePos);
	if(squaredLengthToSphere <= sphereRadius*sphereRadius)
	{
		float x = sqrt(sphereRadius*sphereRadius - squaredLengthToSphere);
		float t = projectedPointT - x;

		// The camera could still be inside sphere
		if(t < 0.0)
			t = projectedPointT + x;

		// The sphere is completely behind the ray or there is something closer to the ray
		if(t < 0.0 || r.currentT < t)
			return;

		vec3 intersectionPoint = r.rayPosition + r.rayDirection * t;
		vec3 normal = normalize(intersectionPoint - spherePos);

		r.currentT = t;

		r.hit.currentNormal = normal;
		r.hit.albedo = vec3(0.0f, 0.0f, 0.0f);
		r.hit.emissive = vec3(1.0f, 0.9f, 0.7f) * 100.0f;
	}
}

void raySceneIntersection(inout Ray ray)
{
	for(int i = 0; i < u_numValidBlocks; i++)
	{
		rayBoxAABBIntersection(
			ray, 
			u_blocks[i] + vec3(-0.5), 
			u_blocks[i] + vec3(0.5),
			i
		);
	}
}

void main()
{
	// Find out where we are
	vec2 uv = gl_FragCoord.xy / vec2(LIGHTMAP_SIZE);
	vec3 currentPos = vec3(
		fract(uv) * CHUNK_WIDTH_LENGTH, 
		-(int(uv.y*2.0) + int(uv.x*2.0) % 2)
	);

	vec3 currentCol = vec3(0.0);//vec3(currentPos);
	vec3 throughput = vec3(1.0f);

	uint rngState = 
		uint(uint(fragCoord.x) * uint(1973) + 
		uint(fragCoord.y) * uint(9277) + 
		uint(u_currentIteration) * uint(26699)) | uint(1);

	// Create ray
	Ray ray = createRay(currentPos, normalize(vec3(0.0, 1.0, 0.0) + randomUnitVector(rngState)));

	for(int bounceIndex = 0; bounceIndex < MAX_RAY_BOUNCES; bounceIndex++)
	{
		ray.currentT = MAX_RAY_DISTANCE;
		raySceneIntersection(ray);
		
		if(ray.currentT >= MAX_RAY_DISTANCE)
			break;

		ray.position = (ray.position + ray.direction * ray.currentT) + ray.hit.currentNormal * 0.0001f;

		ray.direction = normalize(ray.hit.currentNormal + randomUnitVector(rngState));

		currentCol += ray.hit.emissive * throughput;
		throughput *= ray.hit.albedo;
	}
	

	gl_FragColor = vec4(currentCol, 1.0);
}





// x = 0, y = 0 => index = 0
	/*vec3 col = vec3(0.0, 0.0, 0.0);

	if(uv.x > 0.5f && uv.y > 0.5f)	// x = 0.5, y = 0.5 => index = 3
		col = vec3(1.0);
	else if(uv.x > 0.5f)			// x = 0.5, y = 0.0 => index = 1
		col = vec3(1.0, 0.0, 0.0);
	else if(uv.y > 0.5f)			// x = 0.5, y = 0.5 => index = 2
		col = vec3(0.0, 1.0, 0.0);


	// x = 0, y = 0 => index = 0
	vec3 col = vec3(1.0) * 0.0;

	if(uv.x > 0.5f && uv.y > 0.5f)	// x = 0.5, y = 0.5 => index = 3
		col = vec3(1.0) * 1.0;
	else if(uv.x > 0.5f)			// x = 0.5, y = 0.0 => index = 1
		col = vec3(1.0) * 0.33;
	else if(uv.y > 0.5f)			// x = 0.5, y = 0.5 => index = 2
		col = vec3(1.0) * 0.66;*/
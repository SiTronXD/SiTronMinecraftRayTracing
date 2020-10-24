#version 130

// Constants
const int NUM_MAX_BLOCKS = 256;
const int CHUNK_WIDTH_LENGTH = 8;
const int CHUNK_HEIGHT = 4;
const int MAX_RAY_BOUNCES = 8;

const float MAX_RAY_DISTANCE = 64.0;
const float TWO_PI = 3.141592f * 2.0f;
const float RAY_SHORT_OFFSET = 0.0001f;

const vec3 IGNORE_COLOR = vec3(1.0f, 0.0f, 1.0f);

// Uniforms
uniform int u_numValidBlocks;
uniform int u_currentIteration;
uniform int u_lightmapSideSize;
uniform int u_lightmapNumHorizontalTiles;
uniform int u_lightmapNumVerticalTiles;
uniform int u_currentSide;	// 0: up/down, 1: left/right, 2: front/back

uniform vec3 u_blocks[NUM_MAX_BLOCKS];
uniform vec3 u_sunColor;

uniform vec4 u_sunSpherePosRadius;

uniform sampler2D u_lastFrameTexture;

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
	float y = radius * sin(angle);

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
	vec3 oneOverDirection;

	float currentT;

	Hit hit;
};

Ray createRay(vec3 pos, vec3 dir)
{
	Ray r;
	r.position = pos;
	r.direction = dir;
	r.oneOverDirection = vec3(1.0) / dir;

	r.currentT = MAX_RAY_DISTANCE;

	r.hit = createHit();

	return r;
}

float squaredLength(vec3 p)
{
	return dot(p, p);
}

void rayBoxAABBIntersection(inout Ray r, vec3 minCorner, vec3 maxCorner)
{
	float t1 = (minCorner.x - r.position.x) * r.oneOverDirection.x;
	float t2 = (maxCorner.x - r.position.x) * r.oneOverDirection.x;
	float t3 = (minCorner.y - r.position.y) * r.oneOverDirection.y;
	float t4 = (maxCorner.y - r.position.y) * r.oneOverDirection.y;
	float t5 = (minCorner.z - r.position.z) * r.oneOverDirection.z;
	float t6 = (maxCorner.z - r.position.z) * r.oneOverDirection.z;

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

	r.currentT = t;

	r.hit.albedo = vec3(1.0f, 1.0f, 1.0f) * 0.8f;
	r.hit.emissive = vec3(0.0f, 0.0f, 0.0f);
}

void raySphereIntersection(inout Ray r, vec3 spherePos, float sphereRadius)
{
	vec3 rayToSphere = spherePos - r.position;

	// Project and find the closest point on the ray to the sphere
	float projectedPointT = dot(rayToSphere, r.direction); 
	vec3 closestPoint = r.position + r.direction * projectedPointT;

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

		vec3 intersectionPoint = r.position + r.direction * t;
		vec3 normal = normalize(intersectionPoint - spherePos);

		r.currentT = t;

		r.hit.currentNormal = normal;
		r.hit.albedo = vec3(0.0f, 0.0f, 0.0f);
		r.hit.emissive = u_sunColor;
	}
}

bool isPositionInsideBlock(vec3 p)
{
	for(int i = 0; i < u_numValidBlocks; i++)
	{
		vec3 delta = abs(p - u_blocks[i]);
		float dist = max(delta.x, max(delta.y, delta.z));

		if(dist < 0.5f)
			return true;
	}

	return false;
}

void raySceneIntersection(inout Ray ray)
{
	for(int i = 0; i < u_numValidBlocks; i++)
	{
		rayBoxAABBIntersection(
			ray, 
			u_blocks[i] + vec3(-0.5), 
			u_blocks[i] + vec3(0.5)
		);
	}

	raySphereIntersection(ray, u_sunSpherePosRadius.xyz, u_sunSpherePosRadius.w);
}

void main()
{
	// Find out where we are
	vec2 uv = gl_FragCoord.xy / 
		(vec2(u_lightmapNumHorizontalTiles, u_lightmapNumVerticalTiles) * u_lightmapSideSize);

	vec3 currentPos = vec3(0.0f);
	vec3 startNormal = vec3(0.0f);
	if(u_currentSide == 0)
	{
		uv /= vec2(CHUNK_WIDTH_LENGTH);

		startNormal = vec3(0.0f, 1.0f, 0.0f);

		currentPos = vec3(
			fract(uv.x * u_lightmapNumHorizontalTiles) * CHUNK_WIDTH_LENGTH - 0.5f,
			-(int(uv.y * u_lightmapNumVerticalTiles) * u_lightmapNumHorizontalTiles + (int(uv.x * u_lightmapNumHorizontalTiles))) + 0.5f,
			fract(uv.y * u_lightmapNumVerticalTiles) * CHUNK_WIDTH_LENGTH - 0.5f
		);
	}
	else if(u_currentSide == 1)
	{
		uv /= vec2(CHUNK_WIDTH_LENGTH, CHUNK_HEIGHT);
		uv.y = 1.0 - uv.y;

		startNormal = vec3(-1.0f, 0.0f, 0.0f);

		currentPos = vec3(
			(int(uv.y * u_lightmapNumVerticalTiles) * u_lightmapNumHorizontalTiles + (int(uv.x * u_lightmapNumHorizontalTiles))) - 0.5f,
			-fract(uv.y * u_lightmapNumVerticalTiles) * CHUNK_HEIGHT + 0.5f,
			fract(uv.x * u_lightmapNumHorizontalTiles) * CHUNK_WIDTH_LENGTH - 0.5f
		);
		
		uv.y = 1.0 - uv.y;
	}
	else if(u_currentSide == 2)
	{
		uv /= vec2(CHUNK_WIDTH_LENGTH, CHUNK_HEIGHT);
		uv.y = 1.0 - uv.y;

		startNormal = vec3(0.0f, 0.0f, -1.0f);

		currentPos = vec3(
			fract(uv.x * u_lightmapNumHorizontalTiles) * CHUNK_WIDTH_LENGTH - 0.5f,
			-fract(uv.y * u_lightmapNumVerticalTiles) * CHUNK_HEIGHT + 0.5f,
			(int(uv.y * u_lightmapNumVerticalTiles) * u_lightmapNumHorizontalTiles + (int(uv.x * u_lightmapNumHorizontalTiles))) - 0.5f
		);
		
		uv.y = 1.0 - uv.y;
	}

	// Check if this point does not have a surface
	if(isPositionInsideBlock(currentPos + startNormal*0.5f) == isPositionInsideBlock(currentPos - startNormal*0.5f))
	{
		gl_FragColor = vec4(IGNORE_COLOR, 0.0);	

		return;
	}

	// Check if the normal is pointing inside a block
	if(isPositionInsideBlock(currentPos + startNormal*0.5f))
	{
		startNormal *= -1;
	}

	// Short offset so the position doesn't start inside a cube
	currentPos += startNormal * RAY_SHORT_OFFSET;

	vec3 currentCol = vec3(0.0);//vec3(currentPos);
	vec3 throughput = vec3(1.0f);

	uint rngState = 
		uint(uint(gl_FragCoord.x) * uint(1973) + 
				uint(gl_FragCoord.y) * uint(9277) + 
				uint(u_currentIteration) * uint(26699)) | uint(1);

	// Create ray
	Ray ray = createRay(currentPos, normalize(startNormal + randomUnitVector(rngState)));

	for(int bounceIndex = 0; bounceIndex < MAX_RAY_BOUNCES; bounceIndex++)
	{
		ray.currentT = MAX_RAY_DISTANCE;
		raySceneIntersection(ray);
		
		if(ray.currentT >= MAX_RAY_DISTANCE)
			break;

		ray.position = (ray.position + ray.direction * ray.currentT) + ray.hit.currentNormal * RAY_SHORT_OFFSET;
		ray.direction = normalize(ray.hit.currentNormal + randomUnitVector(rngState));

		currentCol += ray.hit.emissive * throughput;
		throughput *= ray.hit.albedo;
	}
	
	
	// Blend between last frame and current frame
	vec3 lastFrameCol = texture2D(u_lastFrameTexture, uv).rgb;
	vec3 finalCol = mix(lastFrameCol, currentCol, 1.0f / float(u_currentIteration + 1));

	// "Store" normal in alpha channel
	float storeNormal = 
		(startNormal.x + 
		startNormal.y + 
		startNormal.z) > 0.0 ? 0.75f : 0.25f;

	gl_FragColor = vec4(finalCol, storeNormal);
	
	// normal = 0: no surface, normal = 0.25: negative, normal = 0.75: positive
	// gl_FragColor = vec4(color of pixel, normal);
}
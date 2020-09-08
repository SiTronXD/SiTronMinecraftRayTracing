#version 130

const int NUM_MAX_BLOCKS = 2;

const float MAX_RAY_DISTANCE = 64.0;

uniform vec3 u_cameraPosition;
uniform vec3 u_cameraForwardDirection;

uniform sampler2D u_textureSheet;
uniform vec4 u_blockTextureRect[3];

uniform vec3 u_blocks[NUM_MAX_BLOCKS];

uniform vec2 u_resolution;
uniform float u_time;

vec2 oneOverTextureSize = vec2(1.0) / textureSize(u_textureSheet, 0).xy;

// Ray structure
struct Ray
{
	vec3 rayPosition;
	vec3 rayDirection;
	vec3 oneOverRayDirection;

	float currentT;

	vec3 currentColor;
};

// Create a new ray
Ray createRay(vec3 _rayPosition, vec3 _rayDirection)
{
	Ray r;
	r.rayPosition = _rayPosition;
	r.rayDirection = _rayDirection;
	r.oneOverRayDirection = 1.0 / _rayDirection;

	r.currentT = MAX_RAY_DISTANCE;

	r.currentColor = vec3(0.1);

	return r;
}

// Squared length of a vector. Used for fast distance comparison
float squaredLength(vec3 p)
{
	return p.x*p.x + p.y*p.y + p.z*p.z;
}

void rayBoxAABBIntersection(inout Ray r, vec3 minCorner, vec3 maxCorner)
{
	float t1 = (minCorner.x - r.rayPosition.x) * r.oneOverRayDirection.x;
	float t2 = (maxCorner.x - r.rayPosition.x) * r.oneOverRayDirection.x;
	float t3 = (minCorner.y - r.rayPosition.y) * r.oneOverRayDirection.y;
	float t4 = (maxCorner.y - r.rayPosition.y) * r.oneOverRayDirection.y;
	float t5 = (minCorner.z - r.rayPosition.z) * r.oneOverRayDirection.z;
	float t6 = (maxCorner.z - r.rayPosition.z) * r.oneOverRayDirection.z;

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

	vec3 intersectionPoint = r.rayPosition + r.rayDirection*t;
	vec3 worldIntersectionPoint = intersectionPoint;

	intersectionPoint += vec3(0.5);
	intersectionPoint = fract(intersectionPoint);
	intersectionPoint.y = 1.0 - intersectionPoint.y;

	vec2 tempUV = vec2(0.0);

	// Horizontal sides
	if(t == t1 || t == t2)
	{
		tempUV = vec2(intersectionPoint.z, intersectionPoint.y);

		tempUV = vec2(
			(u_blockTextureRect[1].x + tempUV.x * u_blockTextureRect[1].z) * oneOverTextureSize.x,
			(u_blockTextureRect[1].y + tempUV.y * u_blockTextureRect[1].w) * oneOverTextureSize.y
		);
	}
	// Top or bottom
	else if(t == t3 || t == t4)
	{
		tempUV = vec2(intersectionPoint.x, intersectionPoint.z);

		if(worldIntersectionPoint.y - minCorner.y >= 0.5)
		{
			tempUV = vec2(
				(u_blockTextureRect[0].x + tempUV.x * u_blockTextureRect[0].z) * oneOverTextureSize.x,
				(u_blockTextureRect[0].y + tempUV.y * u_blockTextureRect[0].w) * oneOverTextureSize.y
			);
		}
		else
		{
			tempUV = vec2(
				(u_blockTextureRect[2].x + tempUV.x * u_blockTextureRect[2].z) * oneOverTextureSize.x,
				(u_blockTextureRect[2].y + tempUV.y * u_blockTextureRect[2].w) * oneOverTextureSize.y
			);
		}
	}
	// Other sides
	else if(t == t5 || t == t6)
	{
		tempUV = vec2(intersectionPoint.x, intersectionPoint.y);

		tempUV = vec2(
			(u_blockTextureRect[1].x + tempUV.x * u_blockTextureRect[1].z) * oneOverTextureSize.x,
			(u_blockTextureRect[1].y + tempUV.y * u_blockTextureRect[1].w) * oneOverTextureSize.y
		);
	}

	r.currentT = t;
	r.currentColor = texture2D(u_textureSheet, tempUV).rgb;
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

		// The sphere is completely behind the ray
		if(t < 0.0)
			return;

		vec3 intersectionPoint = r.rayPosition + r.rayDirection * t;
		vec3 normal = normalize(intersectionPoint - spherePos);

		r.currentT = t;
		r.currentColor = vec3(0.9, 0.0, 0.0) * dot(normalize(vec3(1.0, 1.0, -1.0)), normal);
	}
}

void main()
{
	// UV coordinates
	// x: -1.7 to 1.7
	// y: -1 to 1
    vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;
	uv.x *= u_resolution.x / u_resolution.y;

	// Create camera
	vec3 cameraForward = u_cameraForwardDirection;
	vec3 cameraRight = normalize(cross(cameraForward, vec3(0.0, 1.0, 0.0)));
	vec3 cameraUp = normalize(cross(cameraRight, cameraForward));

	// Ray
	float zoom = 1.0;
	vec3 rayPosition = u_cameraPosition + zoom * cameraForward + uv.x * cameraRight + uv.y * cameraUp;
	vec3 rayDirection = normalize(rayPosition - u_cameraPosition);

	Ray r = createRay(rayPosition, rayDirection);

	// Let the ray interact with the world
	for(int i = 0; i < NUM_MAX_BLOCKS; i++)
	{
		rayBoxAABBIntersection(
			r, 
			u_blocks[i] + vec3(-0.5), 
			u_blocks[i] + vec3(0.5)
		);
	}

	// Let the ray interact with the world
	for(int i = 0; i < NUM_MAX_BLOCKS; i++)
	{
		rayBoxAABBIntersection(
			r, 
			u_blocks[i] + vec3(-0.5), 
			u_blocks[i] + vec3(0.5)
		);
	}

	gl_FragColor = vec4(r.currentColor, 1.0);
}
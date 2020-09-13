#version 130

const int NUM_MAX_RAY_BOUNCES = 3;
const int NUM_MAX_BLOCKS = 256;
const int NUM_MAX_TEXTURERECTS = 12;

uniform int u_numValidBlocks;

const float MAX_RAY_DISTANCE = 64.0;

uniform float u_blockIndex[NUM_MAX_BLOCKS];

uniform vec2 u_resolution;

uniform vec3 u_cameraPosition;
uniform vec3 u_blocks[NUM_MAX_BLOCKS];

uniform mat3x3 u_cameraRot;

uniform vec4 u_blockTextureRect[NUM_MAX_TEXTURERECTS];

uniform sampler2D u_textureSheet;


vec2 oneOverTextureSize = vec2(1.0) / textureSize(u_textureSheet, 0).xy;

// Ray structure
struct Ray
{
	vec3 rayPosition;
	vec3 rayDirection;
	vec3 oneOverRayDirection;

	float currentT;

	int currentBlockIndex;

	vec3 currentNormal;
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

	r.currentBlockIndex = -1;

	r.currentNormal = vec3(0.0, 1.0, 0.0);
	r.currentColor = vec3(0.1);

	return r;
}

// Squared length of a vector. Used for fast distance comparison
float squaredLength(vec3 p)
{
	return p.x*p.x + p.y*p.y + p.z*p.z;
}

void rayBoxAABBIntersection(inout Ray r, vec3 minCorner, vec3 maxCorner, int blockIndex)
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

	int textureIndexOffset = 3 * blockIndex;

	// Horizontal sides
	if(t == t1 || t == t2)
	{
		tempUV = vec2(intersectionPoint.z, intersectionPoint.y);

		tempUV = vec2(
			(u_blockTextureRect[textureIndexOffset + 1].x + tempUV.x * u_blockTextureRect[textureIndexOffset + 1].z) * oneOverTextureSize.x,
			(u_blockTextureRect[textureIndexOffset + 1].y + tempUV.y * u_blockTextureRect[textureIndexOffset + 1].w) * oneOverTextureSize.y
		);

		r.currentNormal = vec3(t == t1 ? -1.0 : 1.0, 0.0, 0.0);
	}
	// Top or bottom
	else if(t == t3 || t == t4)
	{
		tempUV = vec2(intersectionPoint.x, intersectionPoint.z);

		if(worldIntersectionPoint.y - minCorner.y >= 0.5)
		{
			tempUV = vec2(
				(u_blockTextureRect[textureIndexOffset + 0].x + tempUV.x * u_blockTextureRect[textureIndexOffset + 0].z) * oneOverTextureSize.x,
				(u_blockTextureRect[textureIndexOffset + 0].y + tempUV.y * u_blockTextureRect[textureIndexOffset + 0].w) * oneOverTextureSize.y
			);
			
			r.currentNormal = vec3(0.0, 1.0, 0.0);
		}
		else
		{
			tempUV = vec2(
				(u_blockTextureRect[textureIndexOffset + 2].x + tempUV.x * u_blockTextureRect[textureIndexOffset + 2].z) * oneOverTextureSize.x,
				(u_blockTextureRect[textureIndexOffset + 2].y + tempUV.y * u_blockTextureRect[textureIndexOffset + 2].w) * oneOverTextureSize.y
			);

			r.currentNormal = vec3(0.0, -1.0, 0.0);
		}
	}
	// Other sides
	else if(t == t5 || t == t6)
	{
		tempUV = vec2(intersectionPoint.x, intersectionPoint.y);

		tempUV = vec2(
			(u_blockTextureRect[textureIndexOffset + 1].x + tempUV.x * u_blockTextureRect[textureIndexOffset + 1].z) * oneOverTextureSize.x,
			(u_blockTextureRect[textureIndexOffset + 1].y + tempUV.y * u_blockTextureRect[textureIndexOffset + 1].w) * oneOverTextureSize.y
		);
		
		r.currentNormal = vec3(0.0, 0.0, t == t5 ? -1.0 : 1.0);
	}

	r.currentT = t;
	r.currentBlockIndex = blockIndex;
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

		// The sphere is completely behind the ray or there is something closer to the ray
		if(t < 0.0 || r.currentT < t)
			return;

		vec3 intersectionPoint = r.rayPosition + r.rayDirection * t;
		vec3 normal = normalize(intersectionPoint - spherePos);

		r.currentT = t;
		r.currentNormal = normal;
		r.currentColor = vec3(0.9, 0.0, 0.0) * dot(normalize(vec3(1.0, 1.0, -1.0)), normal);
	}
}

vec3 GetSkyboxColor(vec3 rayDirection)
{
	float t = (rayDirection.y*1.2 + 1.0f) * 0.5f;
	t = clamp(t, 0.0, 1.0);

	vec3 topColor = vec3(0.85, 0.85, 0.95);
	vec3 bottomColor = vec3(0.4, 0.4, 0.7);
	vec3 skyColor = mix(bottomColor, topColor, t);
	vec3 sunColor = vec3(0.98, 0.93, 0.6);

	float isSun = 0.0;
	isSun = smoothstep(0.97, 1.0, dot(rayDirection, normalize(vec3(1.0))));
	isSun = pow(isSun, 3.0);

	vec3 col = mix(skyColor, sunColor, isSun);

	return col;
}

void main()
{
	// UV coordinates
	// x: -1.7 to 1.7
	// y: -1 to 1
    vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;
	uv.x *= u_resolution.x / u_resolution.y;

	// Create camera
	vec3 cameraRight = u_cameraRot[0];
	vec3 cameraUp = u_cameraRot[1];
	vec3 cameraForward = u_cameraRot[2];

	// Ray
	float zoom = 1.0;
	vec3 rayLookAtPosition = u_cameraPosition + zoom * cameraForward + uv.x * cameraRight + uv.y * cameraUp;
	vec3 rayDirection = normalize(rayLookAtPosition - u_cameraPosition);

	// Let the ray position be fairly close to the camera position
	vec3 rayPosition = u_cameraPosition + rayDirection*0.01f; 


	Ray r = createRay(rayPosition, rayDirection);
	vec3 currentCol = vec3(0.0);

	// Let the ray interact with the world
	for(int currentRay = 0; currentRay < NUM_MAX_RAY_BOUNCES; currentRay++)
	{
		if(currentRay > 0)
			r = createRay(r.rayPosition + r.rayDirection * r.currentT + r.currentNormal * 0.001f, normalize(reflect(r.rayDirection, r.currentNormal)));

		for(int i = 0; i < u_numValidBlocks; i++)
		{
			rayBoxAABBIntersection(
				r, 
				u_blocks[i] + vec3(-0.5), 
				u_blocks[i] + vec3(0.5),
				int(u_blockIndex[i])
			);
		}
		
		// Ray didn't hit anything
		if(r.currentT >= MAX_RAY_DISTANCE)
		{
			currentCol = GetSkyboxColor(r.rayDirection);

			break;
		}
		
		currentCol = r.currentColor;

		if(r.currentBlockIndex != 3)
			break;

		//currentCol += r.currentColor * (currentRay + 1.0) / float(NUM_MAX_RAY_BOUNCES);
	}

	gl_FragColor = vec4(currentCol, 1.0);
}
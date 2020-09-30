#version 130

#define GOD_RAYS_ENABLED 1

// Constants
const int NUM_MAX_RAY_BOUNCES = 8;
const int NUM_MAX_BLOCKS = 256;
const int NUM_MAX_TEXTURERECTS = 12;

const vec3 LIGHT_DIR = normalize(vec3(-1.0));
const int NUM_FOG_SAMPLES = 8;
const float FOG_DENSITY = 0.5;
const float GOLDEN_RATIO_CONJUGATE = 0.618033f; // also just fract(goldenRatio)

// Uniforms
uniform int u_numValidBlocks;

const float MAX_RAY_DISTANCE = 64.0;

uniform float u_time;
uniform float u_blockIndex[NUM_MAX_BLOCKS];
uniform float u_blockSpecular[NUM_MAX_BLOCKS];

uniform vec2 u_resolution;

uniform vec3 u_cameraPosition;
uniform vec3 u_blocks[NUM_MAX_BLOCKS];

uniform mat3x3 u_cameraRot;

uniform vec4 u_blockTextureRect[NUM_MAX_TEXTURERECTS];

uniform sampler2D u_textureSheet;
uniform sampler2D u_blueNoiseTexture;

// Non-uniforms
vec2 oneOverTextureSize = vec2(1.0) / textureSize(u_textureSheet, 0).xy;

// Hit structure
struct Hit
{
	float specular;

	int currentBlockIndex;

	vec3 currentNormal;
	vec4 currentColor;
};

// Ray structure
struct Ray
{
	vec3 rayPosition;
	vec3 rayDirection;
	vec3 oneOverRayDirection;

	float currentT;
	float currentEnergy;
	
	Hit hit;
};

Hit createEmptyHit()
{
	Hit h;

	h.specular = 0.0;

	h.currentBlockIndex = -1;

	h.currentNormal = vec3(0.0, 1.0, 0.0);
	h.currentColor = vec4(vec3(0.1), 1.0);

	return h;
}

// Create a new ray
Ray createRay(vec3 _rayPosition, vec3 _rayDirection)
{
	Ray r;
	r.rayPosition = _rayPosition;
	r.rayDirection = _rayDirection;
	r.oneOverRayDirection = 1.0 / _rayDirection;

	r.currentT = MAX_RAY_DISTANCE;
	r.currentEnergy = 1.0;

	r.hit = createEmptyHit();

	return r;
}

// Bounce existing ray
void bounceRay(inout Ray r)
{
	r.rayPosition = r.rayPosition + r.rayDirection * r.currentT + r.hit.currentNormal * 0.001f;
	r.rayDirection = normalize(reflect(r.rayDirection, r.hit.currentNormal));
	r.oneOverRayDirection = 1.0 / r.rayDirection;
	
	r.currentT = MAX_RAY_DISTANCE;
	
	r.hit = createEmptyHit();
}

// Squared length of a vector. Used for fast distance comparison
float squaredLength(vec3 p)
{
	return p.x*p.x + p.y*p.y + p.z*p.z;
}

void rayBoxAABBIntersection(inout Ray r, vec3 minCorner, vec3 maxCorner, 
	int loopIndex)
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

	int blockIndex = int(u_blockIndex[loopIndex]);
	int textureIndexOffset = 3 * blockIndex;

	// Horizontal sides
	if(t == t1 || t == t2)
	{
		tempUV = vec2(intersectionPoint.z, intersectionPoint.y);

		tempUV = vec2(
			(u_blockTextureRect[textureIndexOffset + 1].x + tempUV.x * u_blockTextureRect[textureIndexOffset + 1].z) * oneOverTextureSize.x,
			(u_blockTextureRect[textureIndexOffset + 1].y + tempUV.y * u_blockTextureRect[textureIndexOffset + 1].w) * oneOverTextureSize.y
		);

		r.hit.currentNormal = vec3(t == t1 ? -1.0 : 1.0, 0.0, 0.0);
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
			
			r.hit.currentNormal = vec3(0.0, 1.0, 0.0);
		}
		else
		{
			tempUV = vec2(
				(u_blockTextureRect[textureIndexOffset + 2].x + tempUV.x * u_blockTextureRect[textureIndexOffset + 2].z) * oneOverTextureSize.x,
				(u_blockTextureRect[textureIndexOffset + 2].y + tempUV.y * u_blockTextureRect[textureIndexOffset + 2].w) * oneOverTextureSize.y
			);

			r.hit.currentNormal = vec3(0.0, -1.0, 0.0);
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
		
		r.hit.currentNormal = vec3(0.0, 0.0, t == t5 ? -1.0 : 1.0);
	}

	r.currentT = t;
	r.hit.currentBlockIndex = blockIndex;
	r.hit.specular = u_blockSpecular[loopIndex];
	r.hit.currentColor = texture2D(u_textureSheet, tempUV);
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
		r.hit.specular = 0.0f;
		r.hit.currentColor = vec4(0.9, 0.0, 0.0, 1.0) * dot(normalize(vec3(1.0, 1.0, -1.0)), normal);
	}
}

void raySceneIntersection(inout Ray r)
{
	for(int i = 0; i < u_numValidBlocks; i++)
	{
		rayBoxAABBIntersection(
			r, 
			u_blocks[i] + vec3(-0.5), 
			u_blocks[i] + vec3(0.5),
			i
		);
	}
}

vec3 getSkyboxColor(vec3 rayDirection)
{
	float t = (rayDirection.y*1.2 + 1.0f) * 0.5f;
	t = clamp(t, 0.0, 1.0);

	vec3 topColor = vec3(0.85, 0.85, 0.95);
	vec3 bottomColor = vec3(0.4, 0.4, 0.7);
	vec3 skyColor = mix(bottomColor, topColor, t);
	vec3 sunColor = vec3(0.98, 0.93, 0.6);

	float isSun = 0.0;
	isSun = smoothstep(0.97, 1.0, dot(rayDirection, -LIGHT_DIR));
	isSun = pow(isSun, 3.0);

	vec3 col = mix(skyColor, sunColor, isSun);

	return col;
}

vec3 godRaysAttempt1(Ray r)
{
	float godRay = 0.0;

	Ray godRaysRay;

	if(r.currentT <= 8.0 || r.currentT >= MAX_RAY_DISTANCE)
	{
		const float samples = 20.0;
		float maxSampleDistance = clamp(r.currentT, 0.0, 8.0);
		for(float i = 0.0; i < samples; i += 1.0)
		{
			godRaysRay = createRay(r.rayPosition + r.rayDirection * (i/samples) * maxSampleDistance, -LIGHT_DIR);

			for(int i = 0; i < u_numValidBlocks; i++)
			{
				rayBoxAABBIntersection(
					godRaysRay, 
					u_blocks[i] + vec3(-0.5), 
					u_blocks[i] + vec3(0.5),
					i
				);
			}

			godRay += godRaysRay.currentT >= MAX_RAY_DISTANCE ? 1.0 : 0.0;
		}
		godRay /= samples;
	}
	else
		godRay = 1.0;

	vec3 shadowCol = pow(godRaysRay.hit.currentColor.rgb * vec3(0.99), vec3(8.0)) / pow(clamp(godRaysRay.currentT, 1.0, 5.0), 3.0);

	return mix(shadowCol, vec3(1.0), godRay);
}

vec3 godRaysAttempt2(Ray r, vec2 uv, vec3 pixelColor)
{
	float startT = texture(u_blueNoiseTexture, fract(abs(uv))).r;
	startT = fract(startT + (u_time * 500.0) * GOLDEN_RATIO_CONJUGATE);

	float fogLitPercentage = 0.0f;
	for(int i = 0; i < NUM_FOG_SAMPLES; i++)
	{
		vec3 tempPos = r.rayPosition + r.rayDirection * r.currentT * 
			((startT+float(i)) / float(NUM_FOG_SAMPLES));
		Ray tempFogRay = createRay(tempPos, -LIGHT_DIR);

		// Send ray into scene
		raySceneIntersection(tempFogRay);

		fogLitPercentage = mix(
			fogLitPercentage, 
			tempFogRay.currentT >= MAX_RAY_DISTANCE ? 1.0f : 0.0f,
			1.0f / (float(i + 1))
		);
	}

	vec3 unlitColor = vec3(0.0);
	vec3 litColor = pixelColor;

	vec3 fogColor = mix(unlitColor, litColor, fogLitPercentage);
	float absorb = exp(-r.currentT * FOG_DENSITY);

	return mix(fogColor, pixelColor, absorb);
}

void main()
{
	// UV coordinates
	// x: -1.7 to 1.7
	// y: -1 to 1
    vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0;
	uv.x *= u_resolution.x / u_resolution.y;

	// Create camera
	vec2 unitVec = vec2(1.0, 0.0);

	// Ray
	float zoom = 1.0;
	vec3 rayLookAtPosition = u_cameraPosition + 
		zoom * unitVec.yyx + 
		uv.x * unitVec.xyy + 
		uv.y * unitVec.yxy;
	vec3 rayDirection = normalize(rayLookAtPosition - u_cameraPosition);

	// Rotate ray according to camera matrix
	rayDirection = u_cameraRot * rayDirection;	

	// Let the ray position be fairly close to the camera position
	vec3 rayPosition = u_cameraPosition + rayDirection*0.01f; 


	Ray r = createRay(rayPosition, rayDirection);
	vec4 currentCol = vec4(0.0);

	// Let the ray interact with the world
	for(int currentRay = 0; currentRay < NUM_MAX_RAY_BOUNCES; currentRay++)
	{
		if(currentRay > 0)
			bounceRay(r);

		// Ray interacts with scene
		raySceneIntersection(r);

		
		// Ray didn't hit anything
		if(r.currentT >= MAX_RAY_DISTANCE)
		{
			//currentCol.rgb += getSkyboxColor(r.rayDirection) * r.currentEnergy;
			currentCol.rgb = mix(
				currentCol.rgb, 
				getSkyboxColor(r.rayDirection), 
				r.currentEnergy * (1.0 - currentCol.a)
			);

			break;
		}
		
		// God rays attempt 1
		//currentCol *= r.currentColor * vec4(godRaysAttempt1(r), 1.0);

		// God rays attempt 2
		#if GOD_RAYS_ENABLED
			r.hit.currentColor.rgb = godRaysAttempt2(r, (uv + vec2(3.0, 3.0)) * 3.12f, r.hit.currentColor.rgb);
		#endif

		// Apply color
		currentCol += r.hit.currentColor * r.currentEnergy;
		
		// Lose energy
		r.currentEnergy *= r.hit.specular;

		// Recursion only if the ray still has energy left
		if(r.currentEnergy <= 0.0f)
			break;
	}

	gl_FragColor = vec4(currentCol.rgb, 1.0);
}
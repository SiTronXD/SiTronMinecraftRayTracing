#version 130

// Mode 2 paper reference: 
// Dammertz, Sewtz, Hanika, Lensch
// (2010)
// 'Edge-avoiding À-Trous wavelet transform for fast global illumination filtering'
// 'Proceedings of the Conference on High Performance Graphics'
// June
// ACM Digital Library

// Mode 3 references:
// https://en.wikipedia.org/wiki/Gaussian_blur
// Shadertoy by "BrutPitt": https://www.shadertoy.com/view/3dd3Wr
// It's basically 2 gaussian curves which depend on the distance to the current pixel
// and the difference in color.

// 0: Off
// 1: 3x3 kernel blur
// 2: Edge avoiding a trous wavelet transform
// 3: Smart denoise with circular gaussian kernel
#define DENOISER_MODE 3

// 2: Keeps details like hard edged shadows, but can't remove noise all too well
// 3: Removes a lot of noise, but can't keep details like hard edged shadows (unless the lightmaps are high res)

// Uniforms
uniform vec2 u_lightmapNumChunkSize;
uniform vec2 u_lightmapNumTiles;
uniform vec2 u_lightmapSize;

uniform sampler2D u_lightmapTexture;

// Constants
const float PI = acos(-1.0);

// Mode 1
const float blurKernel[9] = float[](
	1.0 / 16.0,		2.0 / 16.0,		1.0 / 16.0,
	2.0 / 16.0,		4.0 / 16.0,		2.0 / 16.0,
	1.0 / 16.0,		2.0 / 16.0,		1.0 / 16.0
);

// Mode 2
const vec2 offset[25] = vec2[](
	vec2(-2, -2), vec2(-1, -2), vec2( 0, -2), vec2( 1, -2), vec2( 2, -2),
	vec2(-2, -1), vec2(-1, -1), vec2( 0, -1), vec2( 1, -1), vec2( 2, -1),
	vec2(-2,  0), vec2(-1,  0), vec2( 0,  0), vec2( 1,  0), vec2( 2,  0),
	vec2(-2,  1), vec2(-1,  1), vec2( 0,  1), vec2( 1,  1), vec2( 2,  1),
	vec2(-2,  2), vec2(-1,  2), vec2( 0,  2), vec2( 1,  2), vec2( 2,  2)
);
const float aTrousConvolutionMask[25] = float[](
	1.0f  / 256.0f,		4.0f  / 256.0f,		6.0f  / 256.0f,		4.0f  / 256.0f,		1.0f  / 256.0f,
	4.0f  / 256.0f,		16.0f / 256.0f,		24.0f / 256.0f,		16.0f / 256.0f,		4.0f  / 256.0f,
	6.0f  / 256.0f,		24.0f / 256.0f,		36.0f / 256.0f,		24.0f / 256.0f,		6.0f  / 256.0f,
	4.0f  / 256.0f,		16.0f / 256.0f,		24.0f / 256.0f,		16.0f / 256.0f,		4.0f  / 256.0f,
	1.0f  / 256.0f,		4.0f  / 256.0f,		6.0f  / 256.0f,		4.0f  / 256.0f,		1.0f  / 256.0f
);

// Clamps uv within surface
vec2 getUVFromClampedUV(vec2 uv, vec2 currentUV, vec4 mainCol, vec2 pixelSize)
{
	// Clamp position to be within the current plane
	vec4 clampArea = vec4(
		floor(uv * u_lightmapNumTiles) / u_lightmapNumTiles,
		(floor(uv * u_lightmapNumTiles) + vec2(1.0)) / u_lightmapNumTiles - pixelSize
	);
	currentUV = clamp(currentUV, clampArea.xy, clampArea.zw);

	// Get color
	vec4 foundCol = texture2D(u_lightmapTexture, currentUV);
			
	// Coordinates have different normals,
	// and the position should be clamped within the block
	if(abs(foundCol.a - mainCol.a) > 0.1)
	{
		clampArea = vec4(
			floor(uv * u_lightmapNumTiles * u_lightmapNumChunkSize) / (u_lightmapNumTiles*u_lightmapNumChunkSize),
			(floor(uv * u_lightmapNumTiles * u_lightmapNumChunkSize) + vec2(1.0)) / (u_lightmapNumTiles*u_lightmapNumChunkSize) - pixelSize
		);
		currentUV = clamp(currentUV, clampArea.xy, clampArea.zw);
	}

	return currentUV;
}

// Color from clamped uv within surface
vec4 getColorFromClampedUV(vec2 uv, vec2 currentUV, vec4 mainCol, vec2 pixelSize)
{
	vec2 clampedUV = getUVFromClampedUV(uv, currentUV, mainCol, pixelSize);
	vec4 foundCol = texture2D(u_lightmapTexture, clampedUV);

	return foundCol;
}

// Mode 3
vec3 gaussianDenoise(vec2 uv, vec2 pixelSize)
{
	const float sigma = 5.0;
	const float radiusScale = 2.0;
	const float threshold = 100.0;

    float radius = round(radiusScale * sigma);
    float radiusSquared = radius * radius;
    

	// Exponents and bases for gaussian curves

	// 2D 
	float gauss_positionExponent = -1.0 / (2.0 * sigma * sigma);
	float gauss_positionBase = 1.0 / (2.0 * sqrt(PI) * sigma * sigma);

	// 1D
	float gauss_colorExponent = -1.0 / (2.0 * threshold * threshold);
	float gauss_colorBase = 1.0 / (sqrt(2.0 * PI * threshold * threshold));


	// The main col needs to be vec4, since the alpha channel contains the "normal"
    vec4 mainCol = texture(u_lightmapTexture, uv);
    
    float sumWeights = 0.0;
    vec3 sumCol = vec3(0.0);


	// Start sampling
    for(float x=-radius; x <= radius; x++) {

        float tempYRadius = ceil(sqrt(radiusSquared - x * x));

        for(float y = -tempYRadius; y <= tempYRadius; y++) {
			
			vec2 deltaPos = vec2(x,y);

			// Create temporary uv
			vec2 tempUV = uv + deltaPos * pixelSize;
			vec2 clampUV = getUVFromClampedUV(uv, tempUV, mainCol, pixelSize);

			vec2 t = tempUV - clampUV;

			// The uvs were changed, because the previous uvs were outside the surface plane
            if(dot(t, t) > 0.0)
				continue;

			// Circular gaussian kernel
            float circularKernel = exp(dot(deltaPos, deltaPos) * gauss_positionExponent) * gauss_positionBase; 

			// Calculate difference in color
			vec3 currentCol = texture(u_lightmapTexture, tempUV).rgb;
            vec3 deltaCol = currentCol - mainCol.rgb;

			// Color weight
			float colWeight = exp(dot(deltaCol, deltaCol) * gauss_colorExponent) * gauss_colorBase;

			// Final weight
            float weight = colWeight * circularKernel;
            
			// Sum
            sumCol += weight * currentCol;
            sumWeights += weight;
        }
    }

    return sumCol / sumWeights;
}

// Mode 2
vec3 aTrousDenoise(vec2 uv, vec2 pixelSize)
{
	const float colPhi = 1.0f;
	const float norPhi = 10.0f;

	vec3 sum = vec3(0.0f);

	// Get color and "normal"
	vec4 c = texture2D(u_lightmapTexture, uv);
	vec3 colOrg = c.xyz;
	vec3 norOrg = c.aaa;

	float sum_w = 0.0f;
	for(int i = 0; i < 25; i++)
	{
		vec2 tempUV = uv + offset[i] * pixelSize;
		vec4 currentCol = getColorFromClampedUV(uv, tempUV, c, pixelSize);

		// Color
		vec3 colTmp = currentCol.xyz;
		vec3 t = colOrg - colTmp;
		float dist = dot(t, t);
		float c_w = min(exp(-dist/colPhi), 1.0);

		// Normal (would have a bigger impact if the lightmaps didn't lay on the same plane, but this
		// weight is still necessary since the normal could point in opposite directions between 
		// neighboring tiles sharing the same texture)
		vec3 norTmp = currentCol.aaa;
		t = norOrg - norTmp;
		dist = dot(t, t);
		float n_w = min(exp(-dist/norPhi), 1.0);

		// (The position buffer is being completely ignored)

		// Weight
		float weight = c_w * n_w;
		float w = weight * aTrousConvolutionMask[i];
		sum += colTmp * w;
		sum_w += w;
	}

	return sum / sum_w;
}

// Mode 1
vec3 blur(vec2 uv, vec2 pixelSize)
{
	vec3 sum = vec3(0.0f);

	vec3 texCols[9];

	// The main pixel's position and tile map index
	vec4 mainCol = texture2D(u_lightmapTexture, uv);

	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			int index = (x+1) + (y+1)*3;

			// Current pixel's position
			vec2 currentUV = uv + vec2(x, y) * pixelSize;

			// Get color within bounds
			vec3 col = getColorFromClampedUV(uv, currentUV, mainCol, pixelSize).rgb;

			texCols[index] = col;
		}
	}

	// Blur by adding surrounding colors
	for(int i = 0; i < 9; i++)
		sum += texCols[i] * blurKernel[i];

	return sum;
}

void main()
{
	// All modes needs these variables
	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;
	vec3 col = vec3(0.0f);


	// Denoiser modes
	#if (DENOISER_MODE == 3)
	
	col = gaussianDenoise(uv, pixelSize);

	#elif (DENOISER_MODE == 2)

	col = aTrousDenoise(uv, pixelSize);

	#elif (DENOISER_MODE == 1)

	col = blur(uv, pixelSize);

	#elif (DENOISER_MODE == 0)

	col = texture2D(u_lightmapTexture, uv).rgb;

	#endif


	gl_FragColor = vec4(col, 1.0);
}
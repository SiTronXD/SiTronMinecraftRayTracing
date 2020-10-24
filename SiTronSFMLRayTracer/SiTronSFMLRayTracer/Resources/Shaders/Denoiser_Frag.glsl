#version 130

// 0: Off
// 1: Kernel blur
// 2: Edge detection with a trous wavelet transform
// 3: Smart denoise with circular gaussian kernel
#define DENOISER_MODE 3
// 2: Keeps details like hard edged shadows, but can't remove noise that well
// 3: Removes a lot of noise, but can't keep details like hard edged shadows


// Uniforms
uniform vec2 u_lightmapNumChunkSize;
uniform vec2 u_lightmapNumTiles;
uniform vec2 u_lightmapSize;

uniform sampler2D u_lightmapTexture;


// Mode 1
float blurKernel[9] = float[](
	1.0 / 16.0,		2.0 / 16.0,		1.0 / 16.0,
	2.0 / 16.0,		4.0 / 16.0,		2.0 / 16.0,
	1.0 / 16.0,		2.0 / 16.0,		1.0 / 16.0
);

// Mode 2
vec2 offset[25] = vec2[](
	vec2(-2, -2), vec2(-1, -2), vec2( 0, -2), vec2( 1, -2), vec2( 2, -2),
	vec2(-2, -1), vec2(-1, -1), vec2( 0, -1), vec2( 1, -1), vec2( 2, -1),
	vec2(-2,  0), vec2(-1,  0), vec2( 0,  0), vec2( 1,  0), vec2( 2,  0),
	vec2(-2,  1), vec2(-1,  1), vec2( 0,  1), vec2( 1,  1), vec2( 2,  1),
	vec2(-2,  2), vec2(-1,  2), vec2( 0,  2), vec2( 1,  2), vec2( 2,  2)
);
float aTrousConvolutionMask[25] = float[](
	1.0f  / 256.0f,		4.0f  / 256.0f,		6.0f  / 256.0f,		4.0f  / 256.0f,		1.0f  / 256.0f,
	4.0f  / 256.0f,		16.0f / 256.0f,		24.0f / 256.0f,		16.0f / 256.0f,		4.0f  / 256.0f,
	6.0f  / 256.0f,		24.0f / 256.0f,		36.0f / 256.0f,		24.0f / 256.0f,		6.0f  / 256.0f,
	4.0f  / 256.0f,		16.0f / 256.0f,		24.0f / 256.0f,		16.0f / 256.0f,		4.0f  / 256.0f,
	1.0f  / 256.0f,		4.0f  / 256.0f,		6.0f  / 256.0f,		4.0f  / 256.0f,		1.0f  / 256.0f
);

vec4 getColorFromClampedUV(vec2 uv, vec2 currentUV, vec4 mainCol, vec2 pixelSize)
{
	vec4 foundCol = vec4(0.0);

	// Clamp position to be within the current plane
	vec4 clampArea = vec4(
		floor(uv * u_lightmapNumTiles) / u_lightmapNumTiles,
		(floor(uv * u_lightmapNumTiles) + vec2(1.0)) / u_lightmapNumTiles - pixelSize
	);
	currentUV = clamp(currentUV, clampArea.xy, clampArea.zw);

	// Get color
	foundCol = texture2D(u_lightmapTexture, currentUV);
			
	// Coordinates have different normals,
	// and the position should be clamped within the block
	if(abs(foundCol.a - mainCol.a) > 0.1)
	{
		clampArea = vec4(
			floor(uv * u_lightmapNumTiles * u_lightmapNumChunkSize) / (u_lightmapNumTiles*u_lightmapNumChunkSize),
			(floor(uv * u_lightmapNumTiles * u_lightmapNumChunkSize) + vec2(1.0)) / (u_lightmapNumTiles*u_lightmapNumChunkSize) - pixelSize
		);
		currentUV = clamp(currentUV, clampArea.xy, clampArea.zw);

		// Find new color
		foundCol = texture2D(u_lightmapTexture, currentUV);
	}

	return foundCol;
}

vec2 getUVFromClampedUV(vec2 uv, vec2 currentUV, vec4 mainCol, vec2 pixelSize)
{
	vec4 foundCol = vec4(0.0);

	// Clamp position to be within the current plane
	vec4 clampArea = vec4(
		floor(uv * u_lightmapNumTiles) / u_lightmapNumTiles,
		(floor(uv * u_lightmapNumTiles) + vec2(1.0)) / u_lightmapNumTiles - pixelSize
	);
	currentUV = clamp(currentUV, clampArea.xy, clampArea.zw);

	// Get color
	foundCol = texture2D(u_lightmapTexture, currentUV);
			
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

#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
#define INV_PI 0.31830988618379067153776752674503


//  float sigma  >  0 - sigma Standard Deviation
//  float kSigma >= 0 - sigma coefficient 
//      kSigma * sigma  -->  radius of the circular kernel
//  float threshold   - edge sharpening threshold 
vec4 smartDeNoise(vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius * radius;
    
    float invSigmaQx2 = .5 / (sigma * sigma);      // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)
    
    float invThresholdSqx2 = .5 / (threshold * threshold);     // 1.0 / (sigma^2 * 2.0)
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;   // 1.0 / (sqrt(2*PI) * sigma)
    
    vec4 centrPx = texture(u_lightmapTexture,uv);
    
    float zBuff = 0.0;
    vec4 aBuff = vec4(0.0);
    vec2 size = vec2(textureSize(u_lightmapTexture, 0));
	vec2 pixelSize = 1.0 / size;
    
    for(float x=-radius; x <= radius; x++) {
        float pt = sqrt(radQ-x*x);  // pt = yRadius: have circular trend
        for(float y=-pt; y <= pt; y++) {
            vec2 d = vec2(x,y);

			vec2 tempUV = uv + d * pixelSize;
			vec2 clampUV = getUVFromClampedUV(uv, tempUV, centrPx, pixelSize);
			vec2 t = tempUV - clampUV;

            if(dot(t, t) > 0.0)
				continue;

            /*vec4 walkPx = texture(u_lightmapTexture, tempUV);
			vec4 walkPxCl = getColorFromClampedUV(uv, uv+d/size, centrPx, 1.0 / size);
			vec3 t = walkPx.rgb - walkPxCl.rgb;

			if(dot(t, t) >= 0.01)
				continue;*/


				
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI; 
			vec4 walkPx = texture(u_lightmapTexture, tempUV);
            vec4 dC = walkPx-centrPx;
            float deltaFactor = exp( -dot(dC, dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                                 
            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return aBuff/zBuff;
}

void main()
{
	#if (DENOISER_MODE == 3)
	
	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;
	vec3 col = smartDeNoise(uv, 5.0, 2.0, 100.0).rgb;

	#elif (DENOISER_MODE == 2)

	vec3 sum = vec3(0.0f);
	float colPhi = 1.0f;
	float norPhi = 10.0f;
	
	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;

	vec4 c = texture2D(u_lightmapTexture, uv);
	vec3 colOrg = c.xyz;
	vec3 norOrg = c.aaa;

	float sum_w = 0.0f;
	for(int i = 0; i < 25; i++)
	{
		vec2 tempUV = uv + offset[i] * pixelSize;
		vec4 currentCol = getColorFromClampedUV(uv, tempUV, c, pixelSize);//texture2D(u_lightmapTexture, tempUV);

		// Color
		vec3 colTmp = currentCol.xyz;
		vec3 t = colOrg - colTmp;
		float dist = dot(t, t);
		float c_w = min(exp(-dist/colPhi), 1.0);

		// Normal
		vec3 norTmp = currentCol.aaa;
		t = norOrg - norTmp;
		dist = dot(t, t);
		float n_w = min(exp(-dist/norPhi), 1.0);


		// Weight
		float weight = c_w * n_w;
		float w = weight * aTrousConvolutionMask[i];
		sum += colTmp * w;
		sum_w += w;
	}

	// Final color
	vec3 col = sum / sum_w;


	#elif (DENOISER_MODE == 1)

	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;

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

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += texCols[i] * blurKernel[i];

	#elif (DENOISER_MODE == 0)

	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;
	vec3 col = texture2D(u_lightmapTexture, uv).rgb;

	#endif

	gl_FragColor = vec4(col, 1.0);
}
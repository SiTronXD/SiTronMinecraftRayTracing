#version 130

#define ENABLED 0

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

void main()
{
	#if (ENABLED == 1)

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

	#elif (ENABLED == 0)

	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;
	vec3 col = texture2D(u_lightmapTexture, uv).rgb;

	#endif

	gl_FragColor = vec4(col, 1.0);
}
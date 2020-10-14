#version 130

#define DENOISER_ENABLED 0

// Uniforms
uniform vec2 u_lightmapSize;

uniform sampler2D u_lightmapTexture;

float blurKernel[9] = float[](
	1.0 / 16.0,		2.0 / 16.0,		1.0 / 16.0,
	2.0 / 16.0,		4.0 / 16.0,		2.0 / 16.0,
	1.0 / 16.0,		2.0 / 16.0,		1.0 / 16.0
);

void main()
{
	#if (DENOISER_ENABLED == 1)

	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;

	vec3 texCols[9];
	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			int index = (x+1) + (y+1)*3;

			texCols[index] = texture2D(u_lightmapTexture, uv + vec2(x, y) * pixelSize).rgb;
		}
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += texCols[i] * blurKernel[i];

	#else

	vec2 pixelSize = 1.0 / u_lightmapSize;
	vec2 uv = gl_FragCoord.xy * pixelSize;
	vec3 col = texture2D(u_lightmapTexture, uv).rgb;

	#endif

	gl_FragColor = vec4(col, 1.0);
}
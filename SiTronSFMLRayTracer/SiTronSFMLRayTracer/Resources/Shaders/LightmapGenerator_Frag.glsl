#version 130

// Constants
const int LIGHTMAP_SIZE = 1024;
const int NUM_MAX_BLOCKS = 256;
const int CHUNK_WIDTH_LENGTH = 8;

// Uniforms
//uniform int u_numValidBlocks;

//uniform vec3 u_blocks[NUM_MAX_BLOCKS];

void main()
{
	vec2 uv = gl_FragCoord.xy / vec2(LIGHTMAP_SIZE);
	vec2 currentPos = uv * CHUNK_WIDTH_LENGTH;

	// x = 0, y = 0 => index = 0
	vec3 col = vec3(0.0, 0.0, 0.0);

	if(uv.x > 0.5f && uv.y > 0.5f)	// x = 0.5, y = 0.5 => index = 3
		col = vec3(1.0);
	else if(uv.x > 0.5f)			// x = 0.5, y = 0.0 => index = 1
		col = vec3(1.0, 0.0, 0.0);
	else if(uv.y > 0.5f)			// x = 0.5, y = 0.5 => index = 2
		col = vec3(0.0, 1.0, 0.0);


	// x = 0, y = 0 => index = 0
	/*vec3 col = vec3(1.0) * 0.0;

	if(uv.x > 0.5f && uv.y > 0.5f)	// x = 0.5, y = 0.5 => index = 3
		col = vec3(1.0) * 1.0;
	else if(uv.x > 0.5f)			// x = 0.5, y = 0.0 => index = 1
		col = vec3(1.0) * 0.33;
	else if(uv.y > 0.5f)			// x = 0.5, y = 0.5 => index = 2
		col = vec3(1.0) * 0.66;*/

	gl_FragColor = vec4(col, 1.0);
}
#version 130

// Constants
const int LIGHTMAP_SIZE = 1024;
const int NUM_MAX_BLOCKS = 256;
const int CHUNK_WIDTH_LENGTH = 8;

// Uniforms
uniform int u_numValidBlocks;

uniform vec3 u_blocks[NUM_MAX_BLOCKS];

void main()
{
	vec2 uv = gl_FragCoord.xy / vec2(LIGHTMAP_SIZE);
	vec2 currentPos = uv * CHUNK_WIDTH_LENGTH;
	vec3 col = vec3(uv, 0.0);

	for(int i = 0; i < u_numValidBlocks; i++)
	{
		if(	currentPos.x >= (u_blocks[i].x) && 
			currentPos.x < (u_blocks[i].x + 1.0f) && 
			currentPos.y >= (u_blocks[i].z) && 
			currentPos.y < (u_blocks[i].z + 1.0f))
		{
			if(u_blocks[i].y >= -1)
				col = vec3(1.0);
		}
	}

	gl_FragColor = vec4(col, 1.0);
}
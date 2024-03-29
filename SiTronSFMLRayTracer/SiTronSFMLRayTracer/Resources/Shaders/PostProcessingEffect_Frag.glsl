#version 130

uniform sampler2D u_mainTexture;

uniform vec2 u_resolution;

vec3 chromaticAbberation(vec3 col)
{
	col.g = texture2D(u_mainTexture, gl_FragCoord.xy / u_resolution + vec2(0.01, 0.01)).g;
	col.b = texture2D(u_mainTexture, gl_FragCoord.xy / u_resolution - vec2(0.01, 0.01)).b;

	return col;
}

void main()
{
	vec3 col = texture2D(u_mainTexture, gl_FragCoord.xy / u_resolution).rgb;

	// Chromatic aberration
	//col = chromaticAbberation(col);

	gl_FragColor = vec4(col, 1.0);
}
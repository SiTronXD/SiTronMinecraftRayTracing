attribute vec4 a_position;
attribute vec2 a_texCoord0;

varying vec2 v_uvCoords;
varying vec2 v_resolution;

uniform vec2 resolution;
uniform mat4 viewProjectionMatrix;

void main()
{
	v_uvCoords = vec2(a_texCoord0.x, 1.0 - a_texCoord0.y);
	v_resolution = resolution;

    gl_Position = viewProjectionMatrix * a_position;
} 
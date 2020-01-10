attribute vec4 a_position;
attribute vec2 a_texCoord0;

varying vec2 uvCoords;

uniform mat4 viewProjectionMatrix;

void main()
{
	uvCoords = vec2(a_texCoord0.x, 1.0 - a_texCoord0.y);

    gl_Position = viewProjectionMatrix * a_position;
} 
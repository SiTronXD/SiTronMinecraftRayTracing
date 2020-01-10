#ifdef GL_ES
precision highp float;
#endif

varying vec2 uvCoords;

void main()
{
    gl_FragColor = vec4(uvCoords, 0.0, 1.0);
}
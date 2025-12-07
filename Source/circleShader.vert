#version 330 core
layout(location = 0) in vec2 aPos;

uniform vec2 uPosition;
uniform float uScale;

void main()
{
    vec2 pos = aPos * uScale + uPosition;
    gl_Position = vec4(pos, 0.0, 1.0);
}

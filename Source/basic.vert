#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform mat4 uProj;
uniform vec2 uTranslate;
uniform vec2 uScale;
uniform float uRotation; // in radians

out vec2 vTex;

void main()
{
    // apply scale, rotate, translate in 2D
    mat2 rot = mat2(cos(uRotation), -sin(uRotation),
                    sin(uRotation),  cos(uRotation));
    vec2 pos = rot * (aPos * uScale) + uTranslate;
    gl_Position = uProj * vec4(pos, 0.0, 1.0);
    vTex = aTex;
}

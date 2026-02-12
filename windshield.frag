#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoord);

    // providnost šoferšajbne
    FragColor = vec4(color.rgb, 0.35);
}

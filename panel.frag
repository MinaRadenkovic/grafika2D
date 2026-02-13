#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

void main()
{
    vec4 color = texture(tex, TexCoord);
    FragColor = vec4(color.rgb, 1.0);
}

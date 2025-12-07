#version 330 core
in vec2 vTex;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor; // multiply color (r,g,b,a)
uniform int uUseTexture; // 0 = no texture, 1 = use texture

void main()
{
    if(uUseTexture == 1)
    {
        vec4 tex = texture(uTexture, vTex);
        FragColor = tex * uColor;
    }
    else
    {
        FragColor = uColor;
    }
    // discard fully transparent fragments optionally
    if(FragColor.a <= 0.01) discard;
}

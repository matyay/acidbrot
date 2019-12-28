#version 130
in vec2 v_TexCoord;

uniform sampler2D texture;

out vec4 o_Color;

void main(void) {
    o_Color = texture2D(texture, v_TexCoord);
}


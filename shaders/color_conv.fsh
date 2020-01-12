#version 130
precision highp float;

in vec2 v_TexCoord;

uniform sampler2D texture;
uniform mat3      matConv;

out vec4 o_Color;

// ============================================================================

void main(void) {

    // Sample the texture
    vec4 colorInp = texture2D(texture, v_TexCoord);

    // Convert color, leave alpha intact.
    vec3 colorOut = matConv * colorInp.rgb + vec3(0.0, 0.5, 0.5);
    o_Color = vec4(colorOut, colorInp.a);
}

#version 330
precision highp float;

in vec2 v_TexCoord;

uniform sampler2D texture;
uniform mat3      matConv;

layout(location = 0) out vec4 o_Color0;
layout(location = 1) out vec4 o_Color1;
layout(location = 2) out vec4 o_Color2;
layout(location = 3) out vec4 o_ColorA;

// ============================================================================

void main(void) {

    // Sample the texture
    vec4 colorInp = texture2D(texture, v_TexCoord);

    // Convert color, leave alpha intact.
    vec3 colorOut = matConv * colorInp.rgb + vec3(0.0, 0.5, 0.5);

    // Split it among render targets
    o_Color0 = vec4(colorOut.r, colorOut.r, colorOut.r, colorInp.a);
    o_Color1 = vec4(colorOut.g, colorOut.g, colorOut.g, colorInp.a);
    o_Color2 = vec4(colorOut.b, colorOut.b, colorOut.b, colorInp.a);
    o_ColorA = vec4(colorInp.a, colorInp.a, colorInp.a, colorInp.a);
}

#version 130
precision highp float;

varying vec2 v_TexCoord;

uniform sampler2D texture;

uniform vec2  filterOffsets [TAPS];
uniform float filterWeights [TAPS];

out vec4 o_Color;

void main(void) {

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i=0; i<TAPS; ++i) {
        color += texture2D(texture, v_TexCoord + filterOffsets[i]) * filterWeights[i];
    }

    o_Color = vec4(abs(color.r), abs(color.g), abs(color.b), color.a);
}


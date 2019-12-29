#version 130
precision highp float;

#include "iter.fsh"

in vec2 v_TexCoord;

uniform sampler2D fractal;

uniform int   filterTaps;
uniform vec2  filterOffsets [MAX_TAPS];
uniform float filterWeights [MAX_TAPS];

out vec4 o_Color;

void main(void) {

    float sum = 0.0;

    for (int i=0; i<filterTaps; ++i) {
        vec4  pel = texture2D(fractal, v_TexCoord + filterOffsets[i]);
        float w   = filterWeights[i];

        if (pel.a > 0.01) {
            float n = decode_iter(pel.rgb);
            sum += w * n;
        }
    }

    if (sum != 0.0) {
        o_Color = vec4(encode_iter(sum), 1.0);
    }
    else {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
    }
}


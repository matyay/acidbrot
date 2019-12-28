#version 130
precision highp float;

#include "iter.fsh"

in vec2 v_TexCoord;

uniform sampler2D fractal;

#define FILTER_N 9
uniform vec2  filterOffsets [FILTER_N];
uniform float filterWeights [FILTER_N];

out vec4 o_Color;

void main(void) {

    float sum  = 0.0;
    float wsum = 0.0;

    for (int i=0; i<FILTER_N; ++i) {
        vec4  pel = texture2D(fractal, v_TexCoord + filterOffsets[i]);
        float w   = filterWeights[i];

        if (pel.a > 0.01) {
            float n = decode_iter(pel.rgb);
            sum    += w * n;
            wsum   += w;
        }
    }

    if (wsum != 0.0) {
        sum    /= wsum;
        o_Color = vec4(encode_iter(sum), 1.0);
    }
    else {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
    }
}


#version 130
precision highp float;

varying vec2 v_TexCoord;

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
            float n = pel.r * 255.0 + pel.g;
            sum    += w * n;
            wsum   += w;
        }
    }

    if (wsum != 0.0) {
        sum    /= wsum;
        float p = floor(sum);
        float q = sum - p;
        o_Color = vec4(p / 255.0, q, 0.0, 1.0);
    }
    else {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
    }
}


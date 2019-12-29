#version 130
precision highp float;

#include "iter.fsh"

in vec2 v_TexCoord;

uniform sampler2D texture;

uniform int   filterTaps;
uniform vec2  filterOffsets [MAX_TAPS];
uniform float filterWeights [MAX_TAPS];

out vec4 o_Color;

void main(void) {

    float nsum = 0.0;
    float asum = 0.0;

    for (int i=0; i<filterTaps; ++i) {
        vec4 pel = texture2D(texture, v_TexCoord + filterOffsets[i]);

        if (pel.a < 0.01) {
            o_Color = vec4(0.0, 0.0, 0.0, 0.0);
            return;
        }

        float n = decode_iter(pel.rgb);

        nsum += n * filterWeights[i];
        asum += pel.a;
    }

    nsum  = sqrt(abs(nsum));
    asum /= float(filterTaps);

    o_Color = vec4(nsum, 0.0, 0.0, asum);
}


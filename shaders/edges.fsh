#version 130
precision highp float;

#include "iter.fsh"

in vec2 v_TexCoord;

uniform sampler2D texture;

uniform vec2  filterOffsets [TAPS];
uniform float filterWeights [TAPS];

out vec4 o_Color;

void main(void) {

    float nsum = 0.0;
    float asum = 0.0;

    for (int i=0; i<TAPS; ++i) {
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
    asum /= float(TAPS);

    o_Color = vec4(nsum, 0.0, 0.0, asum);
}


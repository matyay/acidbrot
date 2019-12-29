#version 130
precision highp float;

#include "iter.fsh"

in vec2 v_TexCoord;

uniform sampler2D fractalColor;
uniform sampler2D fractalIter;

uniform int   filterTaps;
uniform vec2  filterOffsets [MAX_TAPS];
uniform float filterWeights [MAX_TAPS];

out vec4 o_Color;

void main(void) {

    // Filter the iteration count
    float nsum = 0.0;

    for (int i=0; i<filterTaps; ++i) {
        vec3  iter = texture2D(fractalIter, v_TexCoord + filterOffsets[i]).rgb;
        float w    = filterWeights[i];

        nsum += w * decode_iter(iter);
    }

    // Adjust
    nsum = abs(nsum);
    nsum = clamp(nsum, 0.0, 1.0);
    nsum = sqrt(nsum);

    // Sample the color and apply it
    vec3 color = texture2D(fractalColor, v_TexCoord).rgb;

    o_Color = vec4(nsum * color, 1.0);
}


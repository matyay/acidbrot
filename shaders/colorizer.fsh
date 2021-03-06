#version 130
precision highp float;

#include "iter.fsh"

in vec2 v_TexCoord;

uniform sampler2D fractal;
uniform sampler2D colormap;
uniform float     colormapPos;

uniform float     colorExp;
uniform float     colorCycles;

out vec4 o_Color;

void main(void) {

    // Decode fractional iteration count
    vec4  f = texture2D(fractal, v_TexCoord);
    float n = decode_iter(f.rgb);

    // Skip pixels belonging to the fractal set
    if (f.a < 0.01) {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Color mapping
    float e = min(0.0, -colorExp * n / 50.0);
    float m = (1.0 - exp(e)) * colorCycles;
    o_Color = vec4(texture2D(colormap, vec2(m, colormapPos)).rgb, f.a);
}


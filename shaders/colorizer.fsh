#version 130
precision highp float;

varying vec2 v_TexCoord;

uniform sampler2D fractal;
uniform sampler2D colormap;
uniform float     colormapPos;

out vec4 o_Color;

void main(void) {

    // Get the fractal data
    vec4 f = texture2D(fractal, v_TexCoord);

    // Decode the fractal iteration count
    float n = f.r * 255.0 + f.g;

    // Skip pixels belonging to the fractal set
    if (f.a < 0.01) {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Color mapping
    float m = n / 50.0;
    o_Color = vec4(texture2D(colormap, vec2(m, colormapPos)).rgb, f.a);
}


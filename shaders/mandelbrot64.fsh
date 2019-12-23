#version 450
#extension GL_ARB_gpu_shader_fp64 : enable

precision highp float;

varying vec2 texcoord;

uniform sampler2D colormap;
uniform float colormapPos;

uniform dvec2  fractalPos;
uniform double fractalScale;

out vec4 outColor;

void main(void) {

    const int    MAX_ITER = 400;
    const double B = 2.0;

    // Initialize
    dvec2 z = dvec2(0.0, 0.0);
    float n = 0.0;

    // Scale and shift pixel position
    dvec2 c = dvec2(texcoord.x, texcoord.y);
    c  = c / fractalScale;
    c += fractalPos;

    // Evaluation
    for (int i=0; i<MAX_ITER; ++i) {

        double xx = z.x*z.x;
        double yy = z.y*z.y;

        z = dvec2(xx - yy, 2.0 * z.x*z.y) + c;

        if ((xx + yy) > B*B) {
            break;
        }

        n += 1.0;
    }

    // Max.
    if (n == float(MAX_ITER)) {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Smoothing
    n -= log(log(float(length(z)))/log(float(B)))/log(2.0);

    // Color mapping
    float m = n / float(MAX_ITER / 4);
    float a = n / float(MAX_ITER);
    outColor = vec4(texture2D(colormap, vec2(m, colormapPos)).rgb, a);
}


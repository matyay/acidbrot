#version 130
precision highp float;

varying vec2 v_TexCoord;

uniform vec2  fractalPos;
uniform float fractalScale;

out vec4 o_Color;

void main(void) {

    const int    MAX_ITER = 255;
    const float  B = 256.0;

    // Initialize
    vec2  z = vec2(0.0, 0.0);
    float n = 0.0;

    // Scale and shift pixel position
    vec2 c = vec2(v_TexCoord.x, v_TexCoord.y);
    c  = c / fractalScale;
    c += fractalPos;

    // Evaluation
    for (int i=0; i<MAX_ITER; ++i) {

        float xx = z.x * z.x;
        float yy = z.y * z.y;

        z = vec2(xx - yy, 2.0 * z.x*z.y) + c;

        if ((xx + yy) > B*B) {
            break;
        }

        n += 1.0;
    }

    // Max.
    if (n >= float(MAX_ITER)) {
        o_Color = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Smoothing
    //n -= log(log(float(length(z)))/log(float(B)))/log(2.0);
    n -= log2(log2(dot(z, z))) - 4.0;
    n  = clamp(n, 0.0, float(MAX_ITER));

    // Store iteration count
    float m = mod(n / 4.0, 1.0);
    o_Color = vec4(m, m, m, 1.0);
}


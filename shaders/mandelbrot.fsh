precision highp float;

varying vec2 v_TexCoord;

uniform VEC2 fractalPos;
uniform REAL fractalScale;

out vec4 o_Color;

void main(void) {

    const int  MAX_ITER = 256;
    const REAL B = 256.0;

    // Initialize
    VEC2  z = VEC2(0.0, 0.0);
    float n = 0.0;

    // Scale and shift pixel position
    VEC2 c = VEC2(v_TexCoord.x, v_TexCoord.y);
    c  = c / fractalScale;
    c += fractalPos;

    // Evaluation
    for (int i=0; i<MAX_ITER; ++i) {

        REAL xx = z.x * z.x;
        REAL yy = z.y * z.y;

        z = VEC2(xx - yy, 2.0 * z.x*z.y) + c;

        if ((xx + yy) > B*B) {
            break;
        }

        n += 1.0;
    }

    // Iteration limit reached.
    if (n >= float(MAX_ITER)) {
        o_Color = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // Smoothing
    n -= log2(log2(dot(z, z))) - 4.0;
    n  = clamp(n, 0.0, float(MAX_ITER));

    // Store iteration count
    //float m = mod(n / float(MAX_ITER / 4), 1.0);
    float m = n / float(MAX_ITER);
    o_Color = vec4(m, m, m, 1.0);
}


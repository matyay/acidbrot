precision highp float;

varying vec2 v_TexCoord;

uniform VEC2 fractalPos;
uniform REAL fractalScale;
uniform REAL fractalAngle;

out vec4 o_Color;

void main(void) {

    const int  MAX_ITER = 256;
    const REAL B = 256.0;

    // Initialize
    VEC2  z = VEC2(0.0, 0.0);
    float n = 0.0;

    // Get the initial position
    VEC2 c = VEC2(v_TexCoord.x, v_TexCoord.y);

    mat2 rot;
    rot[0] = VEC2( cos(fractalAngle), sin(fractalAngle));
    rot[1] = VEC2(-sin(fractalAngle), cos(fractalAngle));

    c  = rot * c;
    c /= fractalScale;
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
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Smoothing
    n -= log2(log2(dot(z, z))) - 4.0;
    n  = clamp(n, 0.0, float(MAX_ITER));

    // Store iteration count
    float p = floor(n);
    float q = n - p;
    o_Color = vec4(p / 255.0, q, 0.0, 1.0);
}

// re  xxx - xyy - 2xyy
// im 2xxy + xxy -  yyy

precision highp float;

varying vec2 v_TexCoord;

uniform VEC2  fractalPosition;
uniform float fractalRotation;
uniform REAL  fractalScale;
uniform vec2  fractalCoeff;

out vec4 o_Color;

void main(void) {

    const int  MAX_ITER = 256;
    const REAL B = 256.0;

    // Rotation matrix. FIXME: pass values of sin and cos as uniforms.
    MAT2 rot;
    rot[0] = VEC2( cos(fractalRotation), sin(fractalRotation));
    rot[1] = VEC2(-sin(fractalRotation), cos(fractalRotation));

    // Compute coordinates of the point on the complex plane
    VEC2 pos = VEC2(v_TexCoord.x, v_TexCoord.y);
    pos  = rot * pos;
    pos /= fractalScale;
    pos += fractalPosition;

    // Initialize
    float n = 0.0;

#ifdef MANDELBROT
    VEC2  z = VEC2(0.0, 0.0);
    VEC2  c = pos;
#endif

#ifdef JULIA
    VEC2  z = pos;
    VEC2  c = VEC2(fractalCoeff.x, fractalCoeff.y);
#endif

    // Evaluate
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

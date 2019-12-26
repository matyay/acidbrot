#define EXPONENT 2
precision highp float;

#include "iter.fsh"

varying vec2 v_TexCoord;

uniform VEC2  fractalPosition;
uniform float fractalRotation;
uniform REAL  fractalScale;
uniform vec2  fractalCoeff;

out vec4 o_Color;

void main(void) {

    const int  MAX_ITER = 256;
    const REAL B = 10.0;

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

        if ((xx + yy) > B*B) {
            break;
        }

#if   (EXPONENT == 2)
        z = VEC2(xx - yy, 2.0 * z.x*z.y) + c;
#elif (EXPONENT == 3)
        z = VEC2(z.x*(xx - 3.0*yy), z.y*(3.0*xx - yy)) + c;
#elif (EXPONENT == 4)
        z = VEC2(xx*(xx-3.0*yy) - yy*(3.0*xx-yy), 4.0*z.x*z.y*(xx - yy) ) + c;
#endif

        n += 1.0;
    }

    // Iteration limit reached.
    if (n >= float(MAX_ITER)) {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Smoothing
    n -= log(log(length(z)) / log(B)) / log(EXPONENT);
    n  = clamp(n, 0.0, float(MAX_ITER));

    // Store iteration count
    o_Color = vec4(encode_iter(n), 1.0);
}

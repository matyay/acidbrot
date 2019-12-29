#version 130
precision highp float;

// ============================================================================

// Inputs
in vec2 v_TexCoord;

// Uniforms
uniform sampler2D color;
uniform sampler3D noise;

uniform float time;
uniform float weaveAmpl;

// Outputs
out vec4 o_Color;

// ============================================================================

void main (void) {

    // Noise position
    float z = time;

    // Get the noise
    vec2 ofs = vec2(
        texture(noise, vec3(v_TexCoord, z + 0.00)).r,
        texture(noise, vec3(v_TexCoord, z + 0.50)).r
        );

    // Scale displacement
    ofs = (ofs - vec2(0.5, 0.5)) * 2.0 * 0.01 * weaveAmpl;

    // Sample the texture with offset
    vec3 pel = texture2D(color, v_TexCoord + ofs).rgb;

    o_Color = vec4(pel, 1.0);
}


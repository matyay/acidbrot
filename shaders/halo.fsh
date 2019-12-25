#version 130
precision highp float;

varying vec2 v_TexCoord;

uniform sampler2D texture;
uniform sampler2D haloMask;

out vec4 o_Color;

#define STEPS 15

void main(void) {

    // Sample the texture
    vec4  color  = texture2D(texture,  v_TexCoord);

    vec2  origin = v_TexCoord - vec2(0.5, 0.5);
    vec3  halo   = vec3(0.0, 0.0, 0.0);

    float a = 1.0;
    float z = 1.0;

    for (int i=0; i<STEPS; ++i) {
        vec2 pos = z * origin + vec2(0.5, 0.5);
        vec3 pel = texture2D(haloMask, pos).r *
                   texture2D(texture,  pos).rgb;
        
        halo += pel * a;

        z *= 0.9875;
        a *= 0.9250;
    }

    halo *= 1.1f;
    halo /= STEPS;

    // Final color
    o_Color = vec4(color.rgb + halo, 1.0);
}


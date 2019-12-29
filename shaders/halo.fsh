#version 130
precision highp float;

in vec2 v_TexCoord;

uniform sampler2D texture;
uniform sampler2D haloMask;

uniform int   haloSteps;
uniform float haloStepFac;
uniform float haloAttnFac;
uniform float haloGain;

out vec4 o_Color;

void main(void) {

    // Sample the texture
    vec4  color  = texture2D(texture,  v_TexCoord);

    // Add the halo effect
    vec2  origin = v_TexCoord - vec2(0.5, 0.5);
    vec3  halo   = vec3(0.0, 0.0, 0.0);

    float a = 1.0;
    float z = 1.0;

    for (int i=0; i<haloSteps; ++i) {
        vec2 pos = z * origin + vec2(0.5, 0.5);
        vec3 pel = texture2D(haloMask, pos).rgb;
        
        halo += pel * a;

        z *= haloStepFac;
        a *= haloAttnFac;
    }

    halo *= haloGain / float(haloSteps);

    // Final color
    o_Color = vec4(color.rgb + halo, 1.0);
}


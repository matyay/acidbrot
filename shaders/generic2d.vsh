#version 330

layout(location = 0) in vec4 a_Coord;

out vec2 v_TexCoord;

void main () {
    gl_Position = vec4(a_Coord.xy, 0.0, 1.0);
    v_TexCoord  = a_Coord.zw;
}


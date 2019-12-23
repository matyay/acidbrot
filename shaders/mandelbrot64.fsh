#version 150
#extension GL_ARB_gpu_shader_fp64 : enable

// Use 64-bit floats
#define VEC2 dvec2
#define REAL double

// Include the actual shader code
#include "mandelbrot.fsh"


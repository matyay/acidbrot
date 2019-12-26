/// Encodes fractional iteration count in RGB color
vec3 encode_iter(in float n) {
    float p = floor(n);
    float q = n - p;

    return vec3(floor(p / 256.0) / 255.0, mod(p, 256.0) / 255.0, q);
}

/// Decodes fractional iteration count
float decode_iter(in vec3 v) {
    return v.r * 256.0 * 255.0 +
           v.g * 255.0 +
           v.b;
}

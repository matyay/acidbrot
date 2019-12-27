#define DOUBLE vec2

DOUBLE dp_set (float a) {
    DOUBLE z;

    z.x = a;
    z.y = 0.0;

    return z;
}

DOUBLE dp_add (DOUBLE a, DOUBLE b) {
    float t1, t2, e;

    t1 = a.x + b.x;
    e  = t1 - a.x;
    t2 = ((b.x - e) + (a.x - (t1 - e))) + a.y + b.y;

    DOUBLE z;
    z.x = t1 + t2;
    z.y = t2 - (z.x - t1);
    return z;
}

DOUBLE dp_mul (DOUBLE a, DOUBLE b) {
    float c11, c21, c2, e, t1, t2;
    float a1, a2, b1, b2, cona, conb, split = 8193.;

    cona = a.x * split;
    conb = b.x * split;
    a1 = cona - (cona - a.x);
    b1 = conb - (conb - b.x);
    a2 = a.x - a1;
    b2 = b.x - b1;

    c11 = a.x * b.x;
    c21 = a2 * b2 + (a2 * b1 + (a1 * b2 + (a1 * b1 - c11)));

    c2 = a.x * b.y + a.y * b.x;

    t1 = c11 + c2;
    e  = t1 - c11;
    t2 = a.y * b.y + ((c2 - e) + (c11 - (t1 - e))) + c21;

    DOUBLE z;
    z.x = t1 + t2;
    z.y = t2 - (z.x - t1);
 
    return z;
}

// Compare: res = -1 if a < b
//              =  0 if a == b
//              =  1 if a > b
int dp_compare (DOUBLE a, DOUBLE b) {

    if (a.x < b.x) {
        return -1;
    }
    else if (a.x == b.x) {
        if (a.y < b.y) {
            return -1;
        }
        else if (a.y == b.y) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        return 1;
    }
}

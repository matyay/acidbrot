#ifndef COLOR_HH
#define COLOR_HH

///////////////////////////////////////////////////////////////////////////////

// Color
class Color
{
public:

    // Color data
    float    r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;

    // ................................

    // Constructors
    Color(float x, float y, float z)            : r(x),   g(y),   b(z),   a(1.0f)   {};
    Color(float x, float y, float z, float w)   : r(x),   g(y),   b(z),   a(w)      {};
    Color(const Color& c)                       : r(c.r), g(c.g), b(c.b), a(c.a)    {};
    
    Color(unsigned long ulColor) : 
        r( (float)((ulColor>>16)&0xFF) / 255.0f ), 
        g( (float)((ulColor>>8) &0xFF) / 255.0f ),
        b( (float)( ulColor     &0xFF) / 255.0f ),
        a( (float)((ulColor>>24)&0xFF) / 255.0f )
    {}
    
    // ................................    
    
    // Operators
    inline Color& operator += (Color& c)  {r+=c.r;g+=c.g;b+=c.b;a+=c.a; return *this;};
    inline Color& operator -= (Color& c)  {r-=c.r;g-=c.g;b-=c.b;a-=c.a; return *this;};
    inline Color& operator *= (Color& c)  {r*=c.r;g*=c.g;b*=c.b;a*=c.a; return *this;};
    inline Color& operator /= (Color& c)  {r/=c.r;g/=c.g;b/=c.b;a/=c.a; return *this;};
    
    inline Color& operator += (float k)    {r+=k;g+=k;b+=k;a+=k; return *this;};
    inline Color& operator -= (float k)    {r-=k;g-=k;b-=k;a-=k; return *this;};
    inline Color& operator *= (float k)    {r*=k;g*=k;b*=k;a*=k; return *this;};
    inline Color& operator /= (float k)    {r/=k;g/=k;b/=k;a/=k; return *this;};

    // ................................

    /// Inverts the color
    inline Color& invert()
    {
        r = 1.0f - r;
        g = 1.0f - g;
        b = 1.0f - b;
        return *this;
    }
    
    /// Saturates the color
    inline Color& saturate()
    {
        r = clamp(r, 0.0f, 1.0f);
        g = clamp(g, 0.0f, 1.0f);
        b = clamp(b, 0.0f, 1.0f);
        a = clamp(a, 0.0f, 1.0f);
        return *this;
    }

    /// Returns luminance of a color
    inline float getLuminance()    {return 0.299f * r + 0.587f * g + 0.114f * b;}

    // ................................

    inline Color Invert(const Color& c) {
        return Color(1.0f - c.r, 1.0f - c.g, 1.0f - c.b, c.a);
    }

    // ................................

private:

    /// Clamps a float number to given range
    static inline float clamp(float x, float a_Min, float a_Max) {
        if (x < a_Min) return a_Min;
        if (x > a_Max) return a_Max;
        return x;
    }
};

///////////////////////////////////////////////////////////////////////////////
// Operators

inline Color operator +(Color& c1, Color& c2)    {return Color(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b, c1.a+c2.a);}
inline Color operator -(Color& c1, Color& c2)    {return Color(c1.r-c2.r, c1.g-c2.g, c1.b-c2.b, c1.a-c2.a);}
inline Color operator *(Color& c1, Color& c2)    {return Color(c1.r*c2.r, c1.g*c2.g, c1.b*c2.b, c1.a*c2.a);}
inline Color operator /(Color& c1, Color& c2)    {return Color(c1.r/c2.r, c1.g/c2.g, c1.b/c2.b, c1.a/c2.a);}

inline Color operator +(Color& c, float k)       {return Color(c.r+k, c.g+k, c.b+k, c.a+k);}
inline Color operator -(Color& c, float k)       {return Color(c.r-k, c.g-k, c.b-k, c.a-k);}
inline Color operator *(Color& c, float k)       {return Color(c.r*k, c.g*k, c.b*k, c.a*k);}
inline Color operator *(float k, Color& c)       {return Color(c.r*k, c.g*k, c.b*k, c.a*k);}
inline Color operator /(Color& c, float k)       {return Color(c.r/k, c.g/k, c.b/k, c.a/k);}

#endif


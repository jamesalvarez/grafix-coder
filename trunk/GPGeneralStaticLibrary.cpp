#include "GPGeneralStaticLibrary.h"

GPGeneralStaticLibrary::GPGeneralStaticLibrary()
{
}

double GPGeneralStaticLibrary::string_to_double_fast(const unsigned char *p, unsigned int &move_on)
{
    const unsigned char* orig_p = p;

    double r = 0.0;
    bool neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        r = (r*10.0) + (*p - '0');
        ++p;
    }
    if (*p == '.') {
        double f = 0.0;
        int n = 0;
        ++p;
        while (*p >= '0' && *p <= '9') {
            f = (f*10.0) + (*p - '0');
            ++p;
            ++n;
        }
        r += f / std::pow(10.0, n);
    }
    if (*p == 'e' || *p == 'E') {
        ++p;
        bool pos_e = (*p == '+');
        ++p;
        double f = 0.0;
        ++p;
        while (*p >= '0' && *p <= '9') {
            f = (f*10.0) + (*p - '0');
            ++p;
        }
        if (!pos_e) {
            f = -f;
        }
        r *= std::pow(10,f);

    }

    if (neg) {
        r = -r;
    }
    move_on = p-orig_p-1;
    return r;
}

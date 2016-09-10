#ifndef GPGENERALSTATICLIBRARY_H
#define GPGENERALSTATICLIBRARY_H

#include <cmath>

class GPGeneralStaticLibrary {
public:
    GPGeneralStaticLibrary();
    static double string_to_double_fast(const unsigned char *p, unsigned int &move_on);
};

#endif // GPGENERALSTATICLIBRARY_H

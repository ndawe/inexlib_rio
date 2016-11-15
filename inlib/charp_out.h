// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_charp_out
#define inlib_charp_out

#include "snpf.h"
#include <string>

namespace inlib {

    class charp_out : public std::string {
        typedef std::string parent;
    public:
        charp_out(const char* a_value)
        {
            char s[512];

            if (sizeof(unsigned long) == sizeof(char*)) { //majority of cases.
                snpf(s, sizeof(s), "%lu", a_value);
                parent::operator+=(s);
            } else if (sizeof(unsigned long long) == sizeof(char*)) { //majority of cases.
                snpf(s, sizeof(s), "%llu", a_value);
                parent::operator+=(s);
            } else {
                parent::operator+=("charp_out_failed");
            }
        }
    public:
        charp_out(const charp_out& a_from): parent(a_from) {}
        charp_out& operator=(const charp_out& a_from)
        {
            parent::operator=(a_from);
            return *this;
        }
    };

}

#endif

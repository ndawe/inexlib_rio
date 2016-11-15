// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_mathd
#define inlib_mathd

namespace inlib {

    //have : static const pi = 3.1415926535897931160E0; ???

    //HEALPix lsconstants.h. Quite not the same as us.
    //const double pi=3.141592653589793238462643383279502884197;
    //const double twopi=6.283185307179586476925286766559005768394;
    //const double fourpi=12.56637061435917295385057353311801153679;
    //const double halfpi=1.570796326794896619231321691639751442099;

    inline double pi()
    {
        return 3.1415926535897931160E0;
    }
    inline double two_pi()
    {
        return 6.2831853071795862320E0;
    }
    inline double half_pi()
    {
        return 1.5707963267948965580E0;
    }

    inline double deg2rad()
    {
        return pi() / 180.0;
    }
    inline double rad2deg()
    {
        return 180.0 / pi();
    }

    // for Lib/ExpFunc.
    inline bool in_domain_all(double)
    {
        return true;
    }
    inline bool in_domain_log(double a_x)
    {
        return (a_x > 0 ? true : false);
    }
    inline bool in_domain_tan(double a_x)
    {
        int n = int(a_x / half_pi());

        if (a_x != n * half_pi()) return true;

        return (2 * int(n / 2) == n ? true : false);
    }
    inline bool in_domain_acos(double a_x)
    {
        if ((a_x < -1) || (1 < a_x)) return false;

        return true;
    }

}

//#include "power.h"

#endif

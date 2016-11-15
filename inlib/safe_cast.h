// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_safe_cast
#define inlib_safe_cast

#include "cid.h"

namespace inlib {

    template <class FROM, class TO>
    inline TO* safe_cast(FROM& a_o)
    {
        return (TO*)a_o.cast(TO::s_class());
    }

    template <class FROM, class TO>
    inline const TO* safe_cast(const FROM& a_o)
    {
        return (const TO*)a_o.cast(TO::s_class());
    }

    template <class FROM, class TO>
    inline TO* id_cast(FROM& a_o)
    {
        return (TO*)a_o.cast(TO::id_class());
    }

    template <class FROM, class TO>
    inline const TO* id_cast(const FROM& a_o)
    {
        return (const TO*)a_o.cast(TO::id_class());
    }

}

#endif

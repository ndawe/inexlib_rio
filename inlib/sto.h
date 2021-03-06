// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_sto
#define inlib_sto

#include <string>

namespace inlib {

    inline bool to(const std::string& a_string, bool& a_value, bool a_def = false)
    {
        if ((a_string == "1")
            || (a_string == "true") || (a_string == "TRUE") || (a_string == "True")
            || (a_string == "yes") || (a_string == "YES") || (a_string == "Yes")
            || (a_string == "on") || (a_string == "ON") || (a_string == "On")
           ) {
            a_value = true;
            return true;
        } else if ((a_string == "0")
                   || (a_string == "false") || (a_string == "FALSE") || (a_string == "False")
                   || (a_string == "no") || (a_string == "NO") || (a_string == "No")
                   || (a_string == "off") || (a_string == "OFF") || (a_string == "Off")
                  ) {
            a_value = false;
            return true;
        } else {
            a_value = a_def;
            return false;
        }
    }

}

#include <sstream>

namespace inlib {

    template <class T>
    inline bool to(const std::string& a_s, T& a_v, const T& a_def = T())
    {
        if (a_s.empty()) {
            a_v = a_def;    //for INLIB_STL istringstream.
            return false;
        }

        std::istringstream strm(a_s.c_str());
        strm >> a_v;

        if (strm.fail()) {
            a_v = a_def;
            return false;
        }

        return strm.eof();
    }

    template <class T>
    inline bool to(T& a_field, const std::string& a_s, bool& a_changed)
    {
        T old = a_field;

        if (!inlib::to(a_s, a_field)) {
            a_field = old;
            a_changed = false;
            return false;
        }

        a_changed = a_field == old ? false : true;
        return true;
    }

}

#endif

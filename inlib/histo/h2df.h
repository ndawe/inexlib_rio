// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_histo_h2df
#define inlib_histo_h2df

// coord is in double.
// weight is in float.

#include "h2.h"

namespace inlib {
    namespace histo {

        class h2df : public h2<double, unsigned int, unsigned int, float, float> {
            typedef h2<double, unsigned int, unsigned int, float, float> parent;
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::histo::h2df");
                return s_v;
            }
            const std::string& s_cls() const
            {
                return s_class();
            }
        public:
            h2df(const std::string& a_title,
                 unsigned int aXnumber, float aXmin, float aXmax,
                 unsigned int aYnumber, float aYmin, float aYmax)
                : parent(a_title, aXnumber, aXmin, aXmax, aYnumber, aYmin, aYmax)
            {}
            h2df(const std::string& a_title,
                 const std::vector<double>& aEdgesX,
                 const std::vector<double>& aEdgesY)
                : parent(a_title, aEdgesX, aEdgesY)
            {}

            virtual ~h2df() {}
        public:
            h2df(const h2df& a_from): parent(a_from) {}
            h2df& operator=(const h2df& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }

        private:
            static void check_instantiation()
            {
                h2df dummy("", 10, 0, 1, 10, 0, 1);
            }
        };

    }
}

#endif





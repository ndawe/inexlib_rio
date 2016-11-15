// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_histo_h1df
#define inlib_histo_h1df

// coord is in double.
// weight is in float.

#include "h1.h"

namespace inlib {
    namespace histo {

        class h1df : public h1<double, unsigned int, unsigned int, float, float> {
            typedef h1<double, unsigned int, unsigned int, float, float> parent;
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::histo::h1df");
                return s_v;
            }
            const std::string& s_cls() const
            {
                return s_class();
            }
        public:
            h1df(const std::string& a_title, unsigned int aXnumber, float aXmin, float aXmax)
                : parent(a_title, aXnumber, aXmin, aXmax) {}

            h1df(const std::string& a_title, const std::vector<double>& aEdges)
                : parent(a_title, aEdges) {}

            virtual ~h1df() {}
        public:
            h1df(const h1df& a_from): parent(a_from) {}
            h1df& operator=(const h1df& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }

        private:
            static void check_instantiation()
            {
                h1df h("", 10, 0, 1);
                h.gather_bins(5);
            }
        };

    }
}

#endif





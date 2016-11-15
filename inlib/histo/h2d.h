// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_histo_h2d
#define inlib_histo_h2d

#include "h2.h"

namespace inlib {
    namespace histo {

        class h2d : public h2<double, unsigned int, unsigned int, double, double> {
            typedef h2<double, unsigned int, unsigned int, double, double> parent;
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::histo::h2d");
                return s_v;
            }
            const std::string& s_cls() const
            {
                return s_class();
            }
        public:
            h2d(const std::string& a_title,
                unsigned int aXnumber, double aXmin, double aXmax,
                unsigned int aYnumber, double aYmin, double aYmax)
                : parent(a_title, aXnumber, aXmin, aXmax, aYnumber, aYmin, aYmax)
            {}
            h2d(const std::string& a_title,
                const std::vector<double>& aEdgesX,
                const std::vector<double>& aEdgesY)
                : parent(a_title, aEdgesX, aEdgesY)
            {}

            virtual ~h2d() {}
        public:
            h2d(const h2d& a_from): parent(a_from) {}
            h2d& operator=(const h2d& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }

        private:
            static void check_instantiation()
            {
                h2d dummy("", 10, 0, 1, 10, 0, 1);
            }
        };

    }
}

#endif





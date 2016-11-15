// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_histo_h3d
#define inlib_histo_h3d

#include "h3.h"

namespace inlib {
    namespace histo {

        class h3d : public h3<double, unsigned int, unsigned int, double, double> {
            typedef h3<double, unsigned int, unsigned int, double, double> parent;
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::histo::h3d");
                return s_v;
            }
            const std::string& s_cls() const
            {
                return s_class();
            }
        public:
            h3d(const std::string& a_title,
                unsigned int aXnumber, double aXmin, double aXmax,
                unsigned int aYnumber, double aYmin, double aYmax,
                unsigned int aZnumber, double aZmin, double aZmax)
                : parent(a_title, aXnumber, aXmin, aXmax,
                         aYnumber, aYmin, aYmax,
                         aZnumber, aZmin, aZmax)
            {}

            h3d(const std::string& a_title,
                const std::vector<double>& aEdgesX,
                const std::vector<double>& aEdgesY,
                const std::vector<double>& aEdgesZ)
                : parent(a_title, aEdgesX, aEdgesY, aEdgesZ)
            {}

            virtual ~h3d() {}
        public:
            h3d(const h3d& a_from): parent(a_from) {}
            h3d& operator=(const h3d& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }

        private:
            static void check_instantiation()
            {
                h3d dummy("", 10, 0, 1, 10, 0, 1, 10, 0, 1);
            }
        };

    }
}

#endif





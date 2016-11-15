// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_iro
#define inlib_rroot_iro

#include <string>

namespace inlib {
    namespace rroot {
        class buffer;
    }
}

#include "../cid.h"

namespace inlib {
    namespace rroot {

        class iro {
        public:
            virtual ~iro() {}
        public:
            virtual void* cast(const std::string&) const = 0; //for ObjArray
            virtual bool stream(buffer&) = 0;

            virtual const std::string& s_cls() const = 0;
            virtual iro* copy() const = 0;

            virtual void* cast(cid) const = 0; //OPTIMIZATION (geo).
        };

    }
}

#endif

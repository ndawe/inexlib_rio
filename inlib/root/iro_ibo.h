// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_iro_ibo
#define inlib_root_iro_ibo

#include <string>

namespace inlib {
    namespace root {
        class buffer;
    }
}

#include "../cid.h"

namespace inlib {
    namespace root {

        class iro_ibo {
        public:
            virtual ~iro_ibo() {}
        public:
            virtual void* cast(const std::string&) const = 0; //for ObjArray
            virtual const std::string& store_cls() const = 0;
            virtual bool stream(buffer&) const = 0;

            virtual const std::string& s_cls() const = 0;
            virtual iro* copy() const = 0;

            virtual void* cast(cid) const = 0; //OPTIMIZATION (geo).
        };

    }

    namespace wroot {
        using namespace root;
    }

    namespace rroot {
        using namespace root;
    }
}

#endif

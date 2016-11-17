// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_seek
#define inlib_root_seek

//////////////////////////////////////////////////////////////////////////
// Definition of a file pointer .                                       //
//////////////////////////////////////////////////////////////////////////

#include "../typedefs.h"

namespace inlib {
    namespace root {

        typedef inlib::int64 seek;
        typedef int seek32;

    }

    namespace wroot {
        using namespace root;
    }

    namespace rroot {
        using namespace root;
    }
}

#endif

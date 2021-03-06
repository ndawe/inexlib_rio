// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_wroot_ifile
#define inlib_wroot_ifile

#include "seek.h"
#include "../press_func.h"

namespace inlib {
    namespace wroot {
        class buffer;
    }
}

namespace inlib {
    namespace wroot {

        class ifile {
        public:
            virtual ~ifile() {}
        public:
            virtual bool verbose() const = 0;
            virtual std::ostream& out() const = 0;
            virtual bool byte_swap() const = 0;
            enum from {
                begin,
                current,
                end
            };
            virtual bool set_pos(seek = 0, from = begin) = 0;

            virtual seek END() const = 0;
            virtual void set_END(seek) = 0;

            virtual bool write_buffer(const char*, uint32) = 0;

            virtual uint32 version() const = 0;

            virtual bool synchronize() = 0;

            virtual bool ziper(char, compress_func&) const = 0;

            virtual uint32 compression() const = 0;

            virtual void compress_buffer(const buffer&, char*&, uint32&, bool&) = 0;
        };

    }
}

#endif

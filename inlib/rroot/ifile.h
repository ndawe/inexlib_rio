// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_ifile
#define inlib_rroot_ifile

#include "inlib/root/seek.h"
#include "../press_func.h"

namespace inlib {
    namespace rroot {

        class key;

        class ifile {
        public:
            virtual ~ifile() {}
        public:
            virtual const std::string& path() const = 0;

            virtual bool verbose() const = 0;
            virtual std::ostream& out() const = 0;
            virtual bool byte_swap() const = 0;
            enum from {
                begin,
                current,
                end
            };
            virtual bool set_pos(seek = 0, from = begin) = 0;
            virtual bool read_buffer(char*, uint32) = 0;

            virtual bool unziper(char, decompress_func&) const = 0;

            virtual key& sinfos_key() = 0;
        };

    }
}

#endif

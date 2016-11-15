// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_dummy
#define inlib_rroot_dummy

// dummy class with a generic streamer.
// It is used within the reading of a tree
// to create some generic object when we get
// from the file a class name which is unknown
// from the tree factory.

#include "iro.h"

#include "buffer.h"
#include "../scast.h"
#include "cids.h"

namespace inlib {
    namespace rroot {

        class dummy : public virtual iro {
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::rroot::dummy");
                return s_v;
            }
        public: //iro
            virtual void* cast(const std::string& a_class) const
            {
                if (void* p = cmp_cast<dummy>(this, a_class)) return p;

                return 0;
            }
            virtual const std::string& s_cls() const
            {
                return s_class();
            }
        public:
            static cid id_class()
            {
                return dummy_cid();
            }
            virtual void* cast(cid a_class) const
            {
                if (void* p = cmp_cast<dummy>(this, a_class)) {
                    return p;
                }

                return 0;
            }
        public:
            virtual iro* copy() const
            {
                return new dummy(*this);
            }
            virtual bool stream(buffer& a_buffer)
            {
                //the below code skips correctly the data in the file.
                uint32 startpos = a_buffer.length();
                short v;
                unsigned int s, c;

                if (!a_buffer.read_version(v, s, c)) return false;

                a_buffer.set_offset(startpos + c + sizeof(unsigned int));

                if (!a_buffer.check_byte_count(s, c, "dummy")) return false;

                return true;
            }
        public:
            dummy()
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            virtual ~dummy()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        public:
            dummy(const dummy& a_from): iro(a_from)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            dummy& operator=(const dummy&)
            {
                return *this;
            }
        };

    }
}

#endif

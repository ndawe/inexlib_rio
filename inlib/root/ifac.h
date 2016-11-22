// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_ifac
#define inlib_root_ifac

#include <string>
#include <map>

namespace inlib {
    namespace root {

        class iro_ibo;

        class ifac {
        public:
            typedef std::map<char, void*> args;
        public:
            virtual ~ifac() {}
        public:
            virtual iro_ibo* create(const std::string& a_class, const args&) = 0;
            virtual void destroy(iro_ibo*&) = 0;
        public:
            static void* find_args(const args& a_args, char a_key)
            {
                std::map<char, void*>::const_iterator it = a_args.find(a_key);

                if (it == a_args.end()) return 0;

                return (*it).second;
            }
            static char arg_branch()
            {
                return 'B';
            }
            static char arg_class()
            {
                return 'C';
            }

            static std::string* arg_class(const args& a_args)
            {
                void* p = ifac::find_args(a_args, ifac::arg_class());

                if (!p) return 0;

                return (std::string*)p;
            }
        };

    }
}

#endif

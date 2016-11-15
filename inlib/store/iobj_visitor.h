// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_store_iobj_visitor
#define inlib_store_iobj_visitor

#include "../typedefs.h"

#include <string>
#include <vector>
#include <ostream>

namespace inlib {

    class iobj_visitor {
    public:
        virtual ~iobj_visitor() {}
    public:
        virtual std::ostream& out() const = 0;

        //virtual bool begin(IStorable&) = 0;
        //virtual bool end(IStorable&) = 0;
        virtual bool visit(bool&) = 0;
        virtual bool visit(char&) = 0;
        virtual bool visit(short&) = 0;
        virtual bool visit(int&) = 0;
        virtual bool visit(unsigned int&) = 0;
        virtual bool visit(int64&) = 0;
        virtual bool visit(uint64&) = 0;
        virtual bool visit(float&) = 0;
        virtual bool visit(double&) = 0;
        virtual bool visit(std::string&) = 0;
        virtual bool visit(std::vector<bool>&) = 0;
        virtual bool visit(std::vector<char>&) = 0;
        virtual bool visit(std::vector<short>&) = 0;
        virtual bool visit(std::vector<int>&) = 0;
        virtual bool visit(std::vector<int64>&) = 0;
        virtual bool visit(std::vector<float>&) = 0;
        virtual bool visit(std::vector<double>&) = 0;
        virtual bool visit(std::vector<unsigned char>&) = 0;
        virtual bool visit(std::vector<std::string>&) = 0;
        virtual bool visit(std::vector< std::vector<double> >&) = 0;
        //virtual bool visit_double(IArray&) = 0;
    };

}

#endif

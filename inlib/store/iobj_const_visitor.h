// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_store_iobj_const_visitor
#define inlib_store_iobj_const_visitor

#include "../typedefs.h"

#include <string>
#include <vector>

namespace inlib {

    class iobj_const_visitor;

    class istorable {
    public:
        virtual ~istorable() {}
    public:
        virtual void* cast(const std::string&) const = 0;
    public:
        virtual const std::string& store_cls() const = 0;
        virtual bool visit(iobj_const_visitor&) const = 0;
        //  virtual bool read(iobj_visitor&) = 0;
    };

    class iobj_const_visitor {
    public:
        virtual ~iobj_const_visitor() {}
    public:
        typedef bool(*local_func)(const istorable&, iobj_const_visitor&);
    public:
        virtual bool begin(const istorable&, const std::string&, local_func) = 0;
        virtual bool end(const istorable&) = 0;

        virtual bool visit(const std::string&, bool) = 0;
        virtual bool visit(const std::string&, char) = 0;
        //virtual bool visit(const std::string&,unsigned char) = 0;
        virtual bool visit(const std::string&, short) = 0;
        //virtual bool visit(const std::string&,unsigned short) = 0;

        virtual bool visit(const std::string&, int) = 0;
        virtual bool visit(const std::string&, unsigned int) = 0;

        virtual bool visit(const std::string&, int64) = 0;
        virtual bool visit(const std::string&, uint64) = 0;

        virtual bool visit(const std::string&, float) = 0;
        virtual bool visit(const std::string&, double) = 0;

        virtual bool visit(const std::string&, const std::string&) = 0;
        //virtual bool visit(const std::string&,const char*) = 0;
        virtual bool visit(const std::string&, const std::vector<bool>&) = 0;
        virtual bool visit(const std::string&, const std::vector<char>&) = 0;
        virtual bool visit(const std::string&, const std::vector<short>&) = 0;
        virtual bool visit(const std::string&, const std::vector<int>&) = 0;
        virtual bool visit(const std::string&, const std::vector<int64>&) = 0;
        virtual bool visit(const std::string&, const std::vector<float>&) = 0;
        virtual bool visit(const std::string&, const std::vector<double>&) = 0;
        //virtual bool visit(const std::string&,const std::vector<unsigned char>&) = 0;
        virtual bool visit(const std::string&, const std::vector<std::string>&) = 0;
        virtual bool visit(const std::string&, const std::vector< std::vector<double> >&) = 0;

        //virtual bool visit_double(const std::string&,const IArray&) = 0;
        virtual bool visit(const std::string&, const istorable&) = 0;
    };

}


#endif

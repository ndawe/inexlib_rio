// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_ntuple_binding
#define inlib_ntuple_binding

// a little class to capture column binding parameters
// when reading an ntuple.

#include <string>
#include <vector>
#include "forit.h"

namespace inlib {

    class column_binding {
    public:
        column_binding(const std::string& a_name, void* a_user_obj)
            : m_name(a_name)
            , m_user_obj(a_user_obj) //WARNING : not owner.
        {}
        virtual ~column_binding() {}
    public:
        column_binding(const column_binding& a_from)
            : m_name(a_from.m_name)
            , m_user_obj(a_from.m_user_obj)
        {}
        column_binding& operator=(const column_binding& a_from)
        {
            if (&a_from == this) return *this;

            m_name = a_from.m_name;
            m_user_obj = a_from.m_user_obj;
            return *this;
        }
    public:
        const std::string& name() const
        {
            return m_name;
        }
        void* user_obj() const
        {
            return m_user_obj;
        }
    protected:
        std::string m_name;
        void* m_user_obj;
    };

    class ntuple_binding {
    public:
        ntuple_binding()
        {}
        virtual ~ntuple_binding() {}
    public:
        ntuple_binding(const ntuple_binding& a_from)
            : m_columns(a_from.m_columns)
        {}
        ntuple_binding& operator=(const ntuple_binding& a_from)
        {
            m_columns = a_from.m_columns;
            return *this;
        }
    public:
        template <class T>
        void add_column(const std::string& a_name, T& a_user_var)
        {
            m_columns.push_back(column_binding(a_name, (void*)&a_user_var));
        }

        const std::vector<column_binding>& columns() const
        {
            return m_columns;
        }

        template <class T>
        T* find_variable(const std::string& a_name) const
        {
            inlib_vforcit(column_binding, m_columns, it) {
                if ((*it).name() == a_name) return (T*)((*it).user_obj());
            }
            return 0;
        }
    protected:
        std::vector<column_binding> m_columns;
    };

}

#endif

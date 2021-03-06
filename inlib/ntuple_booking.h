// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_ntuple_booking
#define inlib_ntuple_booking

// a little class to capture booking parameters
// to create an ntuple.

#include "cids.h"

namespace inlib {

    class column_booking {
    public:
        column_booking(const std::string& a_name, cid a_cid, void* a_user_obj)
            : m_name(a_name)
            , m_cid(a_cid)
            , m_user_obj(a_user_obj) //WARNING : not owner.
        {}
        virtual ~column_booking() {}
    public:
        column_booking(const column_booking& a_from)
            : m_name(a_from.m_name)
            , m_cid(a_from.m_cid)
            , m_user_obj(a_from.m_user_obj)
        {}
        column_booking& operator=(const column_booking& a_from)
        {
            if (&a_from == this) return *this;

            m_name = a_from.m_name;
            m_cid = a_from.m_cid;
            m_user_obj = a_from.m_user_obj;
            return *this;
        }
    public:
        const std::string& name() const
        {
            return m_name;
        }
        cid cls_id() const
        {
            return m_cid;
        }
        void* user_obj() const
        {
            return m_user_obj;
        }
    protected:
        std::string m_name;
        cid m_cid;
        void* m_user_obj;
    };

    class ntuple_booking {
    public:
        ntuple_booking(const std::string& a_name = "", const std::string& a_title = "")
            : m_name(a_name)
            , m_title(a_title)
        {}
        virtual ~ntuple_booking() {}
    public:
        ntuple_booking(const ntuple_booking& a_from)
            : m_name(a_from.m_name)
            , m_title(a_from.m_title)
            , m_columns(a_from.m_columns)
        {}
        ntuple_booking& operator=(const ntuple_booking& a_from)
        {
            m_name = a_from.m_name;
            m_title = a_from.m_title;
            m_columns = a_from.m_columns;
            return *this;
        }
    public:
        template <class T>
        void add_column(const std::string& a_name)
        {
            m_columns.push_back(column_booking(a_name, _cid(T()), 0));
        }
        template <class T>
        void add_column(const std::string& a_name, std::vector<T>& a_user_vec)
        {
            m_columns.push_back(column_booking(a_name, _cid_std_vector<T>(), (void*)&a_user_vec));
        }

        const std::string& name() const
        {
            return m_name;
        }
        const std::string& title() const
        {
            return m_title;
        }
        const std::vector<column_booking>& columns() const
        {
            return m_columns;
        }

        void set_name(const std::string& a_s)
        {
            m_name = a_s;
        }
        void set_title(const std::string& a_s)
        {
            m_title = a_s;
        }
    protected:
        std::string m_name;
        std::string m_title;
        std::vector<column_booking> m_columns;
    };

}

#endif

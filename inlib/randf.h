// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_randf
#define inlib_randf

#include "randT.h"
#include "rtausmef.h"

namespace inlib {

    class rgaussf : public rgauss<rtausmef, float> {
        typedef rgauss<rtausmef, float> parent;
    public:
        rgaussf(float a_mean = 0, float a_std_dev = 1): parent(m_flat, a_mean, a_std_dev), m_flat() {}
        virtual ~rgaussf() {}
    public:
        rgaussf(const rgaussf& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rgaussf& operator=(const rgaussf& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmef m_flat;
    };

    class rbwf : public rbw<rtausmef, float> {
        typedef rbw<rtausmef, float> parent;
    public:
        rbwf(float a_mean = 0, float a_gamma = 1): parent(m_flat, a_mean, a_gamma), m_flat() {}
        virtual ~rbwf() {}
    public:
        rbwf(const rbwf& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rbwf& operator=(const rbwf& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmef m_flat;
    };

    class rexpf : public rexp<rtausmef, float> {
        typedef rexp<rtausmef, float> parent;
    public:
        rexpf(float a_rate = 1): parent(m_flat, a_rate), m_flat() {}
        virtual ~rexpf() {}
    public:
        rexpf(const rexpf& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rexpf& operator=(const rexpf& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmef m_flat;
    };

}

#endif

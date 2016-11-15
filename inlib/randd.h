// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_randd
#define inlib_randd

#include "randT.h"
#include "rtausmed.h"

namespace inlib {

    class rgaussd : public rgauss<rtausmed, double> {
        typedef rgauss<rtausmed, double> parent;
    public:
        rgaussd(double a_mean = 0, double a_std_dev = 1): parent(m_flat, a_mean, a_std_dev), m_flat() {}
        virtual ~rgaussd() {}
    public:
        rgaussd(const rgaussd& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rgaussd& operator=(const rgaussd& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmed m_flat;
    };

    class rbwd : public rbw<rtausmed, double> {
        typedef rbw<rtausmed, double> parent;
    public:
        rbwd(double a_mean = 0, double a_gamma = 1): parent(m_flat, a_mean, a_gamma), m_flat() {}
        virtual ~rbwd() {}
    public:
        rbwd(const rbwd& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rbwd& operator=(const rbwd& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmed m_flat;
    };

    class rexpd : public rexp<rtausmed, double> {
        typedef rexp<rtausmed, double> parent;
    public:
        rexpd(double a_rate = 1): parent(m_flat, a_rate), m_flat() {}
        virtual ~rexpd() {}
    public:
        rexpd(const rexpd& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rexpd& operator=(const rexpd& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmed m_flat;
    };

    class rdir2d : public rdir2<rtausmed, double> {
        typedef rdir2<rtausmed, double> parent;
    public:
        rdir2d(): parent(m_flat), m_flat() {}
        virtual ~rdir2d() {}
    public:
        rdir2d(const rdir2d& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rdir2d& operator=(const rdir2d& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmed m_flat;
    };

    class rdir3d : public rdir3<rtausmed, double> {
        typedef rdir3<rtausmed, double> parent;
    public:
        rdir3d(): parent(m_flat), m_flat() {}
        virtual ~rdir3d() {}
    public:
        rdir3d(const rdir3d& a_from): parent(m_flat), m_flat(a_from.m_flat) {}
        rdir3d& operator=(const rdir3d& a_from)
        {
            parent::operator=(a_from);
            m_flat = a_from.m_flat;
            return *this;
        }
    protected:
        rtausmed m_flat;
    };

}

#endif

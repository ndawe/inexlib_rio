// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_buf
#define inlib_root_buf

#include "../stype.h"
#include "../long_out.h"
#include "../charp_out.h"

#ifdef INLIB_MEM
    #include "../mem.h"
#endif

#include <ostream>
#include <vector>
#include <cstring> //memcpy

namespace inlib {
    namespace root {

        class buf {

            /////////////////////////////////////////////////////////
            /// swap ////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            // NOTE : on most common platforms (including Android, iPad)
            //        CERN-ROOT byte swaps ! (Bad luck). We have arranged to
            //        optimize this operation. The below "_swap_" functions
            //        do not have local variables and manipulates pointers
            //        directly.

            static void read_swap_2(char* a_pos, char* a_x)
            {
                *a_x++ = *(a_pos + 1);
                *a_x++ = *a_pos;
            }

            static void write_swap_2(char* a_pos, char* a_x)
            {
                *a_pos++ = *(a_x + 1);
                *a_pos++ = *a_x;
            }

            static void read_swap_4(char* a_pos, char* a_x)
            {
                a_pos += 3;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos;
            }

            static void write_swap_4(char* a_pos, char* a_x)
            {
                a_x += 3;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x;
            }

            static void read_swap_8(char* a_pos, char* a_x)
            {
                a_pos += 7;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos--;
                *a_x++ = *a_pos;
            }

            static void write_swap_8(char* a_pos, char* a_x)
            {
                a_x += 7;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x--;
                *a_pos++ = *a_x;
            }

            /////////////////////////////////////////////////////////
            /// nswp ////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            static void read_nswp_2(char* a_pos, char* a_x)
            {
                ::memcpy(a_x, a_pos, 2);
            }

            static void write_nswp_2(char* a_pos, char* a_x)
            {
                ::memcpy(a_pos, a_x, 2);
            }

            static void read_nswp_4(char* a_pos, char* a_x)
            {
                ::memcpy(a_x, a_pos, 4);
            }

            static void write_nswp_4(char* a_pos, char* a_x)
            {
                ::memcpy(a_pos, a_x, 4);
            }

            static void read_nswp_8(char* a_pos, char* a_x)
            {
                ::memcpy(a_x, a_pos, 8);
            }

            static void write_nswp_8(char* a_pos, char* a_x)
            {
                ::memcpy(a_pos, a_x, 8);
            }

            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////


            static const std::string& s_class()
            {
                static const std::string s_v("inlib::root::buf");
                return s_v;
            }
            typedef void (*r_2_func)(char*, char*);
            typedef void (*r_4_func)(char*, char*);
            typedef void (*r_8_func)(char*, char*);
            typedef void (*w_2_func)(char*, char*);
            typedef void (*w_4_func)(char*, char*);
            typedef void (*w_8_func)(char*, char*);
        public:
            buf(std::ostream& a_out, bool a_byte_swap,
                const char* a_eob, char*& a_pos)
                : m_out(a_out)
                , m_byte_swap(a_byte_swap)
                , m_eob(a_eob)
                , m_pos(a_pos)

                , m_r_2_func(0)
                , m_r_4_func(0)
                , m_r_8_func(0)
                , m_w_2_func(0)
                , m_w_4_func(0)
                , m_w_8_func(0)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
                set_byte_swap(a_byte_swap);
            }
            virtual ~buf()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        public:
            buf(const buf& a_from)
                : m_out(a_from.m_out)
                , m_byte_swap(a_from.m_byte_swap)
                , m_eob(a_from.m_eob)
                , m_pos(a_from.m_pos)
                , m_r_2_func(a_from.m_r_2_func)
                , m_r_4_func(a_from.m_r_4_func)
                , m_r_8_func(a_from.m_r_8_func)
                , m_w_2_func(a_from.m_w_2_func)
                , m_w_4_func(a_from.m_w_4_func)
                , m_w_8_func(a_from.m_w_8_func)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
                set_byte_swap(a_from.m_byte_swap);
            }
            buf& operator=(const buf& a_from)
            {
                set_byte_swap(a_from.m_byte_swap);
                m_eob = a_from.m_eob;
                //m_pos is a ref.
                m_r_2_func = a_from.m_r_2_func;
                m_r_4_func = a_from.m_r_4_func;
                m_r_8_func = a_from.m_r_8_func;
                m_w_2_func = a_from.m_w_2_func;
                m_w_4_func = a_from.m_w_4_func;
                m_w_8_func = a_from.m_w_8_func;
                return *this;
            }
        public:
            std::ostream& out() const
            {
                return m_out;
            }

            void skip(unsigned int a_num)
            {
                m_pos += a_num;
            }

            void set_eob(const char* a_eob)
            {
                m_eob = a_eob;
            }
            char*& pos()
            {
                return m_pos;
            }
            const char* eob() const
            {
                return m_eob;
            }

            bool byte_swap() const
            {
                return m_byte_swap;
            }

            void set_byte_swap(bool a_value)
            {
                m_byte_swap = a_value;

                if (m_byte_swap) {
                    m_r_2_func = read_swap_2;
                    m_r_4_func = read_swap_4;
                    m_r_8_func = read_swap_8;
                    m_w_2_func = write_swap_2;
                    m_w_4_func = write_swap_4;
                    m_w_8_func = write_swap_8;
                } else {
                    m_r_2_func = read_nswp_2;
                    m_r_4_func = read_nswp_4;
                    m_r_8_func = read_nswp_8;
                    m_w_2_func = write_nswp_2;
                    m_w_4_func = write_nswp_4;
                    m_w_8_func = write_nswp_8;
                }
            }
        public:
            bool read(unsigned char& a_x)
            {
                if (!_check_eob<unsigned char>(a_x)) return false;

                a_x = *m_pos++;
                return true;
            }

            bool write(unsigned char a_x)
            {
                if (!check_eob<unsigned char>()) return false;

                *m_pos++ = a_x;
                return true;
            }

            bool read(unsigned short& a_x)
            {
                if (!_check_eob<unsigned short>(a_x)) return false;

                m_r_2_func(m_pos, (char*)&a_x);
                m_pos += sizeof(unsigned short);
                return true;
            }

            bool write(unsigned short a_x)
            {
                if (!check_eob<unsigned short>()) return false;

                m_w_2_func(m_pos, (char*)&a_x);
                m_pos += sizeof(unsigned short);
                return true;
            }

            bool read(unsigned int& a_x)
            {
                if (!_check_eob<unsigned int>(a_x)) return false;

                m_r_4_func(m_pos, (char*)&a_x);
                m_pos += sizeof(unsigned int);
                return true;
            }

            bool write(unsigned int a_x)
            {
                if (!check_eob<unsigned int>()) return false;

                m_w_4_func(m_pos, (char*)&a_x);
                m_pos += sizeof(unsigned int);
                return true;
            }

            bool read(uint64& a_x)
            {
                if (!_check_eob<uint64>(a_x)) return false;

                m_r_8_func(m_pos, (char*)&a_x);
                m_pos += 8;
                return true;
            }

            bool write(uint64 a_x)
            {
                if (!check_eob<uint64>()) return false;

                m_w_8_func(m_pos, (char*)&a_x);
                m_pos += 8;
                return true;
            }

            bool read(float& a_x)
            {
                if (!_check_eob<float>(a_x)) return false;

                m_r_4_func(m_pos, (char*)&a_x);
                m_pos += sizeof(float);
                return true;
            }

            bool write(float a_x)
            {
                if (!check_eob<float>()) return false;

                m_w_4_func(m_pos, (char*)&a_x);
                m_pos += sizeof(float);
                return true;
            }

            bool read(double& a_x)
            {
                if (!_check_eob<double>(a_x)) return false;

                m_r_8_func(m_pos, (char*)&a_x);
                m_pos += sizeof(double);
                return true;
            }

            bool write(double a_x)
            {
                if (!check_eob<double>()) return false;

                m_w_8_func(m_pos, (char*)&a_x);
                m_pos += sizeof(double);
                return true;
            }

            bool read(char& a_x)
            {
                if (!_check_eob<char>(a_x)) return false;

                a_x = *m_pos++;
                return true;
            }

            bool write(char a_x)
            {
                return write((unsigned char)a_x);
            }

            bool read(short& a_x)
            {
                if (!_check_eob<short>(a_x)) return false;

                m_r_2_func(m_pos, (char*)&a_x);
                m_pos += sizeof(short);
                return true;
            }

            bool write(short a_x)
            {
                return write((unsigned short)a_x);
            }

            bool read(int& a_x)
            {
                if (!_check_eob<int>(a_x)) return false;

                m_r_4_func(m_pos, (char*)&a_x);
                m_pos += sizeof(int);
                return true;
            }

            bool write(int a_x)
            {
                return write((unsigned int)a_x);
            }

            bool read(int64& a_x)
            {
                if (!_check_eob<int64>(a_x)) return false;

                m_r_8_func(m_pos, (char*)&a_x);
                m_pos += 8;
                return true;
            }

            bool write(int64 a_x)
            {
                return write((uint64)a_x);
            }

            bool read(std::string& a_x)
            {
                unsigned char nwh;

                if (!read(nwh)) {
                    a_x.clear();
                    return false;
                }

                int nchars;

                if (nwh == 255) {
                    if (!read(nchars)) {
                        a_x.clear();
                        return false;
                    }
                } else {
                    nchars = nwh;
                }

                if (nchars < 0) {
                    m_out << s_class() << "::read(string) :"
                          << " negative char number " << nchars << "." << std::endl;
                    a_x.clear();
                    return false;
                }

                if ((m_pos + nchars) > m_eob) {
                    m_out << s_class() << "::read(string) :"
                          << " try to access out of buffer " << long_out(nchars) << " bytes "
                          << " (pos=" << charp_out(m_pos)
                          << ", eob=" << charp_out(m_eob) << ")." << std::endl;
                    a_x.clear();
                    return false;
                }

                a_x.resize(nchars);
                ::memcpy((char*)a_x.c_str(), m_pos, nchars);
                m_pos += nchars;
                return true;
            }

            bool write(const std::string& a_x)
            {
                unsigned char nwh;
                unsigned int nchars = a_x.size();

                if (nchars > 254) {
                    if (!check_eob(1 + 4, "std::string")) return false;

                    nwh = 255;

                    if (!write(nwh)) return false;

                    if (!write(nchars)) return false;
                } else {
                    if (!check_eob(1, "std::string")) return false;

                    nwh = (unsigned char)nchars;

                    if (!write(nwh)) return false;
                }

                if (!check_eob(nchars, "std::string")) return false;

                for (unsigned int i = 0; i < nchars; i++) m_pos[i] = a_x[i];

                m_pos += nchars;
                return true;
            }

            bool read(bool& x)
            {
                unsigned char uc = 0;
                bool status = read(uc);
                x = uc ? true : false;
                return status;
            }

            bool read(std::vector<std::string>& a_a)
            {
                int n;

                if (!read(n)) {
                    a_a.clear();
                    return false;
                }

                for (int index = 0; index < n; index++) {
                    std::string s;

                    if (!read(s)) {
                        a_a.clear();
                        return false;
                    }

                    a_a.push_back(s);
                }

                return true;
            }

            template <class T>
            bool write(const T* a_a, uint32 a_n)
            {
                if (!a_n) return true;

                uint32 l = a_n * sizeof(T);

                if (!check_eob(l, "array")) return false;

                if (m_byte_swap) {
                    for (uint32 i = 0; i < a_n; i++) {
                        if (!write(a_a[i])) return false;
                    }
                } else {
                    ::memcpy(m_pos, a_a, l);
                    m_pos += l;
                }

                return true;
            }

            template <class T>
            bool write(const std::vector<T>& a_v)
            {
                if (a_v.empty()) return true;

                uint32 n = a_v.size();
                uint32 l = n * sizeof(T);

                if (!check_eob(l, "array")) return false;

                for (uint32 i = 0; i < n; i++) {
                    if (!write(a_v[i])) return false;
                }

                return true;
            }

            //////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////
            bool read_fast_array(bool* b, uint32 n)
            {
                if (!n) return true;

                uint32 l = n * sizeof(unsigned char);

                if (!check_eob(l)) return false;

                for (uint32 i = 0; i < n; i++) {
                    unsigned char uc;

                    if (!read(uc)) return false;

                    b[i] = uc ? true : false;
                }

                return true;
            }
            bool read_fast_array(char* c, uint32 n)
            {
                if (!n) return true;

                uint32 l = n * sizeof(char);

                if (!check_eob(l)) return false;

                ::memcpy(c, m_pos, l);
                m_pos += l;
                return true;
            }
            bool read_fast_array(unsigned char* c, uint32 n)
            {
                if (!n) return true;

                uint32 l = n * sizeof(unsigned char);

                if (!check_eob(l)) return false;

                ::memcpy(c, m_pos, l);
                m_pos += l;
                return true;
            }

            template <class T>
            bool read_fast_array(T* a_a, uint32 a_n)
            {
                if (!a_n) return true;

                uint32 l = a_n * sizeof(T);

                if (!check_eob(l)) {
                    m_out << s_class() << "::read_fast_array :"
                          << " try to access out of buffer " << long_out(l) << " bytes "
                          << " (pos=" << charp_out(m_pos)
                          << ", eob=" << charp_out(m_eob) << ")." << std::endl;
                    return false;
                }

                if (m_byte_swap) {
                    for (uint32 i = 0; i < a_n; i++) {
                        if (!read(*(a_a + i))) return false;
                    }
                } else {
                    ::memcpy(a_a, m_pos, l);
                    m_pos += l;
                }

                return true;
            }

            template <class T>
            bool read_array(uint32 a_sz, T*& a_a, uint32& a_n)
            {
                a_n = 0;
                {
                    int n;

                    if (!read(n)) {
                        a_n = 0;
                        return false;
                    }

                    a_n = n;
                }

                if (!a_n) return true;

                uint32 l = a_n * sizeof(T);

                if (!check_eob(l)) return false;

                bool owner = false;

                if (!a_a) {
                    //ignore a_sz
                    a_a = new T[a_n];

                    if (!a_a) {
                        a_n = 0;
                        return false;
                    }

                    owner = true;
                } else {
                    if (a_n > a_sz) return false;
                }

                if (m_byte_swap) {
                    for (uint32 i = 0; i < a_n; i++) {
                        if (!read(*(a_a + i))) {
                            if (owner) {
                                delete [] a_a;
                                a_a = 0;
                            }

                            a_n = 0;
                            return false;
                        }
                    }
                } else {
                    ::memcpy(a_a, m_pos, l);
                    m_pos += l;
                }

                return true;
            }

            template <class T>
            bool read_array(std::vector<T>& a_v)
            {
                T* buffer = 0;
                uint32 n;

                if (!read_array(0, buffer, n)) {
                    a_v.clear();
                    return false;
                }

                if (!buffer) {
                    a_v.clear();
                    return true;
                }

                a_v.resize(n);

                for (uint32 index = 0; index < n; index++) {
                    a_v[index] = buffer[index];
                }

                delete [] buffer;
                return true;
            }

            template <class T>
            bool read_array2(std::vector< std::vector<T> >& a_v)
            {
                int n;

                if (!read(n)) {
                    a_v.clear();
                    return false;
                }

                a_v.resize(n);

                for (int index = 0; index < n; index++) {
                    if (!read_array(a_v[index])) return false;
                }

                return true;
            }

            bool check_eob(uint32 n)
            {
                if ((m_pos + n) > m_eob) {
                    m_out << "inlib::rroot::buf::check_eob :"
                          << " try to access out of buffer " << n << " bytes."
                          << std::endl;
                    return false;
                }

                return true;
            }

        protected:
            template <class T>
            bool _check_eob(T& a_x)
            {
                if ((m_pos + sizeof(T)) > m_eob) {
                    a_x = T();
                    m_out << s_class() << " : " << stype(T()) << " : "
                          << " try to access out of buffer " << long_out(sizeof(T)) << " bytes"
                          << " (pos=" << charp_out(m_pos)
                          << ", eob=" << charp_out(m_eob) << ")." << std::endl;
                    return false;
                }

                return true;
            }

            template <class T>
            bool check_eob()
            {
                if ((m_pos + sizeof(T)) > m_eob) {
                    m_out << s_class() << " : " << stype(T()) << " : "
                          << " try to access out of buffer " << long_out(sizeof(T)) << " bytes"
                          << " (pos=" << charp_out(m_pos)
                          << ", eob=" << charp_out(m_eob) << ")." << std::endl;
                    return false;
                }

                return true;
            }

            bool check_eob(size_t a_n, const char* a_cmt)
            {
                if ((m_pos + a_n) > m_eob) {
                    m_out << s_class() << " : " << a_cmt << " : "
                          << " try to access out of buffer " << long_out(a_n) << " bytes"
                          << " (pos=" << charp_out(m_pos)
                          << ", eob=" << charp_out(m_eob) << ")." << std::endl;
                    return false;
                }

                return true;
            }
        protected:
            std::ostream& m_out;
            bool m_byte_swap;
            const char* m_eob;
            char*& m_pos;

            r_2_func m_r_2_func;
            r_4_func m_r_4_func;
            r_8_func m_r_8_func;

            w_2_func m_w_2_func;
            w_4_func m_w_4_func;
            w_8_func m_w_8_func;
        };

    }
}

#endif

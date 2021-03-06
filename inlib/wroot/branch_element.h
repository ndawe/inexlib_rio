// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_wroot_branch_element
#define inlib_wroot_branch_element

#include "branch.h"
#include "leaf.h"

namespace inlib {
    namespace wroot {

        class branch_element : public branch {
            typedef branch parent;
            #ifdef INLIB_MEM
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::wroot::branch_element");
                return s_v;
            }
            #endif
        public: //ibo
            virtual const std::string& store_cls() const
            {
                static const std::string s_v("TBranchElement");
                return s_v;
            }
            virtual bool stream(buffer& a_buffer) const
            {
                unsigned int c;

                if (!a_buffer.write_version(1, c)) return false;

                if (!parent::stream(a_buffer)) return false;

                if (!a_buffer.write(fClassName)) return false;

                if (!a_buffer.write(fClassVersion)) return false;

                if (!a_buffer.write(fID)) return false;

                if (!a_buffer.write(fType)) return false;

                if (!a_buffer.write(fStreamerType)) return false;

                if (!a_buffer.set_byte_count(c)) return false;

                return true;
            }

        public:
            branch_element(itree& a_tree,
                           const std::string& a_name,
                           const std::string& a_title)
                : parent(a_tree, a_name, a_title)
                , fClassVersion(0)
                , fID(0)
                , fType(0)
                , fStreamerType(-1)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            virtual ~branch_element()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            branch_element(const branch_element& a_from)
                : ibo(a_from)
                , parent(a_from)
            {}
            branch_element& operator=(const branch_element& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }
        public:
            leaf_element* create_leaf_element(const std::string& a_name,
                                              const std::string& a_title)
            {
                leaf_element* lf = new leaf_element(m_out, *this, a_name, a_title, fID, fType);
                m_leaves.push_back(lf);
                return lf;
            }
        protected:
            virtual bool fill_leaves(buffer&)
            {
                return false;   //must be derived.
            }
        protected:
            std::string fClassName; //Class name of referenced object
            int fClassVersion;      //Version number of class
            int fID;                //element serial number in fInfo
            int fType;              //branch type
            int fStreamerType;      //branch streamer type
        };

        template <class T>
        class std_vector_be : public branch_element {
            typedef branch_element parent;
            #ifdef INLIB_MEM
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::wroot::std_vector_be");
                return s_v;
            }
            #endif
        public:
            std_vector_be(itree& a_tree,
                          const std::string& a_name,
                          const std::string& a_title,
                          std::vector<T>& a_vec)
                : parent(a_tree, a_name, a_title)
                , m_vec(a_vec)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
                fClassName = "vector<" + stype(T()) + ">";
                fClassVersion = 0;
                fID = -1;
                fType = 0;
                fStreamerType  = -1; // TStreamerInfo::kSTLp;
            }
            virtual ~std_vector_be()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            std_vector_be(const std_vector_be& a_from)
                : ibo(a_from)
                , parent(a_from)
                , m_vec(a_from.m_vec)
            {}
            std_vector_be& operator=(const std_vector_be& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }
        protected:
            virtual bool fill_leaves(buffer& a_buffer)
            {
                unsigned int c;

                if (!a_buffer.write_version(4, c)) return false;

                if (!a_buffer.write((int)m_vec.size())) return false;

                if (m_vec.size()) {
                    // The awfull below is to pass T=bool :
                    const T& vr = m_vec[0];

                    if (!a_buffer.write_fast_array(&vr, (int)m_vec.size())) return false;
                }

                if (!a_buffer.set_byte_count(c)) return false;

                return true;
            }
        protected:
            std::vector<T>& m_vec;
        };

    }
}

//inlib_build_use kernel

#endif

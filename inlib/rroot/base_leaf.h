// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_base_leaf
#define inlib_rroot_base_leaf

#ifdef INLIB_MEM
    #include "../mem.h"
#endif

#include "named.h"

namespace inlib {
    namespace rroot {
        class branch;
    }
}

namespace inlib {
    namespace rroot {

        class base_leaf : public virtual iro {
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::rroot::base_leaf");
                return s_v;
            }
        public: //iro
            virtual void* cast(const std::string& a_class) const
            {
                if (void* p = cmp_cast<base_leaf>(this, a_class)) return p;

                return 0;
            }
            virtual const std::string& s_cls() const
            {
                return s_class();
            }
        public:
            static cid id_class()
            {
                return base_leaf_cid();
            }
            virtual void* cast(cid a_class) const
            {
                if (void* p = cmp_cast<base_leaf>(this, a_class)) {
                    return p;
                } else return 0;
            }
        public:
            virtual bool stream(buffer& a_buffer)
            {
                delete m_leaf_count;
                m_leaf_count = 0;
                int fLengthType;
                int fOffset;
                bool fIsRange;
                bool fIsUnsigned;
                short v;
                unsigned int s, c;

                if (!a_buffer.read_version(v, s, c)) return false;

                //FIXME if (v > 1) {
                //TLeaf::Class()->ReadBuffer(b, this, R__v, R__s, R__c);
                //FIXME } else {
                //====process old versions before automatic schema evolution
                if (!Named_stream(a_buffer, m_name, m_title)) return false;

                // Ok with v 1 & 2
                if (!a_buffer.read(m_length)) return false;

                if (!a_buffer.read(fLengthType)) return false;

                if (!a_buffer.read(fOffset)) return false;

                if (!a_buffer.read(fIsRange)) return false;

                if (!a_buffer.read(fIsUnsigned)) return false;

                {
                    ifac::args args;
                    args[ifac::arg_branch()] = &m_branch;
                    iro* obj;

                    if (!a_buffer.read_object(m_fac, args, obj)) {
                        m_out << "inlib::rroot::base_leaf::stream :"
                              << " can't read object."
                              << std::endl;
                        return false;
                    }

                    if (!obj) {
                        //m_out << "inlib::rroot::base_leaf::stream :"
                        //      << " null leaf count object."
                        //      << std::endl;
                    } else {
                        m_leaf_count = safe_cast<iro, base_leaf>(*obj);

                        if (!m_leaf_count) {
                            m_out << "inlib::rroot::base_leaf::stream :"
                                  << " can't cast base_leaf."
                                  << std::endl;
                            m_leaf_count = 0;
                            delete obj;
                            return false;
                        }
                    }
                }

                if (!a_buffer.check_byte_count(s, c, "TLeaf")) return false;

                if (!m_length) m_length = 1;

                /*
                    fNewValue = false;
                    if(!setAddress(0)) return false;
                */
                return true;
            }
        public:
            virtual bool read_basket(buffer&) = 0;
            virtual bool print_value(std::ostream&, uint32) const = 0;
            virtual uint32 num_elem() const = 0;
        public:
            base_leaf(std::ostream& a_out, rroot::branch& a_branch, ifac& a_fac)
                : m_out(a_out)
                , m_branch(a_branch)
                , m_fac(a_fac)
                , m_name("")
                , m_title("")

                  //,fIndirectAddress(false)
                  //,fNewValue(false)
                , m_ndata(0)
                , m_length(0)
                  /*
                    ,fLengthType(0)
                    ,fOffset(0)
                    ,fIsRange(false)
                    ,fIsUnsigned(false)
                  */
                , m_leaf_count(0)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            virtual ~base_leaf()
            {
                delete m_leaf_count;
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            base_leaf(const base_leaf& a_from)
                : iro(a_from)
                , m_out(a_from.m_out)
                , m_branch(a_from.m_branch)
                , m_fac(a_from.m_fac)
                , m_ndata(0)
                , m_length(0)
                , m_leaf_count(0)
            {}
            base_leaf& operator=(const base_leaf&)
            {
                return *this;
            }
        public:
            rroot::branch& branch()
            {
                return m_branch;
            }
            const std::string& name() const
            {
                return m_name;
            }
            const std::string& title() const
            {
                return m_title;
            }
            /*
              uint32 length() const {
                // Return the number of effective elements of this leaf.
                if(m_leaf_count) {
                  m_out << "inlib::rroot::base_leaf::length :"
                        << " m_leaf_count not null. Case not yet handled."
                        << std::endl;
                  return m_length;

                  //uint32 len = m_leaf_count->number();
                  //if (len > fLeafCount->maximum()) {
                  //  m_out << "inlib::rroot::base_leaf::length :"
                  //        << fName << ", len=" << len << " and max="
                  //        << fLeafCount->maximum()
                  //        << std::endl;
                  //  len = fLeafCount->maximum();
                  //}
                  //return len * fLength;

                } else {
                  return m_length;
                }
              }
            */
        protected:
            std::ostream& m_out;
            rroot::branch& m_branch;
            ifac& m_fac;
        protected: //Named
            std::string m_name;
            std::string m_title;
            //bool fIndirectAddress;
            //bool fNewValue;
            uint32 m_ndata;           //! Number of elements in fAddress data buffer
            uint32 m_length;          //  Number of fixed length elements
            /*
              int fLengthType;      //  Number of bytes for this data type
              int fOffset;          //  Offset in ClonesArray object (if one)
              bool fIsRange;        //  (=true if leaf has a range, false otherwise)
              bool fIsUnsigned;     //  (=kTRUE if unsigned, kFALSE otherwise)
            */
            base_leaf* m_leaf_count; //  Pointer to Leaf count if variable length
        };

    }
}

#endif

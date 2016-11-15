// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_info
#define inlib_rroot_info

//#include "buffer"
//#include "element"
#include "named"

namespace inlib {
    namespace rroot {

        class streamer_element : public virtual iro {
            static const std::string& s_store_class()
            {
                static const std::string s_v("TStreamerElement");
                return s_v;
            }
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::rroot::streamer_element");
                return s_v;
            }
            static cid id_class()
            {
                return streamer_element_cid();
            }
        public: //iro
            virtual void* cast(const std::string& a_class) const
            {
                if (void* p = cmp_cast<streamer_element>(this, a_class)) return p;

                return 0;
            }
            virtual void* cast(cid a_class) const
            {
                if (void* p = cmp_cast<streamer_element>(this, a_class)) {
                    return p;
                } else return 0;
            }
            virtual const std::string& s_cls() const
            {
                return s_class();
            }
            virtual iro* copy() const
            {
                return new streamer_element(*this);
            }

            virtual bool stream(buffer& a_buffer)
            {
                short v;
                unsigned int s, c;

                if (!a_buffer.read_version(v, s, c)) return false;

                if (!Named_stream(a_buffer, fName, fTitle)) return false;

                if (!a_buffer.read(fType)) return false;

                if (!a_buffer.read(fSize)) return false;

                if (!a_buffer.read(fArrayLength)) return false;

                if (!a_buffer.read(fArrayDim)) return false;

                if (!a_buffer.read_fast_array<int>(fMaxIndex, 5)) return false;

                if (!a_buffer.read(fTypeName)) return false;

                return a_buffer.check_byte_count(s, c, s_store_class());
            }
        public:
            virtual void out(std::ostream& aOut) const
            {
                std::string _fname;
                fullName(_fname);
                char s[128];
                snpf(s, sizeof(s), "  %-14s%-15s offset=%3d type=%2d %-20s",
                     fTypeName.c_str(), _fname.c_str(), fOffset, fType, fTitle.c_str());
                aOut << s << std::endl;
            }
        public:
            streamer_element()
                : fName(), fTitle(), fType(-1)
                , fSize(0), fArrayLength(0), fArrayDim(0), fOffset(0)
                , fTypeName()
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif

                for (int i = 0; i < 5; i++) fMaxIndex[i] = 0;
            }
            virtual ~streamer_element()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            streamer_element(const streamer_element& a_from)
                : iro(a_from)
                , fName(a_from.fName), fTitle(a_from.fTitle)
                , fType(a_from.fType), fSize(a_from.fSize)
                , fArrayLength(a_from.fArrayLength)
                , fArrayDim(a_from.fArrayDim), fOffset(a_from.fOffset)
                , fTypeName(a_from.fTypeName)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif

                for (int i = 0; i < 5; i++) fMaxIndex[i] = a_from.fMaxIndex[i];
            }
            streamer_element& operator=(const streamer_element& a_from)
            {
                fName = a_from.fName;
                fTitle = a_from.fTitle;
                fType = a_from.fType;
                fSize = a_from.fSize;
                fArrayLength = a_from.fArrayLength;
                fArrayDim = a_from.fArrayDim;
                fOffset = a_from.fOffset;
                fTypeName = a_from.fTypeName;

                for (int i = 0; i < 5; i++) fMaxIndex[i] = a_from.fMaxIndex[i];

                return *this;
            }
        public:
            virtual void fullName(std::string& a_s) const
            {
                a_s = fName;

                for (int i = 0; i < fArrayDim; i++) {
                    char cdim[32];
                    snpf(cdim, sizeof(cdim), "[%d]", fMaxIndex[i]);
                    a_s += cdim;
                }
            }
        protected: //Named
            std::string fName;
            std::string fTitle;
        protected:
            int fType;            //element type
            int fSize;            //sizeof element
            int fArrayLength;     //cumulative size of all array dims
            int fArrayDim;        //number of array dimensions
            int fMaxIndex[5];     //Maximum array index for array dimension "dim"
            int fOffset;          //!element offset in class
            //FIXME Int_t         fNewType;         //!new element type when reading
            std::string fTypeName;        //Data type name of data member
        };

        class dummy_streamer_element : public streamer_element {
            typedef streamer_element parent;
        public: //iro
            virtual iro* copy() const
            {
                return new dummy_streamer_element(*this);
            }
            virtual bool stream(buffer& a_buffer)
            {
                //the below code skips correctly the data in the file.
                uint32 startpos = a_buffer.length();
                short v;
                unsigned int s, c;

                if (!a_buffer.read_version(v, s, c)) return false;

                if (!parent::stream(a_buffer)) return false;

                a_buffer.set_offset(startpos + c + sizeof(unsigned int));

                if (!a_buffer.check_byte_count(s, c, "dummy_streamer_element")) return false;

                return true;
            }
        public:
            dummy_streamer_element() {}
            virtual ~dummy_streamer_element() {}
        protected:
            dummy_streamer_element(const dummy_streamer_element& a_from): iro(a_from), parent(a_from) {}
            dummy_streamer_element& operator=(const dummy_streamer_element& a_from)
            {
                parent::operator=(a_from);
                return *this;
            }
        };

        class StreamerInfo : public virtual iro {
            static const std::string& s_store_class()
            {
                static const std::string s_v("TStreamerInfo");
                return s_v;
            }
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::rroot::StreamerInfo");
                return s_v;
            }
            static cid id_class()
            {
                return StreamerInfo_cid();
            }
        public: //iro
            virtual void* cast(const std::string& a_class) const
            {
                if (void* p = cmp_cast<StreamerInfo>(this, a_class)) return p;

                return 0;
            }
            virtual void* cast(cid a_class) const
            {
                if (void* p = cmp_cast<StreamerInfo>(this, a_class)) {
                    return p;
                } else return 0;
            }
            virtual const std::string& s_cls() const
            {
                return s_class();
            }
            virtual iro* copy() const
            {
                return new StreamerInfo(*this);
            }

            virtual bool stream(buffer& a_buffer)
            {
                short v;
                unsigned int s, c;

                if (!a_buffer.read_version(v, s, c)) return false;

                if (!Named_stream(a_buffer, m_name, m_title)) return false;

                if (!a_buffer.read(m_check_sum)) return false;

                if (!a_buffer.read(m_streamed_class_version)) return false;

                //TObjArray        *fElements;       //Array of TStreamerElements
                {
                    ObjArray<streamer_element>* obj;
                    ifac::args args;
                    args[ifac::arg_class()] = (void*) & (streamer_element::s_class());

                    if (!pointer_stream(a_buffer, m_fac, args, obj)) {
                        a_buffer.out() << "inlib::rroot::StreamerInfo::stream : "
                                       << "can't read fElements."
                                       << std::endl;
                        return false;
                    }

                    m_elements.operator = (*obj);
                    delete obj;
                }
                return a_buffer.check_byte_count(s, c, s_store_class());
            }
        public:
            void out(std::ostream& a_out) const
            {
                a_out << "StreamerInfo for class :"
                      << " " << m_name << ", version=" << m_streamed_class_version
                      << std::endl;
                inlib_vforcit(streamer_element*, m_elements, it)(*it)->out(a_out);
            }
        public:
            StreamerInfo(ifac& a_fac)
                : m_fac(a_fac)
                , m_name()
                , m_title()
                , m_check_sum(0)
                , m_streamed_class_version(0)
                , m_elements(a_fac, true)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            virtual ~StreamerInfo()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            StreamerInfo(const StreamerInfo& a_from)
                : iro(a_from)
                , m_fac(a_from.m_fac)
                , m_name(a_from.m_name)
                , m_title(a_from.m_name)
                , m_check_sum(a_from.m_check_sum)
                , m_streamed_class_version(a_from.m_streamed_class_version)
                , m_elements(a_from.m_elements)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            StreamerInfo& operator=(const StreamerInfo& a_from)
            {
                m_name = a_from.m_name;
                m_title = a_from.m_name;
                m_check_sum = a_from.m_check_sum;
                m_streamed_class_version = a_from.m_streamed_class_version;
                m_elements = a_from.m_elements;
                return *this;
            }
        public:
        protected:
            ifac& m_fac;
        protected: //Named
            std::string m_name;
            std::string m_title;
        protected:
            unsigned int m_check_sum;    //checksum of original class
            int m_streamed_class_version; //Class version identifier
            //int fNumber;               //!Unique identifier
            ObjArray<streamer_element> m_elements; //Array of TStreamerElements
        };

    }
}

#endif
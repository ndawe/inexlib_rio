// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_tree
#define inlib_rroot_tree

#include "ifac.h"
#include "branch_element.h"
#include "../sout.h"

namespace inlib {
    namespace rroot {

        inline const std::string& TTree_cls()
        {
            static const std::string s_v("TTree");
            return s_v;
        }

        class tree {
        public:
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::rroot::tree");
                return s_v;
            }
        public:
            tree(ifile& a_file, ifac& a_fac)
                : m_file(a_file)
                , m_fac(a_fac)
                , m_out(a_file.out())
                , m_name("")
                , m_title("")
                , m_branches(a_fac, true)
                , m_entries(0)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }
            virtual ~tree()
            {
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            tree(const tree& a_from)
                : m_file(a_from.m_file)
                , m_fac(a_from.m_fac)
                , m_out(a_from.m_out)
                , m_branches(m_fac, false)
            {}
            tree& operator=(const tree&)
            {
                return *this;
            }
        public:
            ifile& file()
            {
                return m_file;
            }

            const std::string& name() const
            {
                return m_name;
            }
            const std::string& title() const
            {
                return m_title;
            }

            const std::vector<branch*>& branches() const
            {
                return m_branches;
            }

            bool find_entry(uint32 a_entry, uint32& a_nbytes)
            {
                a_nbytes = 0;

                if (a_entry >= m_entries) return false;

                int nbytes = 0;
                //fReadEntry = a_entry;
                inlib_vforit(branch*, m_branches, it) {
                    uint32 n;

                    if (!(*it)->find_entry(a_entry, n)) return false;

                    nbytes += n;
                }
                a_nbytes = nbytes;
                return true;
            }

            void dump(std::ostream& a_out, const std::string& a_spaces = "", const std::string& a_indent = " ")
            {
                a_out << a_spaces
                      << "tree :"
                      << " name=" << sout(m_name)
                      << " title=" << sout(m_title)
                      << " entries=" << m_entries
                      << std::endl;
                _dump_branches(a_out, m_branches, a_spaces + a_indent, a_indent);
            }

            branch* find_branch(const std::string& a_name,
                                bool a_recursive = false) const
            {
                return _find_branch(m_branches, a_name, a_recursive);
            }

            std::vector<base_leaf*> find_leaves()
            {
                std::vector<base_leaf*> leaves;
                _find_leaves(m_branches, leaves);
                return leaves;
            }

            std::vector<branch*> find_branches()
            {
                std::vector<branch*> _branches;
                _find_branches(m_branches, _branches);
                return _branches;
            }

            //branch* find_leaf_branch(base_leaf* a_leaf){
            //  return _find_leaf_branch(m_branches,a_leaf);
            //}

            bool show(std::ostream& a_out, uint32 a_entry)
            {
                a_out << "======> EVENT:" << a_entry << std::endl;
                inlib_vforit(branch*, m_branches, it) {
                    if (!(*it)->show(a_out, a_entry)) return false;
                }
                return true;
            }

            uint64 entries() const
            {
                return m_entries;
            }

            bool stream(buffer& a_buffer)
            {
                //uint64 m_tot_bytes;
                //uint64 m_zip_bytes;
                //uint64 m_saved_bytes;
                short vers;
                unsigned int s, c;

                if (!a_buffer.read_version(vers, s, c)) return false;

                //::printf("debug : tree::stream : version %d count %d\n",vers,c);

                //if (vers > 4) {
                //TTree::Class()->ReadBuffer(b, this, vers, s, c);
                //if (fEstimate <= 10000) fEstimate = 1000000;
                //m_saved_bytes = m_tot_bytes;
                //fDirectory = gDirectory;
                //gDirectory->Append(this);
                //return;
                //}

                if (!Named_stream(a_buffer, m_name, m_title)) return false;

                {
                    short color, style, width;

                    if (!AttLine_stream(a_buffer, color, style, width)) return false;
                }
                {
                    short color, style;

                    if (!AttFill_stream(a_buffer, color, style)) return false;
                }

                if (!AttMarker_stream(a_buffer)) return false;

                if (vers <= 4) {
                    int dummy_int;

                    if (!a_buffer.read(dummy_int)) return false; //fScanField

                    if (!a_buffer.read(dummy_int)) return false; //fMaxEntryLoop

                    {
                        int fMaxVirtualSize;

                        if (!a_buffer.read(fMaxVirtualSize)) return false;
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        m_entries = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_tot_bytes = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_zip_bytes = uint64(v);
                    }
                    {
                        int fAutoSave;

                        if (!a_buffer.read(fAutoSave)) return false;
                    }

                    if (!a_buffer.read(dummy_int)) return false; //fEstimate
                } else if (vers <= 9) {
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        m_entries = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_tot_bytes = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_zip_bytes = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_saved_bytes = uint64(v);
                    }
                    int dummy_int;

                    if (!a_buffer.read(dummy_int)) return false; //fTimerInterval

                    if (!a_buffer.read(dummy_int)) return false; //fScanField

                    if (!a_buffer.read(dummy_int)) return false; //fUpdate

                    if (!a_buffer.read(dummy_int)) return false; //fMaxEntryLoop

                    {
                        int fMaxVirtualSize;

                        if (!a_buffer.read(fMaxVirtualSize)) return false;
                    }
                    {
                        int fAutoSave;

                        if (!a_buffer.read(fAutoSave)) return false;
                    }

                    if (!a_buffer.read(dummy_int)) return false; //fEstimate
                } else if (vers < 16) { //FIXME : what is the exact version ?
                    double dummy_double;
                    int dummy_int;
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        m_entries = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_tot_bytes = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_zip_bytes = uint64(v);
                    }
                    {
                        double v;

                        if (!a_buffer.read(v)) return false;

                        //m_saved_bytes = uint64(v);
                    }

                    if (!a_buffer.read(dummy_double)) return false; //fWeight

                    if (!a_buffer.read(dummy_int)) return false; //fTimerInterval

                    if (!a_buffer.read(dummy_int)) return false; //fScanField

                    if (!a_buffer.read(dummy_int)) return false; //fUpdate

                    if (!a_buffer.read(dummy_int)) return false; //fMaxEntryLoop

                    {
                        int fMaxVirtualSize;

                        if (!a_buffer.read(fMaxVirtualSize)) return false;
                    }
                    {
                        int fAutoSave;

                        if (!a_buffer.read(fAutoSave)) return false;
                    }

                    if (!a_buffer.read(dummy_int)) return false; //fEstimate
                } else { //vers>=16
                    double dummy_double;
                    int dummy_int;
                    int64 dummy_int64;
                    {
                        uint64 v;

                        if (!a_buffer.read(v)) return false;

                        m_entries = v;
                    }
                    {
                        uint64 v;

                        if (!a_buffer.read(v)) return false;

                        //m_tot_bytes = v;
                    }
                    {
                        uint64 v;

                        if (!a_buffer.read(v)) return false;

                        //m_zip_bytes = v;
                    }
                    {
                        uint64 v;

                        if (!a_buffer.read(v)) return false;

                        //m_saved_bytes = v;
                    }

                    if (vers >= 18) {
                        if (!a_buffer.read(dummy_int64)) return false; //fFlushedBytes
                    }

                    if (!a_buffer.read(dummy_double)) return false; //fWeight

                    if (!a_buffer.read(dummy_int)) return false; //fTimerInterval

                    if (!a_buffer.read(dummy_int)) return false; //fScanField

                    if (!a_buffer.read(dummy_int)) return false; //fUpdate

                    if (vers >= 18) {
                        if (!a_buffer.read(dummy_int)) return false; //fDefaultEntryOffsetLen
                    }

                    if (!a_buffer.read(dummy_int64)) return false; //fMaxEntries

                    if (!a_buffer.read(dummy_int64)) return false; //fMaxEntryLoop

                    {
                        uint64 fMaxVirtualSize;

                        if (!a_buffer.read(fMaxVirtualSize)) return false;
                    }
                    {
                        uint64 fAutoSave;

                        if (!a_buffer.read(fAutoSave)) return false;
                    }

                    if (vers >= 18) {
                        if (!a_buffer.read(dummy_int64)) return false; //fAutoFlush
                    }

                    if (!a_buffer.read(dummy_int64)) return false; //fEstimate
                }

                //FIXME if (fEstimate <= 10000) fEstimate = 1000000;
                //TObjArray
                //The below m_branches.read will create leaves.
                //::printf("debug : tree : read branches : begin\n");
                {
                    ifac::args args;

                    if (!m_branches.stream(a_buffer, args)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read branches."
                              << std::endl;
                        return false;
                    }
                }
                //::printf("debug : tree : read branches : end\n");
                //TObjArray
                // We read leaves in order to keep streaming synchronisation.
                // In fact m_leaves are references to existing leaves read by
                // the branches in the upper line of code.
                //::printf("debug : tree : read leaves : begin\n");
                {
                    ObjArray<base_leaf> m_leaves(m_fac, true);
                    branch b(m_file, m_fac);
                    ifac::args args;
                    args[ifac::arg_branch()] = &b;

                    if (!m_leaves.stream(a_buffer, args)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read leaves."
                              << std::endl;
                        return false;
                    }
                }
                //::printf("debug : tree : read leaves : end\n");

                if (vers >= 10) {
                    //TList* fAliases
                    if (!dummy_TXxx_pointer_stream(a_buffer, m_fac)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read fAliases."
                              << std::endl;
                        return false;
                    }
                }

                //m_saved_bytes = m_tot_bytes;
                {
                    std::vector<double> v;

                    if (!Array_stream<double>(a_buffer, v)) return false;
                } //fIndexValues TArrayD
                {
                    std::vector<int> v;

                    if (!Array_stream<int>(a_buffer, v)) return false;
                }    // fIndex (TArrayI).

                if (vers >= 16) {
                    //TVirtualIndex* fTreeIndex //FIXME ???
                    if (!dummy_TXxx_pointer_stream(a_buffer, m_fac)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read fTreeIndex."
                              << std::endl;
                        return false;
                    }
                }

                if (vers >= 6) {
                    //TList* fFriends
                    if (!dummy_TXxx_pointer_stream(a_buffer, m_fac)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read fFriends."
                              << std::endl;
                        return false;
                    }
                }

                if (vers >= 16) {
                    //TList* fUserInfo
                    if (!dummy_TXxx_pointer_stream(a_buffer, m_fac)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read fUserInfo."
                              << std::endl;
                        return false;
                    }

                    //TBranchRef* fBranchRef
                    if (!dummy_TXxx_pointer_stream(a_buffer, m_fac)) {
                        m_out << "inlib::rroot::tree::stream : "
                              << "can't read fBranchRef."
                              << std::endl;
                        return false;
                    }
                }

                if (!a_buffer.check_byte_count(s, c, TTree_cls())) return false;

                return true;
            }
        protected:
            void _dump_branches(std::ostream& a_out,
                                const std::vector<branch*>& a_bs,
                                const std::string& a_spaces = "",
                                const std::string& a_indent = " ")
            {
                inlib_vforcit(branch*, a_bs, it) {
                    if (branch_element* be = safe_cast<branch, branch_element>(*(*it))) {
                        a_out << a_spaces
                              << "branch_element :"
                              << " name=" << sout((*it)->name())
                              << " title=" << sout((*it)->title())
                              << " entry_number=" << be->entry_number()
                              << " ref_cls=" << sout(be->class_name())
                              << " (type=" << be->type()
                              << ",id=" << be->id()
                              << ",stype=" << be->streamer_type()
                              << ")."
                              << std::endl;
                    } else {
                        a_out << a_spaces
                              << "branch :"
                              << " name=" << sout((*it)->name())
                              << " title=" << sout((*it)->title())
                              << " entry_number=" << (*it)->entry_number()
                              << std::endl;
                    }

                    {
                        const std::vector<base_leaf*>& lvs = (*it)->leaves();
                        inlib_vforcit(base_leaf*, lvs, itl) {
                            a_out << a_spaces << a_indent
                                  << "leave :"
                                  << " name=" << sout((*itl)->name())
                                  << " title=" << sout((*itl)->title())
                                  << " cls=" << sout((*itl)->s_cls())
                                  << std::endl;
                        }
                    }

                    _dump_branches(a_out, (*it)->branches(), a_spaces + a_indent, a_indent);
                }
            }

            branch* _find_branch(const std::vector<branch*>& a_bs,
                                 const std::string& a_name,
                                 bool a_recursive) const
            {
                inlib_vforcit(branch*, a_bs, it) {
                    if (inlib::rcmp((*it)->name(), a_name)) return *it;

                    if (a_recursive) {
                        branch* br = _find_branch((*it)->branches(), a_name, a_recursive);

                        if (br) return br;
                    }
                }
                return 0;
            }

            void _find_leaves(const std::vector<branch*>& a_bs,
                              std::vector<base_leaf*>& a_leaves)
            {
                inlib_vforcit(branch*, a_bs, it) {
                    {
                        const std::vector<base_leaf*>& lvs = (*it)->leaves();
                        inlib_vforcit(base_leaf*, lvs, itl) {
                            a_leaves.push_back(*itl);
                        }
                    }
                    _find_leaves((*it)->branches(), a_leaves);
                }
            }

            void _find_branches(const std::vector<branch*>& a_bs,
                                std::vector<branch*>& a_branches)
            {
                inlib_vforcit(branch*, a_bs, it) {
                    a_branches.push_back(*it);
                    _find_branches((*it)->branches(), a_branches);
                }
            }

            //branch* _find_leaf_branch(const std::vector<branch*>& a_bs,
            //                                 base_leaf* a_leaf){
            //  std::vector<branch*>::const_iterator it;
            //  for(it=a_bs.begin();it!=a_bs.end();++it) {
            //   {const std::vector<base_leaf*>& lvs = (*it)->leaves();
            //    std::vector<base_leaf*>::const_iterator itl;
            //    for(itl=lvs.begin();itl!=lvs.end();++itl) {
            //      if(*itl==a_leaf) return *it;
            //    }}
            //   {branch* br = _find_leaf_branch((*it)->branches(),a_leaf);
            //    if(br) return br;}
            //  }
            //  return 0;
            //}
        protected:
            ifile& m_file;
            ifac& m_fac;
            std::ostream& m_out;
            //Named
            std::string m_name;
            std::string m_title;

            ObjArray<branch> m_branches;
            uint64 m_entries;   // Number of entries
        };

    }
}

#endif

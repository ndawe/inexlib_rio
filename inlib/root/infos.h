// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_infos
#define inlib_root_infos

#include "info.h"

namespace inlib {
    namespace root {

        inline void scs(unsigned int& a_cs, const std::string& a_s)
        {
            unsigned int l = a_s.size();

            for (unsigned int i = 0; i < l; i++) a_cs = a_cs * 3 + a_s[i];
        }

        inline void acs(unsigned int& a_cs, int a_n, int* a_dims)
        {
            for (int i = 0; i < a_n; i++) a_cs = a_cs * 3 + (unsigned int)a_dims[i];
        }

        inline void fill_vec(List<StreamerInfo>& a_infos,
                             const std::string& a_type,
                             streamer_info::Type a_si_type)
        {
            unsigned int check = 196608;
            StreamerInfo* info = new StreamerInfo(std::string("vector<") + a_type + ">", 4, check);
            a_infos.push_back(info);
            info->add(new streamer_STL("This", "Used to call the proper TStreamerInfo case", 0, a_si_type, std::string("vector<") + a_type + ">"));
        }

        inline int size_VIRTUAL()
        {
            return 4;
        }

        inline int size_TObject()
        {
            return 12;
        }
        inline int size_TNamed()
        {
            return 28;
        }

        inline void fill_infos_core(List<StreamerInfo>& a_infos, std::ostream& a_out)
        {
            // sizeof(TString) = 8   (4 (virtual ~) + 1 + 3 (align))
            // sizeof(TObject) = 12
            // sizeof(TNamed)  = 28
            // sizeof(TObjArray) = 40
            // sizeof(TArray) = 8
            // sizeof(TArrayI) = 12
            // sizeof(TArrayD) = 12
            // sizeof(TArrayF) = 12
            const int size_POINTER = 4;
            //const int size_OBJECT_POINTER = 4;
            //const int size_COUNTER = 4;
            const int size_TArray = 8;
            short TArray_version = 1;
            short TObject_version = 1;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TObject");
                //members :
                scs(check, "fUniqueID");
                scs(check, "UInt_t");
                scs(check, "fBits");
                scs(check, "UInt_t");
                StreamerInfo* info = new StreamerInfo("TObject", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = size_VIRTUAL();
                info->add(new streamer_uint(ofs, "fUniqueID", "object unique identifier"));
                info->add(new streamer_uint(ofs, "fBits", "bit field status word"));

                if (ofs != size_TObject()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TObject " << ofs << " (" << size_TObject() << " expected.)"
                          << std::endl;
                }
            }
            short TNamed_version = 1;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TNamed");
                //base :
                scs(check, "TObject");
                //members :
                scs(check, "fName");
                scs(check, "TString");
                scs(check, "fTitle");
                scs(check, "TString");
                StreamerInfo* info = new StreamerInfo("TNamed", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TObject", "Basic ROOT object", ofs, TObject_version));
                ofs += size_TObject();
                info->add(new streamer_string(ofs, "fName", "object identifier"));
                info->add(new streamer_string(ofs, "fTitle", "object title"));

                if (ofs != size_TNamed()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TNamed " << ofs << " (" << size_TNamed() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerInfo");
                //bases :
                scs(check, "TNamed");
                //members :
                scs(check, "fCheckSum");
                scs(check, "UInt_t");
                scs(check, "fClassVersion");
                scs(check, "Int_t");
                scs(check, "fElements");
                scs(check, "TObjArray*");
                StreamerInfo* info = new StreamerInfo("TStreamerInfo", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", 0, TNamed_version)); //28
                info->add(new streamer_uint("fCheckSum", "checksum of original class", 28)); //4
                info->add(new streamer_int("fClassVersion", "Class version identifier", 32)); //4
                //40 = 2*int+4*int_p+2*ulong_p+bool+p (bool=4 !)
                info->add(new streamer_object_pointer("fElements", "Array of TStreamerElements", 76, "TObjArray*")); //4
                //80
            }
            short TStreamerElement_version = 2;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerElement");
                //bases :
                scs(check, "TNamed");
                //members :
                scs(check, "fType");
                scs(check, "Int_t");
                scs(check, "fSize");
                scs(check, "Int_t");
                scs(check, "fArrayLength");
                scs(check, "Int_t");
                scs(check, "fArrayDim");
                scs(check, "Int_t");
                scs(check, "fMaxIndex");
                scs(check, "Int_t");
                int dim = 5;
                acs(check, 1, &dim);
                scs(check, "fTypeName");
                scs(check, "TString");
                //Should be : 2369818458U
                StreamerInfo* info = new StreamerInfo("TStreamerElement", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", 0, TNamed_version));
                info->add(new streamer_int("fType", "element type", 28));
                info->add(new streamer_int("fSize", "sizeof element", 32));
                info->add(new streamer_int("fArrayLength", "cumulative size of all array dims", 36));
                info->add(new streamer_int("fArrayDim", "number of array dimensions", 40));
                {
                    streamer_element* elem = new streamer_int("fMaxIndex", "Maximum array index for array dimension \"dim\"", 44);
                    info->add(elem);
                    elem->setArrayDimension(1);
                    elem->setMaxIndex(0, 5);
                }
                info->add(new streamer_string("fTypeName", "Data type name of data member", 72));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerBase");
                //bases :
                scs(check, "TStreamerElement");
                //members :
                scs(check, "fBaseVersion");
                scs(check, "Int_t");
                //Should be : 2671078514U
                StreamerInfo* info = new StreamerInfo("TStreamerBase", 3, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
                info->add(new streamer_int("fBaseVersion", "version number of the base class", 88));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerString");
                //bases :
                scs(check, "TStreamerElement");
                //Should be : 2525579865U
                StreamerInfo* info = new StreamerInfo("TStreamerString", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerBasicType");
                //bases :
                scs(check, "TStreamerElement");
                //Should be : 3001875966U;
                StreamerInfo* info = new StreamerInfo("TStreamerBasicType", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerBasicPointer");
                //bases :
                scs(check, "TStreamerElement");
                //members :
                scs(check, "fCountVersion");
                scs(check, "Int_t");
                scs(check, "fCountName");
                scs(check, "TString");
                scs(check, "fCountClass");
                scs(check, "TString");
                //Should be : 1587298059U
                StreamerInfo* info = new StreamerInfo("TStreamerBasicPointer", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
                info->add(new streamer_int("fCountVersion", "version number of the class with the counter", 88));
                info->add(new streamer_string("fCountName", "name of data member holding the array count", 92));
                info->add(new streamer_string("fCountClass", "name of the class with the counter", 100));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerObject");
                //bases :
                scs(check, "TStreamerElement");
                //Should be : 2177456715U
                StreamerInfo* info = new StreamerInfo("TStreamerObject", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerObjectPointer");
                //bases :
                scs(check, "TStreamerElement");
                //Should be : 720556968U
                StreamerInfo* info = new StreamerInfo("TStreamerObjectPointer", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TStreamerObjectAny");
                //bases :
                scs(check, "TStreamerElement");
                //Should be : 3108880127U
                StreamerInfo* info = new StreamerInfo("TStreamerObjectAny", 2, check);
                a_infos.push_back(info);
                // Elements :
                info->add(new streamer_base("TStreamerElement", "base class for one element (data member) to be Streamed", 0, TStreamerElement_version));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TArray");
                //members :
                scs(check, "fN");
                scs(check, "Int_t");
                StreamerInfo* info = new StreamerInfo("TArray", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                ofs += size_VIRTUAL();
                info->add(new streamer_int(ofs, "fN", "Number of array elements"));
                //size_TArray = ofs; //8
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TArrayI");
                //base :
                scs(check, "TArray");
                //members :
                scs(check, "fArray");
                scs(check, "Int_t*");
                StreamerInfo* info = new StreamerInfo("TArrayI", 1, check);
                a_infos.push_back(info);
                // Elements :
                int offset = 0;
                info->add(new streamer_base("TArray", "Abstract array base class", offset, TArray_version));
                offset += size_TArray;
                info->add(new streamer_basic_pointer("fArray", "[fN] Array of fN integers", offset, streamer_info::INT, "fN", "TArray", 1, "Int_t*"));
                offset += size_POINTER;
                //12
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TArrayD");
                //base :
                scs(check, "TArray");
                //members :
                scs(check, "fArray");
                scs(check, "Double_t*");
                StreamerInfo* info = new StreamerInfo("TArrayD", 1, check);
                a_infos.push_back(info);
                // Elements :
                int offset = 0;
                info->add(new streamer_base("TArray", "Abstract array base class", offset, TArray_version));
                offset += size_TArray; //8
                info->add(new streamer_basic_pointer("fArray", "[fN] Array of fN integers", offset, streamer_info::DOUBLE, "fN", "TArray", 1, "Double_t*"));
                offset += size_POINTER;
                //12
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TArrayF");
                //base :
                scs(check, "TArray");
                //members :
                scs(check, "fArray");
                scs(check, "Float_t*");
                StreamerInfo* info = new StreamerInfo("TArrayF", 1, check);
                a_infos.push_back(info);
                // Elements :
                int offset = 0;
                info->add(new streamer_base("TArray", "Abstract array base class", offset, TArray_version));
                offset += size_TArray;
                info->add(new streamer_basic_pointer("fArray", "[fN] Array of fN integers", offset, streamer_info::FLOAT, "fN", "TArray", 1, "Float_t*"));
                offset += size_POINTER;
                //12
            }
            fill_vec(a_infos, "char", streamer_info::CHAR);
            fill_vec(a_infos, "short", streamer_info::SHORT);
            fill_vec(a_infos, "int", streamer_info::INT);
            fill_vec(a_infos, "unsigned char", streamer_info::UNSIGNED_CHAR);
            fill_vec(a_infos, "unsigned short", streamer_info::UNSIGNED_SHORT);
            fill_vec(a_infos, "unsigned int", streamer_info::UNSIGNED_INT);
            fill_vec(a_infos, "float", streamer_info::FLOAT);
            fill_vec(a_infos, "double", streamer_info::DOUBLE);
            fill_vec(a_infos, "bool", streamer_info::BOOL);
        }

        inline void fill_infos_cont(List<StreamerInfo>& a_infos, std::ostream&)
        {
            // sizeof(TString) = 8   (4 (virtual ~) + 1 + 3 (align))
            // sizeof(TObject) = 12
            short TObject_version = 1;
            const int size_POINTER = 4;
            const int size_BOOL = 4;
            int size_TCollection = 0;
            short TCollection_version = 3;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TCollection");
                //base :
                scs(check, "TObject");
                //members :
                scs(check, "fName");
                scs(check, "TString");
                scs(check, "fSize");
                scs(check, "Int_t");
                StreamerInfo* info = new StreamerInfo("TCollection", TCollection_version, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TObject", "Basic ROOT object", ofs, TObject_version));
                ofs += size_TObject();
                info->add(new streamer_string(ofs, "fName", "name of the collection"));
                info->add(new streamer_int(ofs, "fSize", "number of elements in collection"));
                size_TCollection = ofs;
            }
            int size_TSeqCollection = 0;
            short TSeqCollection_version = 0;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TSeqCollection");
                //base :
                scs(check, "TCollection");
                //members :
                scs(check, "fSorted");
                scs(check, "Bool_t");
                StreamerInfo* info = new StreamerInfo("TSeqCollection", TSeqCollection_version, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TCollection", "Collection abstract base class", ofs, TCollection_version));
                ofs += size_TCollection;
                info->add(new streamer_bool(ofs, "fSorted", " true if collection has been sorted"));
                size_TSeqCollection = ofs;
            }
            //int size_TList = 0;
            short TList_version = 4;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TList");
                //base :
                scs(check, "TSeqCollection");
                //members :
                StreamerInfo* info = new StreamerInfo("TList", TList_version, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TSeqCollection", "Sequenceable collection abstract base class", ofs, TSeqCollection_version));
                ofs += size_TSeqCollection;
                ofs += size_POINTER; //!*fFirst
                ofs += size_POINTER; //!*fLast
                ofs += size_POINTER; //!*fCache
                ofs += size_BOOL;    //!fAscending
                //size_TList = ofs;
            }
        }

        inline int size_TAttLine()
        {
            return 10;   //12?
        }
        inline int size_TAttFill()
        {
            return 8;
        }
        inline int size_TAttMarker()
        {
            return 12;
        }

        inline void fill_infos_graf(List<StreamerInfo>& a_infos, std::ostream& a_out)
        {
            //sizeof(Font_t) = 2
            //sizeof(Style_t) = 2
            //sizeof(Marker_t) = 2
            //sizeof(Width_t) = 2
            //sizeof(Size_t) = 4
            const int size_SHORT = 2;
            const int size_FLOAT = 4;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TAttLine");
                //members :
                scs(check, "fLineColor");
                scs(check, "Color_t");
                scs(check, "fLineStyle");
                scs(check, "Style_t");
                scs(check, "fLineWidth");
                scs(check, "Width_t");
                //Should be : 1369587346U
                // Beurk ; but the ROOT TTree compells indirectly the below.
                StreamerInfo* info = new StreamerInfo("TAttLine", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = size_VIRTUAL();
                info->add(new streamer_basic_type("fLineColor", "line color", ofs, streamer_info::SHORT, "Color_t"));
                ofs += size_SHORT;
                info->add(new streamer_basic_type("fLineStyle", "line style", ofs, streamer_info::SHORT, "Style_t"));
                ofs += size_SHORT;
                info->add(new streamer_basic_type("fLineWidth", "line width", ofs, streamer_info::SHORT, "Width_t"));
                ofs += size_SHORT;

                //2 (alignement ???)
                if (ofs != size_TAttLine()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TAttLine " << ofs << " (" << size_TAttLine() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TAttFill");
                //members :
                scs(check, "fFillColor");
                scs(check, "Color_t");
                scs(check, "fFillStyle");
                scs(check, "Style_t");
                //Should be : 1204118360U
                StreamerInfo* info = new StreamerInfo("TAttFill", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = size_VIRTUAL();
                info->add(new streamer_basic_type("fFillColor", "fill area color", ofs, streamer_info::SHORT, "Color_t"));
                ofs += size_SHORT;
                info->add(new streamer_basic_type("fFillStyle", "fill area style", ofs, streamer_info::SHORT, "Style_t"));
                ofs += size_SHORT;

                if (ofs != size_TAttFill()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TAttFill " << ofs << " (" << size_TAttFill() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TAttMarker");
                //members :
                scs(check, "fMarkerColor");
                scs(check, "Color_t");
                scs(check, "fMarkerStyle");
                scs(check, "Style_t");
                scs(check, "fMarkerSize");
                scs(check, "Size_t");
                //Should be 4207747460U
                StreamerInfo* info = new StreamerInfo("TAttMarker", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = size_VIRTUAL();
                info->add(new streamer_basic_type("fMarkerColor", "Marker color index", ofs, streamer_info::SHORT, "Color_t"));
                ofs += size_SHORT;
                info->add(new streamer_basic_type("fMarkerStyle", "Marker style", ofs, streamer_info::SHORT, "Style_t"));
                ofs += size_SHORT;
                info->add(new streamer_basic_type("fMarkerSize", "Marker size", ofs, streamer_info::FLOAT, "Size_t"));
                ofs += size_FLOAT;

                if (ofs != size_TAttMarker()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TAttMarker " << ofs << " (" << size_TAttMarker() << " expected.)"
                          << std::endl;
                }
            }
        }

        inline int size_TBranch()
        {
            return 264;   //256?
        }
        inline int size_TLeaf()
        {
            return 60;   //52?
        }
        inline int size_TTree()
        {
            return 254;   //240?
        }

        inline void fill_infos_tree(List<StreamerInfo>& a_infos, std::ostream& a_out)
        {
            const int size_CHAR = 1;
            const int size_INT = 4;
            const int size_POINTER = 4;
            const int size_COUNTER = 4;
            const int size_TObjArray = 40;
            const int size_TArrayD = 12;
            const int size_TArrayI = 12;
            short TNamed_version = 1;
            short TAttLine_version = 1;
            short TAttFill_version = 1;
            short TAttMarker_version = 1;
            short TBranch_version = 8;
            short TLeaf_version = 2;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TTree");
                //bases :
                scs(check, "TNamed");
                scs(check, "TAttLine");
                scs(check, "TAttFill");
                scs(check, "TAttMarker");
                //members :
                scs(check, "fEntries");
                scs(check, "Stat_t");
                scs(check, "fTotBytes");
                scs(check, "Stat_t");
                scs(check, "fZipBytes");
                scs(check, "Stat_t");
                scs(check, "fSavedBytes");
                scs(check, "Stat_t");
                scs(check, "fTimerInterval");
                scs(check, "Int_t");
                scs(check, "fScanField");
                scs(check, "Int_t");
                scs(check, "fUpdate");
                scs(check, "Int_t");
                scs(check, "fMaxEntryLoop");
                scs(check, "Int_t");
                scs(check, "fMaxVirtualSize");
                scs(check, "Int_t");
                scs(check, "fAutoSave");
                scs(check, "Int_t");
                scs(check, "fEstimate");
                scs(check, "Int_t");
                scs(check, "fBranches");
                scs(check, "TObjArray");
                scs(check, "fLeaves");
                scs(check, "TObjArray");
                //scs(check,"fAliases");
                //scs(check,"TList*");
                scs(check, "fIndexValues");
                scs(check, "TArrayD");
                scs(check, "fIndex");
                scs(check, "TArrayI");
                //scs(check,"fFriends");
                //scs(check,"TList*");
                //Should be : 3245044844U //3.00.06
                //Should be : FIXME //3.10.02
                StreamerInfo* info = new StreamerInfo("TTree", 5, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", ofs, TNamed_version));
                ofs += size_TNamed();
                info->add(new streamer_base("TAttLine", "Line attributes", ofs, TAttLine_version));
                ofs += size_TAttLine();
                info->add(new streamer_base("TAttFill", "Fill area attributes", ofs, TAttFill_version));
                ofs += size_TAttFill();
                info->add(new streamer_base("TAttMarker", "Marker attributes", ofs, TAttMarker_version));
                ofs += size_TAttMarker();
                info->add(new streamer_stat_t(ofs, "fEntries", "Number of entries"));
                info->add(new streamer_stat_t(ofs, "fTotBytes", "Total number of bytes in all branches before compression"));
                info->add(new streamer_stat_t(ofs, "fZipBytes", "Total number of bytes in all branches after compression"));
                info->add(new streamer_stat_t(ofs, "fSavedBytes", "Number of autosaved bytes"));
                info->add(new streamer_int(ofs, "fTimerInterval", "Timer interval in milliseconds"));
                info->add(new streamer_int(ofs, "fScanField", "Number of runs before prompting in Scan"));
                info->add(new streamer_int(ofs, "fUpdate", "Update frequency for EntryLoop"));
                info->add(new streamer_int(ofs, "fMaxEntryLoop", "Maximum number of entries to process"));
                info->add(new streamer_int(ofs, "fMaxVirtualSize", "Maximum total size of buffers kept in memory"));
                info->add(new streamer_int(ofs, "fAutoSave", "Autosave tree when fAutoSave bytes produced"));
                info->add(new streamer_int(ofs, "fEstimate", "Number of entries to estimate histogram limits"));
                ofs += size_INT; //!fChainOffset
                ofs += size_INT; //!fReadEntry
                ofs += size_INT; //!fTotalBuffers
                ofs += size_INT; //!fPacketSize
                ofs += size_INT; //!fNfill
                ofs += size_POINTER; //!*fDirectory
                info->add(new streamer_object("fBranches", "List of Branches", ofs, "TObjArray"));
                ofs += size_TObjArray;
                info->add(new streamer_object("fLeaves", "Direct pointers to individual branch leaves", ofs, "TObjArray"));
                ofs += size_TObjArray;
                ofs += size_POINTER; //!*fEventList
                info->add(new streamer_object_any("fIndexValues", "Sorted index values", ofs, "TArrayD"));
                ofs += size_TArrayD;
                info->add(new streamer_object_any("fIndex", "Index of sorted values", ofs, "TArrayI"));
                ofs += size_TArrayI;
                ofs += size_POINTER; //!*fPlayer

                if (ofs != size_TTree()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TTree " << ofs << " (" << size_TTree() << " expected.)"
                          << std::endl;
                }

                //info->add(new streamer_object_pointer("fAliases","List of aliases for expressions based on the tree branches.",offset,"TList*"));offset += size_OBJECT_POINTER;
                //info->add(new streamer_object_pointer("fFriends","pointer to list of friend elements",offset,"TList*"));offset += size_OBJECT_POINTER;
            }
            //---------------------------------------------------------------------
            {
                unsigned int check = 0;
                //this :
                scs(check, "TBranch");
                //bases :
                scs(check, "TNamed");
                scs(check, "TAttFill");
                //members :
                scs(check, "fCompress");
                scs(check, "Int_t");
                scs(check, "fBasketSize");
                scs(check, "Int_t");
                scs(check, "fEntryOffsetLen");
                scs(check, "Int_t");
                scs(check, "fWriteBasket");
                scs(check, "Int_t");
                scs(check, "fEntryNumber");
                scs(check, "Int_t");
                scs(check, "fOffset");
                scs(check, "Int_t");
                scs(check, "fMaxBaskets");
                scs(check, "Int_t");
                scs(check, "fSplitLevel");
                scs(check, "Int_t");
                scs(check, "fEntries");
                scs(check, "Stat_t");
                scs(check, "fTotBytes");
                scs(check, "Stat_t");
                scs(check, "fZipBytes");
                scs(check, "Stat_t");
                scs(check, "fBranches");
                scs(check, "TObjArray");
                scs(check, "fLeaves");
                scs(check, "TObjArray");
                scs(check, "fBaskets");
                scs(check, "TObjArray");
                scs(check, "fBasketBytes");
                scs(check, "Int_t*");
                scs(check, "fBasketEntry");
                scs(check, "Int_t*");
                scs(check, "fBasketSeek");
                scs(check, "Seek_t*");
                scs(check, "fFileName");
                scs(check, "TString");
                //Should be : 2056727376U (6 3.00.06)
                //Should be : FIXME (7 3.03.01)
                StreamerInfo* info = new StreamerInfo("TBranch", 8, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", ofs, TNamed_version));
                ofs += size_TNamed();
                info->add(new streamer_base("TAttFill", "Fill area attributes", ofs, TAttFill_version));
                ofs += size_TAttFill();
                info->add(new streamer_int(ofs, "fCompress", "(=1 branch is compressed, 0 otherwise)"));
                info->add(new streamer_int(ofs, "fBasketSize", "Initial Size of  Basket Buffer"));
                info->add(new streamer_int(ofs, "fEntryOffsetLen", "Initial Length of fEntryOffset table in the basket buffers"));
                info->add(new streamer_int(ofs, "fWriteBasket", "Last basket number written"));
                info->add(new streamer_int(ofs, "fEntryNumber", "Current entry number (last one filled in this branch)"));
                info->add(new streamer_int(ofs, "fOffset", "Offset of this branch"));
                info->add(new streamer_basic_type("fMaxBaskets", "Maximum number of Baskets so far", ofs, streamer_info::COUNTER, "Int_t"));
                ofs += size_COUNTER;
                info->add(new streamer_int(ofs, "fSplitLevel", "Branch split level")); //3.03.01
                ofs += size_INT; //!fNLeaves
                ofs += size_INT; //!fReadBasket
                ofs += size_INT; //!fReadEntry
                info->add(new streamer_stat_t(ofs, "fEntries", "Number of entries"));
                info->add(new streamer_stat_t(ofs, "fTotBytes", "Total number of bytes in all leaves before compression"));
                info->add(new streamer_stat_t(ofs, "fZipBytes", "Total number of bytes in all leaves after compression"));
                info->add(new streamer_object("fBranches", "-> List of Branches of this branch", ofs, "TObjArray"));
                ofs += size_TObjArray;
                info->add(new streamer_object("fLeaves", "-> List of leaves of this branch", ofs, "TObjArray"));
                ofs += size_TObjArray;
                info->add(new streamer_object("fBaskets", "-> List of baskets of this branch", ofs, "TObjArray"));
                ofs += size_TObjArray;
                ofs += size_INT; //!fNBasketRAM
                ofs += size_POINTER; //!*fBasketRAM
                info->add(new streamer_basic_pointer("fBasketBytes", "[fMaxBaskets] Lenght of baskets on file", ofs, streamer_info::INT, "fMaxBaskets", "TBranch", 6, "Int_t*"));
                ofs += size_POINTER;
                info->add(new streamer_basic_pointer("fBasketEntry", "[fMaxBaskets] Table of first entry in eack basket", ofs, streamer_info::INT, "fMaxBaskets", "TBranch", 6, "Int_t*"));
                ofs += size_POINTER;
                info->add(new streamer_basic_pointer("fBasketSeek", "[fMaxBaskets] Addresses of baskets on file", ofs, streamer_info::INT, "fMaxBaskets", "TBranch", 6, "Seek_t*"));
                ofs += size_POINTER;
                ofs += size_POINTER; //!*fTree
                ofs += size_POINTER; //!*fAddress
                ofs += size_POINTER; //!*fDirectory
                info->add(new streamer_string(ofs, "fFileName", "Name of file where buffers are stored (\"\" if in same file as Tree header)"));

                if (ofs != size_TBranch()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TBranch " << ofs << " (" << size_TBranch() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TBranchObject");
                //bases :
                scs(check, "TBranch");
                //members :
                scs(check, "fClassName");
                scs(check, "TString");
                //Should be : 2857878535U
                StreamerInfo* info = new StreamerInfo("TBranchObject", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TBranch", "Branch descriptor", ofs, TBranch_version));
                ofs += size_TBranch();
                info->add(new streamer_string(ofs, "fClassName", "Class name of referenced object"));
            }
            //---------------------------------------------------------------------
            {
                unsigned int check = 0;
                //this :
                scs(check, "TBranchElement");
                //bases :
                scs(check, "TBranch");
                //members :
                scs(check, "fClassName");
                scs(check, "TString");
                scs(check, "fClassVersion");
                scs(check, "Int_t");
                scs(check, "fID");
                scs(check, "Int_t");
                scs(check, "fType");
                scs(check, "Int_t");
                scs(check, "fStreamerType");
                scs(check, "Int_t");
                StreamerInfo* info = new StreamerInfo("TBranchElement", 1, check);
                a_infos.push_back(info);
                // Elements :
                int offset = 0;
                info->add(new streamer_base("TBranch", "Branch descriptor", offset, TBranch_version));
                offset += size_TBranch();
                info->add(new streamer_string(offset, "fClassName", "Class name of referenced object"));
                info->add(new streamer_int(offset, "fClassVersion", "Version number of class"));
                info->add(new streamer_int(offset, "fID", "element serial number in fInfo"));
                info->add(new streamer_int(offset, "fType", "branch type"));
                info->add(new streamer_int(offset, "fStreamerType", "branch streamer type"));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeaf");
                //bases :
                scs(check, "TNamed");
                //members :
                scs(check, "fLen");
                scs(check, "Int_t");
                scs(check, "fLenType");
                scs(check, "Int_t");
                scs(check, "fOffset");
                scs(check, "Int_t");
                scs(check, "fIsRange");
                scs(check, "Bool_t");
                scs(check, "fIsUnsigned");
                scs(check, "Bool_t");
                scs(check, "fLeafCount");
                scs(check, "TLeaf*");
                //Should be : 727988519U
                StreamerInfo* info = new StreamerInfo("TLeaf", 2, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", ofs, TNamed_version));
                ofs += size_TNamed();
                ofs += size_INT; //!fNdata
                info->add(new streamer_int(ofs, "fLen", "Number of fixed length elements"));
                info->add(new streamer_int(ofs, "fLenType", "Number of bytes for this data type"));
                info->add(new streamer_int(ofs, "fOffset", "Offset in ClonesArray object (if one)"));
                info->add(new streamer_bool(ofs, "fIsRange", "(=kTRUE if leaf has a range, kFALSE otherwise)"));
                info->add(new streamer_bool(ofs, "fIsUnsigned", "(=kTRUE if unsigned, kFALSE otherwise)"));
                info->add(new streamer_object_pointer("fLeafCount", "Pointer to Leaf count if variable length", ofs, "TLeaf*"));
                ofs += size_POINTER;
                ofs += size_POINTER; //!*fBranch

                if (ofs != size_TLeaf()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TLeaf " << ofs << " (" << size_TLeaf() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafS");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fMinimum");
                scs(check, "Short_t");
                scs(check, "fMaximum");
                scs(check, "Short_t");
                //Should be : FIXME
                StreamerInfo* info = new StreamerInfo("TLeafS", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_short(ofs, "fMinimum", "Minimum value if leaf range is specified"));
                info->add(new streamer_short(ofs, "fMaximum", "Maximum value if leaf range is specified"));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafI");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fMinimum");
                scs(check, "Int_t");
                scs(check, "fMaximum");
                scs(check, "Int_t");
                //Should be : 3495201397U
                StreamerInfo* info = new StreamerInfo("TLeafI", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_int(ofs, "fMinimum", "Minimum value if leaf range is specified"));
                info->add(new streamer_int(ofs, "fMaximum", "Maximum value if leaf range is specified"));
                //size_TLeafI = ofs;
            }
            {
                unsigned int check = 0;
                //name :
                scs(check, "TLeafF");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fMinimum");
                scs(check, "Float_t");
                scs(check, "fMaximum");
                scs(check, "Float_t");
                //Should be 1366318032U
                StreamerInfo* info = new StreamerInfo("TLeafF", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_float(ofs, "fMinimum", "Minimum value if leaf range is specified"));
                info->add(new streamer_float(ofs, "fMaximum", "Maximum value if leaf range is specified"));
                //size_TLeafF = ofs;
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafD");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fMinimum");
                scs(check, "Double_t");
                scs(check, "fMaximum");
                scs(check, "Double_t");
                //Should be
                StreamerInfo* info = new StreamerInfo("TLeafD", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_double(ofs, "fMinimum", "Minimum value if leaf range is specified"));
                info->add(new streamer_double(ofs, "fMaximum", "Maximum value if leaf range is specified"));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafB");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fMinimum");
                scs(check, "Char_t");
                scs(check, "fMaximum");
                scs(check, "Char_t");
                //Should be : FIXME
                StreamerInfo* info = new StreamerInfo("TLeafB", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_basic_type("fMinimum", "Minimum value if leaf range is specified", ofs, streamer_info::CHAR, "Char_t"));
                ofs += size_CHAR;
                info->add(new streamer_basic_type("fMaximum", "Maximum value if leaf range is specified", ofs, streamer_info::CHAR, "Char_t"));
                ofs += size_CHAR;
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafC");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fMinimum");
                scs(check, "Int_t");
                scs(check, "fMaximum");
                scs(check, "Int_t");
                //Should be : FIXME
                StreamerInfo* info = new StreamerInfo("TLeafC", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_int(ofs, "fMinimum", "Minimum value if leaf range is specified"));
                info->add(new streamer_int(ofs, "fMaximum", "Maximum value if leaf range is specified"));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafObject");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fVirtual");
                scs(check, "Bool_t");
                //Should be 2312661809U
                StreamerInfo* info = new StreamerInfo("TLeafObject", 4, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                info->add(new streamer_bool(ofs, "fVirtual", "Support for Virtuality"));
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TLeafElement");
                //bases :
                scs(check, "TLeaf");
                //members :
                scs(check, "fID");
                scs(check, "Int_t");
                scs(check, "fType");
                scs(check, "Int_t");
                StreamerInfo* info = new StreamerInfo("TLeafElement", 1, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TLeaf", "Leaf: description of a Branch data type", ofs, TLeaf_version));
                ofs += size_TLeaf();
                ofs += size_POINTER; //!*fAbsAddress
                info->add(new streamer_int(ofs, "fID", "element serial number in fInfo"));
                info->add(new streamer_int(ofs, "fType", "leaf type"));
                //size_TLeafElement = ofs;
            }
        }

        inline int size_TAttAxis()
        {
            return 38;   //40?
        }
        inline int size_TAxis()
        {
            return 126;   //128?
        }
        inline int size_TH1()
        {
            return 560;   //568?
        }
        inline int size_TH2()
        {
            return 592;   //600?
        }
        inline int size_TH2D()
        {
            return 604;   //612?
        }

        inline void fill_infos_histo(List<StreamerInfo>& a_infos, std::ostream& a_out)
        {
            typedef streamer_basic_type sbt;
            const int size_POINTER = 4;
            const int size_DOUBLE = 8;
            const int size_SHORT = 2;
            const int size_INT = 4;
            //const int size_TObjArray = 40;
            const int size_TArrayD = 12;
            const int size_TArrayF = 12;
            short TNamed_version = 1;
            short TArrayF_version = 1;
            short TArrayD_version = 1;
            short TAttLine_version = 1;
            short TAttFill_version = 1;
            short TAttMarker_version = 1;
            short TAttAxis_version = 4;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TAttAxis");
                //members :
                scs(check, "fNdivisions");
                scs(check, "Int_t");
                scs(check, "fAxisColor");
                scs(check, "Color_t");
                scs(check, "fLabelColor");
                scs(check, "Color_t");
                scs(check, "fLabelFont");
                scs(check, "Style_t");
                scs(check, "fLabelOffset");
                scs(check, "Float_t");
                scs(check, "fLabelSize");
                scs(check, "Float_t");
                scs(check, "fTickLength");
                scs(check, "Float_t");
                scs(check, "fTitleOffset");
                scs(check, "Float_t");
                scs(check, "fTitleSize");
                scs(check, "Float_t");
                scs(check, "fTitleColor");
                scs(check, "Color_t");
                scs(check, "fTitleFont");
                scs(check, "Style_t");
                StreamerInfo* info = new StreamerInfo("TAttAxis", 4, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = size_VIRTUAL();
                info->add(new streamer_int(ofs, "fNdivisions", "Number of divisions(10000*n3 + 100*n2 + n1)"));
                info->add(new sbt("fAxisColor", "color of the line axis", ofs, streamer_info::SHORT, "Color_t"));
                ofs += size_SHORT;
                info->add(new sbt("fLabelColor", "color of labels", ofs, streamer_info::SHORT, "Color_t"));
                ofs += size_SHORT;
                info->add(new sbt("fLabelFont", "font for labels", ofs, streamer_info::SHORT, "Style_t"));
                ofs += size_SHORT;
                info->add(new streamer_float(ofs, "fLabelOffset", "offset of labels"));
                info->add(new streamer_float(ofs, "fLabelSize", "size of labels"));
                info->add(new streamer_float(ofs, "fTickLength", "length of tick marks"));
                info->add(new streamer_float(ofs, "fTitleOffset", "offset of axis title"));
                info->add(new streamer_float(ofs, "fTitleSize", "size of axis title"));
                info->add(new sbt("fTitleColor", "color of axis title", ofs, streamer_info::SHORT, "Color_t"));
                ofs += size_SHORT;
                info->add(new sbt("fTitleFont", "font for axis title", ofs, streamer_info::SHORT, "Style_t"));
                ofs += size_SHORT;

                //uuuu ofs += 2; //alignement ???
                if (ofs != size_TAttAxis()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TAttAxis " << ofs << " (" << size_TAttAxis() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TAxis");
                //bases :
                scs(check, "TNamed");
                scs(check, "TAttAxis");
                //members :
                scs(check, "fNbins");
                scs(check, "Int_t");
                scs(check, "fXmin");
                scs(check, "Axis_t");
                scs(check, "fXmax");
                scs(check, "Axis_t");
                scs(check, "fXbins");
                scs(check, "TArrayD");
                scs(check, "fFirst");
                scs(check, "Int_t");
                scs(check, "fLast");
                scs(check, "Int_t");
                scs(check, "fTimeDisplay");
                scs(check, "Bool_t");
                scs(check, "fTimeFormat");
                scs(check, "TString");
                StreamerInfo* info = new StreamerInfo("TAxis", 6, check);
                a_infos.push_back(info);
                // Elements :
                int ofs = 0;
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", ofs, TNamed_version));
                ofs += size_TNamed();
                info->add(new streamer_base("TAttAxis", "Axis attributes", ofs, TAttAxis_version));
                ofs += size_TAttAxis();
                info->add(new streamer_int(ofs, "fNbins", "Number of bins"));
                info->add(new sbt("fXmin", "low edge of first bin", ofs, streamer_info::DOUBLE, "Axis_t"));
                ofs += size_DOUBLE;
                info->add(new sbt("fXmax", "upper edge of last bin", ofs, streamer_info::DOUBLE, "Axis_t"));
                ofs += size_DOUBLE;
                info->add(new streamer_object_any("fXbins", "Bin edges array in X", ofs, "TArrayD"));
                ofs += size_TArrayD;
                ofs += size_POINTER; //!*fXlabels
                info->add(new streamer_int(ofs, "fFirst", "first bin to display"));
                info->add(new streamer_int(ofs, "fLast", "last bin to display"));
                info->add(new streamer_bool(ofs, "fTimeDisplay", "on/off displaying time values instead of numerics"));
                info->add(new streamer_string(ofs, "fTimeFormat", "Date&time format, ex: 09/12/99 12:34:00"));
                ofs += size_POINTER; //!*fParent

                //v3-05-07 : 124 (stored) + 4  = 128
                if (ofs != size_TAxis()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TAxis " << ofs << " (" << size_TAxis() << " expected.)"
                          << std::endl;
                }
            }
            short TH1_version = 3;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH1");
                //bases :
                scs(check, "TNamed");
                scs(check, "TAttLine");
                scs(check, "TAttFill");
                scs(check, "TAttMarker");
                //members :
                scs(check, "fNcells");
                scs(check, "Int_t");
                scs(check, "fXaxis");
                scs(check, "TAxis");
                scs(check, "fBarOffset");
                scs(check, "Short_t");
                scs(check, "fBarWidth");
                scs(check, "Short_t");
                scs(check, "fEntries");
                scs(check, "Stat_t");
                scs(check, "fTsumw");
                scs(check, "Stat_t");
                scs(check, "fTsumw2");
                scs(check, "Stat_t");
                scs(check, "fTsumwx");
                scs(check, "Stat_t");
                scs(check, "fTsumwx2");
                scs(check, "Stat_t");
                scs(check, "fMaximum");
                scs(check, "Double_t");
                scs(check, "fMinimum");
                scs(check, "Double_t");
                scs(check, "fNormFactor");
                scs(check, "Double_t");
                scs(check, "fContour");
                scs(check, "TArrayD");
                scs(check, "fSumw2");
                scs(check, "TArrayD");
                scs(check, "fOption");
                scs(check, "TString");
                scs(check, "fFunctions");
                scs(check, "TList*");
                StreamerInfo* info = new StreamerInfo("TH1", TH1_version, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TNamed", "The basis for a named object (name, title)", ofs, TNamed_version));
                ofs += size_TNamed();
                info->add(new streamer_base("TAttLine", "Line attributes", ofs, TAttLine_version));
                ofs += size_TAttLine();
                info->add(new streamer_base("TAttFill", "Fill area attributes", ofs, TAttFill_version));
                ofs += size_TAttFill();
                info->add(new streamer_base("TAttMarker", "Marker attributes", ofs, TAttMarker_version));
                ofs += size_TAttMarker();
                info->add(new streamer_int(ofs, "fNcells", "number of bins(1D), cells (2D) +U/Overflows"));
                info->add(new streamer_object("fXaxis", "X axis descriptor", ofs, "TAxis"));
                ofs += size_TAxis();
                info->add(new streamer_object("fYaxis", "Y axis descriptor", ofs, "TAxis"));
                ofs += size_TAxis();
                info->add(new streamer_object("fZaxis", "Z axis descriptor", ofs, "TAxis"));
                ofs += size_TAxis();
                info->add(new streamer_short(ofs, "fBarOffset", "(1000*offset) for bar charts or legos"));
                info->add(new streamer_short(ofs, "fBarWidth", "(1000*width) for bar charts or legos"));
                info->add(new streamer_stat_t(ofs, "fEntries", "Number of entries"));
                info->add(new streamer_stat_t(ofs, "fTsumw", "Total Sum of weights"));
                info->add(new streamer_stat_t(ofs, "fTsumw2", "Total Sum of squares of weights"));
                info->add(new streamer_stat_t(ofs, "fTsumwx", "Total Sum of weight*X"));
                info->add(new streamer_stat_t(ofs, "fTsumwx2", "Total Sum of weight*X*X"));
                info->add(new streamer_double(ofs, "fMaximum", "Maximum value for plotting"));
                info->add(new streamer_double(ofs, "fMinimum", "Minimum value for plotting"));
                info->add(new streamer_double(ofs, "fNormFactor", "Normalization factor"));
                info->add(new streamer_object_any("fContour", "Array to display contour levels", ofs, "TArrayD"));
                ofs += size_TArrayD;
                info->add(new streamer_object_any("fSumw2", "Array of sum of squares of weights", ofs, "TArrayD"));
                ofs += size_TArrayD;
                info->add(new streamer_string(ofs, "fOption", "histogram options"));
                info->add(new streamer_object_pointer("fFunctions", "->Pointer to list of functions (fits and user)", ofs, "TList*"));
                ofs += size_POINTER;
                ofs += size_POINTER; //!*fDirectory
                ofs += size_INT;     //!fDimension
                ofs += size_POINTER; //!*fIntegral
                ofs += size_POINTER; //!*fPainter

                //v3-05-07 : 576  = 568 + 2 * 4 = ok
                if (ofs != size_TH1()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TH1 " << ofs << " (" << size_TH1() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH1F");
                //base :
                scs(check, "TH1");
                scs(check, "TArrayF");
                StreamerInfo* info = new StreamerInfo("TH1F", 1, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH1", "1-Dim histogram base class", ofs, TH1_version));
                ofs += size_TH1();
                info->add(new streamer_base("TArrayF", "Array of floats", ofs, TArrayF_version));
                ofs += size_TArrayF;
            }
            int size_TH1D = 0;
            short TH1D_version = 1;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH1D");
                //base :
                scs(check, "TH1");
                scs(check, "TArrayD");
                StreamerInfo* info = new StreamerInfo("TH1D", TH1D_version, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH1", "1-Dim histogram base class", ofs, TH1_version));
                ofs += size_TH1();
                info->add(new streamer_base("TArrayD", "Array of doubles", ofs, TArrayD_version));
                ofs += size_TArrayD;
                size_TH1D = ofs; //580
                //v3-05-07 : 588 = 576 + 12 = ok
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TProfile");
                //base :
                scs(check, "TH1D");
                //members :
                scs(check, "fBinEntries");
                scs(check, "TArrayD");
                scs(check, "fErrorMode");
                scs(check, "EErrorType"); //FIXME : ok ?
                scs(check, "fYmin");
                scs(check, "Double_t");
                scs(check, "fYmax");
                scs(check, "Double_t");
                scs(check, "fTsumwy");
                scs(check, "Double_t");
                scs(check, "fTsumwy2");
                scs(check, "Double_t");
                StreamerInfo* info = new StreamerInfo("TProfile", 4, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH1D", "1-Dim histograms (one double per channel)", ofs, TH1D_version));
                ofs += size_TH1D;
                info->add(new streamer_object_any("fBinEntries", "number of entries per bin", ofs, "TArrayD"));
                ofs += size_TArrayD;
                info->add(new streamer_basic_type("fErrorMode", "Option to compute errors", ofs, streamer_info::INT, "EErrorType"));
                ofs += size_INT;
                info->add(new streamer_double(ofs, "fYmin", "Lower limit in Y (if set)"));
                info->add(new streamer_double(ofs, "fYmax", "Upper limit in Y (if set)"));
                info->add(new streamer_double(ofs, "fTsumwy", "Total Sum of weight*Y"));
                info->add(new streamer_double(ofs, "fTsumwy2", "Total Sum of weight*Y*Y"));
                //628
                //v3-05-07 : 624 = 612 + 4 + 8 (diff TH1D) = ok
            }
            short TH2_version = 3;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH2");
                //bases :
                scs(check, "TH1");
                //members :
                scs(check, "fScalefactor");
                scs(check, "Stat_t");
                scs(check, "fTsumwy");
                scs(check, "Stat_t");
                scs(check, "fTsumwy2");
                scs(check, "Stat_t");
                scs(check, "fTsumwxy");
                scs(check, "Stat_t");
                StreamerInfo* info = new StreamerInfo("TH2", TH2_version, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH1", "1-Dim histogram base class", ofs, TH1_version));
                ofs += size_TH1();
                info->add(new streamer_stat_t(ofs, "fScalefactor", "Scale factor"));
                info->add(new streamer_stat_t(ofs, "fTsumwy", "Total Sum of weight*Y"));
                info->add(new streamer_stat_t(ofs, "fTsumwy2", "Total Sum of weight*Y*Y"));
                info->add(new streamer_stat_t(ofs, "fTsumwxy", "Total Sum of weight*X*Y"));

                //v3-05-07 : <to be checked>
                if (ofs != size_TH2()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TH2 " << ofs << " (" << size_TH2() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH2F");
                //base :
                scs(check, "TH2");
                scs(check, "TArrayF");
                StreamerInfo* info = new StreamerInfo("TH2F", 3, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH2", "2-Dim histogram base class", ofs, TH2_version));
                ofs += size_TH2();
                info->add(new streamer_base("TArrayF", "Array of floats", ofs, TArrayF_version));
                ofs += size_TArrayF;
                //612
            }
            short TH2D_version = 3;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH2D");
                //base :
                scs(check, "TH2");
                scs(check, "TArrayD");
                StreamerInfo* info = new StreamerInfo("TH2D", TH2D_version, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH2", "2-Dim histogram base class", ofs, TH2_version));
                ofs += size_TH2();
                info->add(new streamer_base("TArrayD", "Array of doubles", ofs, TArrayD_version));
                ofs += size_TArrayD;

                if (ofs != size_TH2D()) {
                    a_out << "inlib::root::fill_infos :"
                          << " TH2D " << ofs << " (" << size_TH2D() << " expected.)"
                          << std::endl;
                }
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TProfile2D");
                //base :
                scs(check, "TH2D");
                //members :
                scs(check, "fBinEntries");
                scs(check, "TArrayD");
                scs(check, "fErrorMode");
                scs(check, "EErrorType"); //FIXME : ok ?
                scs(check, "fZmin");
                scs(check, "Double_t");
                scs(check, "fZmax");
                scs(check, "Double_t");
                scs(check, "fTsumwz");
                scs(check, "Double_t");
                scs(check, "fTsumwz2");
                scs(check, "Double_t");
                StreamerInfo* info = new StreamerInfo("TProfile2D", 5, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH2D", "2-Dim histograms (one double per channel)", ofs, TH2D_version));
                ofs += size_TH2D();
                info->add(new streamer_object_any("fBinEntries", "number of entries per bin", ofs, "TArrayD"));
                ofs += size_TArrayD;
                info->add(new sbt("fErrorMode", "Option to compute errors", ofs, streamer_info::INT, "EErrorType"));
                ofs += size_INT;
                info->add(new streamer_double(ofs, "fZmin", "Lower limit in Z (if set)"));
                info->add(new streamer_double(ofs, "fZmax", "Upper limit in Z (if set)"));
                info->add(new streamer_double(ofs, "fTsumwz", "Total Sum of weight*Z"));
                info->add(new streamer_double(ofs, "fTsumwz2", "Total Sum of weight*Z*Z"));
                //660
            }
            int size_TH3 = 0;
            short TH3_version = 4;
            short TAtt3D_version = 1;
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH3");
                //bases :
                scs(check, "TH1");
                scs(check, "TAtt3D");
                //members :
                scs(check, "fTsumwy");
                scs(check, "Double_t");
                scs(check, "fTsumwy2");
                scs(check, "Double_t");
                scs(check, "fTsumwxy");
                scs(check, "Double_t");
                scs(check, "fTsumwz");
                scs(check, "Double_t");
                scs(check, "fTsumwz2");
                scs(check, "Double_t");
                scs(check, "fTsumwxz");
                scs(check, "Double_t");
                scs(check, "fTsumwyz");
                scs(check, "Double_t");
                StreamerInfo* info = new StreamerInfo("TH3", TH3_version, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH1", "1-Dim histogram base class", ofs, TH1_version));
                ofs += size_TH1();
                info->add(new streamer_base("TAtt3D", "3D attributes", ofs, TAtt3D_version));
                ofs += 0;
                info->add(new streamer_double(ofs, "fTsumwy", "Total Sum of weight*Y"));
                info->add(new streamer_double(ofs, "fTsumwy2", "Total Sum of weight*Y*Y"));
                info->add(new streamer_double(ofs, "fTsumwxy", "Total Sum of weight*X*Y"));
                info->add(new streamer_double(ofs, "fTsumwz", "Total Sum of weight*Z"));
                info->add(new streamer_double(ofs, "fTsumwz2", "Total Sum of weight*Z*Z"));
                info->add(new streamer_double(ofs, "fTsumwxz", "Total Sum of weight*X*Z"));
                info->add(new streamer_double(ofs, "fTsumwyz", "Total Sum of weight*Y*Z"));
                size_TH3 = ofs; //624
                //v4-00-02 : <to be checked>
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH3F");
                //base :
                scs(check, "TH3");
                scs(check, "TArrayF");
                StreamerInfo* info = new StreamerInfo("TH3F", 3, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH3", "3-Dim histogram base class", ofs, TH3_version));
                ofs += size_TH3;
                info->add(new streamer_base("TArrayF", "Array of floats", ofs, TArrayF_version));
                ofs += size_TArrayF;
                //636
            }
            {
                unsigned int check = 0;
                //this :
                scs(check, "TH3D");
                //base :
                scs(check, "TH3");
                scs(check, "TArrayD");
                StreamerInfo* info = new StreamerInfo("TH3D", 3, check);
                a_infos.push_back(info);
                int ofs = 0;
                info->add(new streamer_base("TH3", "3-Dim histogram base class", ofs, TH3_version));
                ofs += size_TH3;
                info->add(new streamer_base("TArrayD", "Array of doubles", ofs, TArrayD_version));
                ofs += size_TArrayD;
                //636
            }
        }

        inline void fill_infos(List<StreamerInfo>& a_infos, std::ostream& a_out)
        {
            fill_infos_core(a_infos, a_out);
            fill_infos_cont(a_infos, a_out);
            fill_infos_graf(a_infos, a_out);
            fill_infos_tree(a_infos, a_out);
            fill_infos_histo(a_infos, a_out);
        }

    }
}

#endif

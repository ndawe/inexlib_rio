// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_rroot_clss
#define inlib_rroot_clss

#include <string>

namespace inlib {
    namespace rroot {

        inline const std::string& TH1F_cls()
        {
            static const std::string s_v("TH1F");
            return s_v;
        }

        inline const std::string& TH1D_cls()
        {
            static const std::string s_v("TH1D");
            return s_v;
        }

        inline const std::string& TH2F_cls()
        {
            static const std::string s_v("TH2F");
            return s_v;
        }

        inline const std::string& TH2D_cls()
        {
            static const std::string s_v("TH2D");
            return s_v;
        }

        inline const std::string& TH3D_cls()
        {
            static const std::string s_v("TH3D");
            return s_v;
        }

        inline const std::string& TProfile_cls()
        {
            static const std::string s_v("TProfile");
            return s_v;
        }

        inline const std::string& TProfile2D_cls()
        {
            static const std::string s_v("TProfile2D");
            return s_v;
        }

        inline const std::string& TDirectory_cls()
        {
            static const std::string s_v("TDirectory");
            return s_v;
        }

        inline const std::string& TGeoManager_cls()
        {
            static const std::string s_v("TGeoManager");
            return s_v;
        }

        inline const std::string& TList_cls()
        {
            static const std::string s_v("TList");
            return s_v;
        }

    }
}

#endif
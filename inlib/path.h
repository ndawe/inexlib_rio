// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_path
#define inlib_path

#include <string>

namespace inlib {

    inline std::string suffix(const std::string& a_string, bool a_back = true)
    {
        // If a_string = dir0/dir1/dir2/dir3/name.xxx
        //   return xxx
        std::string::size_type pos = a_back ? a_string.rfind('.') : a_string.find('.');

        if (pos == std::string::npos) return "";

        pos++;
        return a_string.substr(pos, a_string.size() - pos);
    }

    inline std::string nosuffix(const std::string& a_string, bool a_back = true)
    {
        // If a_string = dir0/dir1/dir2/dir3/name.xxx
        //   return name
        // Start searching after the last / (or last \ for Windows).
        std::string::size_type pos = a_string.rfind('/');

        if (pos == std::string::npos) pos = a_string.rfind('\\');

        if (pos == std::string::npos) pos = 0;
        else pos++;

        std::string s = a_string.substr(pos, a_string.size() - pos);
        std::string::size_type dot_pos = a_back ? s.rfind('.') : s.find('.');

        if (dot_pos == std::string::npos) return s;

        return s.substr(0, dot_pos);
    }

    inline void base_name(const std::string& a_path, std::string& a_value)
    {
        std::string::size_type pos_slash = a_path.rfind('/');
        std::string::size_type pos_bslash = a_path.rfind('\\');
        std::string::size_type pos = 0;

        if (pos_slash == std::string::npos) {
            if (pos_bslash == std::string::npos) {
                pos = std::string::npos;
            } else {
                pos = pos_bslash;
            }
        } else {
            if (pos_bslash == std::string::npos) {
                pos = pos_slash;
            } else {
                if (pos_slash <= pos_bslash) {
                    pos = pos_bslash;
                } else {
                    pos = pos_slash;
                }
            }
        }

        if (pos == std::string::npos) {
            a_value = a_path;
            return;
        }

        pos++;
        a_value = a_path.substr(pos, a_path.size() - pos);
    }

    inline std::string base_name(const std::string& a_path)   //deprecated.
    {
        std::string value;
        base_name(a_path, value);
        return value;
    }

    inline bool is_absolute_path(const std::string& a_path)
    {
        if (a_path.find('\\') != std::string::npos) { //Windows path.
            if (a_path.find(':') != std::string::npos) return true;

            return (a_path.size() && (a_path[0] == '\\') ? true : false);
        } else { //UNIX path
            return (a_path.size() && (a_path[0] == '/') ? true : false);
        }
    }

    inline bool path_name_suffix(const std::string& a_string, std::string& a_path, std::string& a_name, std::string& a_suffix)
    {
        // If a_string = dir0/dir1/dir2/dir3/name.xxx
        //   a_path = dir0/dir1/dir2/dir3
        //   a_name = name.xxx
        //   a_suffix = xxx
        // If a_string = dir0/name.xxx
        //   a_path = dir0
        //   a_name = name.xxx
        //   a_suffix = xxx
        // If a_string = name.xxx
        //   a_path.clear()
        //   a_name = name.xxx
        //   a_suffix = xxx
        // If a_string = /name.xxx
        //   a_path = "/"
        //   a_name = name.xxx
        //   a_suffix = xxx
        // If a_string = .
        //   a_path = "."
        //   a_name.clear()
        //   a_suffix.clear()
        // If a_string = ..
        //   a_path = ".."
        //   a_name.clear()
        //   a_suffix.clear()
        if (a_string == ".") {
            a_path = ".";
            a_name.clear();
            a_suffix.clear();
            return true;
        } else if (a_string == "..") {
            a_path = "..";
            a_name.clear();
            a_suffix.clear();
            return true;
        }

        std::string::size_type pos_slash = a_string.rfind('/');
        std::string::size_type pos_bslash = a_string.rfind('\\');
        std::string::size_type pos = 0;

        if (pos_slash == std::string::npos) {
            if (pos_bslash == std::string::npos) {
                pos = std::string::npos;
            } else {
                pos = pos_bslash;
            }
        } else {
            if (pos_bslash == std::string::npos) {
                pos = pos_slash;
            } else {
                if (pos_slash <= pos_bslash) {
                    pos = pos_bslash;
                } else {
                    pos = pos_slash;
                }
            }
        }

        if (pos == std::string::npos) {
            a_path.clear();
            pos = 0;
        } else if (pos == 0) {
            a_path = "/";
            pos++;
        } else {
            a_path = a_string.substr(0, pos);
            pos++;
        }

        std::string s = a_string.substr(pos, a_string.size() - pos);
        pos = s.rfind('.');

        if (pos == std::string::npos) {
            a_name = s;
            a_suffix.clear();
        } else {
            a_name = s;
            pos++;
            a_suffix = s.substr(pos, s.size() - pos);
        }

        return true;
    }

    inline std::string dir_name(const std::string& a_path, unsigned int a_num = 1)
    {
        std::string path = a_path;

        for (unsigned int index = 0; index < a_num; index++) {
            std::string p, n, s;
            path_name_suffix(path, p, n, s);
            path = p;
        }

        return path;
    }

    //used in OpenPAW, BatchLab.
    inline bool is_f77(const std::string& a_path)
    {
        std::string sfx = suffix(a_path);

        //tolowercase(sfx);
        for (std::string::iterator it = sfx.begin(); it != sfx.end(); ++it) {
            char c = *it;
            *it = ((c) >= 'A' && (c) <= 'Z' ?  c - 'A' + 'a' : c);
        }

        if (sfx == "f") return true;    //the standard.

        if (sfx == "for") return true;  //for opaw. Known by g77.

        if (sfx == "ftn") return true;  //for opaw.

        if (sfx == "fortran") return true; //for opaw.

        if (sfx == "f77") return true;

        return false;
    }

    //used in OpenPAW, BatchLab.
    inline bool is_cpp(const std::string& a_path)
    {
        std::string sfx = suffix(a_path);

        //tolowercase(sfx);
        for (std::string::iterator it = sfx.begin(); it != sfx.end(); ++it) {
            char c = *it;
            *it = ((c) >= 'A' && (c) <= 'Z' ?  c - 'A' + 'a' : c);
        }

        if (sfx == "c") return true;

        if (sfx == "cxx") return true;

        if (sfx == "cpp") return true;

        if (sfx == "C") return true;

        return false;
    }

}

#endif
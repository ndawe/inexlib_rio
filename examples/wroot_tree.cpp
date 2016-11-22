// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

//inlib_build_use inlib inlib zlib kernel

#ifdef INLIB_MEM
    #include <inlib/mem.h>
#endif

#include <inlib/root/file.h>
#include <inlib/root/tree.h>
#include <inlib/randd.h>
#include <inlib/randf.h>
#include <inlib/zlib.h>

#include <iostream>
#include <cstdlib>

int main(int, char**)
{
    #ifdef INLIB_MEM
    inlib::mem::set_check_by_class(true);
    {
    #endif //INLIB_MEM
        //////////////////////////////////////////////////////////
        /// create a .root file : ////////////////////////////////
        //////////////////////////////////////////////////////////
        std::string file = "root_tree.root";
        inlib::root::file rfile(std::cout, file);
        rfile.add_ziper('Z', inlib::compress_buffer);
        rfile.set_compression(9);
        //////////////////////////////////////////////////////////
        /// create and fill a tree : /////////////////////////////
        //////////////////////////////////////////////////////////
        {
            //WARNING : the tree can't be on the stack. It is owned
            //          by the directory.
            inlib::root::tree* tr =
                new inlib::root::tree(rfile.dir(), "tree", "first tree");
            inlib::root::branch* br = tr->create_branch("branch");
            inlib::root::leaf<int>* leaf_index =
                br->create_leaf<int>("index", "index");
            inlib::root::leaf<double>* leaf_rgauss =
                br->create_leaf<double>("rgauss", "Random gaussian");
            inlib::root::leaf<float>* leaf_rbw =
                br->create_leaf<float>("rbw", "Random BW");
            inlib::rgaussd rg(1, 2);
            inlib::rbwf rbw(0, 1);
            // fill :
            unsigned int entries = 1000000;

            //unsigned int entries = 400000000; //to test >2Gbytes file.
            //br->set_basket_size(1000000);
            for (unsigned int count = 0; count < entries; count++) {
                //inlib::count_out<unsigned int>(std::cout,count,1000000);
                leaf_index->fill(count);
                leaf_rgauss->fill(rg.shoot());
                leaf_rbw->fill(rbw.shoot());
                inlib::uint32 n;

                if (!tr->fill(n)) {
                    std::cout << "tree fill failed." << std::endl;
                    break;
                }
            }
        }
        //////////////////////////////////////////////////////////
        /// write and close file : ///////////////////////////////
        //////////////////////////////////////////////////////////
        {
            unsigned int n;

            if (!rfile.write(n)) {
                std::cout << "file write failed." << std::endl;
            }
        }
        rfile.close();
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        #ifdef INLIB_MEM
    }
    inlib::mem::balance(std::cout);
        #endif //INLIB_MEM
    return EXIT_SUCCESS;
}

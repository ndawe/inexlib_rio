
To build::

   cd ./examples
   ./build

To run::

   ./bin/wroot

it produces a wroot.root file with histos and ntuples in it. To see the "keys" in it::

   ./bin/root_ls wroot.root

To read it with inexlib::

   ./bin/rroot wroot.root

With CERN-ROOT around, you can do::

  <source setup CERN-ROOT>
  root read_wroot_root.C

Also::

  ./bin/wroot_tree
  ./bin/root_ls wroot_tree.root

With CERN-ROOT around, you can do on the wroot_tree.root::

  <source setup CERN-ROOT>
  root read_wroot_tree_root.C

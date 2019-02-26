//=============================================================================
//
//   Copyright (c) by Computer Graphics Group, Bielefeld University
//
// This work is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
//
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.
//
//=============================================================================

#include "MLMViewer.h"
#include <iostream>

//=============================================================================

int main(int argc, char **argv)
{
    MLMViewer window("MLMViewer", 1024, 768);

    bool ok_load = false;
    if (argc == 2)
        ok_load = window.load_mlm(argv[1]);
    else
        ok_load = window.load_mlm("../data/");

    if (!ok_load)
    {
        std::cerr << "[ERROR] Can't load multilinear model!" << std::endl
                  << "Usage: './mlmviewer <directory name where the multilinear model is stored>'" << std::endl;
        return EXIT_FAILURE;
    }

    return window.run();
}

//=============================================================================

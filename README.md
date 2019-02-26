Multilinear model demo
======================

This application demonstrates how to load and evaluate a multilinear model for different skull shape and FSTT distribution parameters and thereby generating meshes for skull and skin surface.

We provide different multilinear models, that where all trained on the same training data:
  - a full model with 64 parameters for skull shape and 32 parameters for FSTT distribution
  - a restricted model with 16 parameters for skull shape and 16 parameters for FSTT distribution
  - a restricted model with 7 parameters for skull shape and 4 parameters for FSTT distribution
The models can be found in mlm_models.zip, please see the README file therein.


Building under Linux/MacOS
--------------------------

Inside the top-level directory, execute the following commands:

    mkdir build
    cd build
    cmake ..
    make


Using Xcode under MacOS
-----------------------

If you prefer to use Xcode on Mac, do the following:

    mkdir build
    cd build
    cmake -G Xcode ..

and then open the generated Xcode project.


Building and running under Windows
----------------------------------

* Download and install [Visual Studio Community](https://www.visualstudio.com/vs/community/)
* Make sure that you check "Desktop development with C++" during installation
* Download [cmake](https://cmake.org/download/) via the platform windows .zip version and extract it somewhere
* Create an empty build folder inside the project's top-level directory
* Start cmake-gui.exe (located in cmake's bin folder)
* Specify the top-level directory as source directory (button Browse source...)
* Specify the build folder as build directory (button Browse build...)
* Select Configure using your Visual Studio Version as option.
* When configuration is finished, select Generate.
* Start Visual Studio Community
* Open the project via File -> open -> project -> .sln in build folder
* In the project explorer window on the right, right-click the project and set it as startup-project
* Switch to release mode
* The executable will need the path to the multilinear model as command line arguments. You can specify the argument via project -> properties -> debugging -> command arguments
* Hit CTRL + F5 to build and run (or CTRL + SHIFT + B to build)


Documentation
-------------

Open the pre-built documentation from within the `build` directory via

    firefox ../doc/index.html

or using any other web browser.

You can also build the HTML documentation on your own as long as you have [Doxygen](www.doxygen.org/) installed. To do so, still inside the directory `build`, execute the following command:

    make doc

View the documentation by opening the file `html/index.html` with any web browser / HTML viewer. If you are into LaTeX, navigate into the directory `latex` and execute the command `make` to create a printable version of the documentation.


Running
-------

After building (i.e. execution of `make`), still in the directory `build`, launch the program by executing the following command:

    ./mlmviewer <directory name where the multilinear model is stored>

or

    ./mlmviewer

which by default loads the restricted model with 7 parameters for skull shape and 4 parameters for FSTT distribution.


Controls
--------

* `Esc`: quit application.
* GUI:
  - Show/hide skin and skull meshes,
  - vary transparency for skin mesh,
  - vary skull and FSTT parameters,
  - save meshes to disk into build folder,
    - note that the saved meshes are named 'mesh_skin_N.off' and 'mesh_skull_N.off', while N is an enumerating counter
  - reset parameters to default,
  - use mouse controls to rotate and translate the meshes.


License
-------

Copyright (c) by Computer Graphics Group, Bielefeld University

This work is licensed under a
Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

You should have received a copy of the license along with this
work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

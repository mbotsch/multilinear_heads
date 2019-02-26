# Multilinear model demo

This project demonstrates how to load and evaluate a multilinear model for different skull shapes and facial soft tissue thicknesses (FSTT) and thereby generating meshes for the skull and skin surfaces.

In this repository we provide the full source code for evaluating the multilinear model as well as the data for a  **reduced** multilinear model with 7 parameters for skull shape and 4 parameters for FSTT distribution. Your can download the full model [here](https://pub.uni-bielefeld.de/record/2930619).


## References

This project is based on these two papers:
- Jascha Achenbach, Robert Brylka, Thomas Gietzen, Katja zum Hebel, Elmar Schömer, Ralf Schulze, Mario Botsch, Ulrich Schwanecke:
  [A Multilinear Model for Bidirectional Craniofacial Reconstruction](http://graphics.uni-bielefeld.de/publications/vcbm18.pdf),
  Proceedings of Eurographics Workshop on Visual Computing for Biology and Medicine, 2018, pp. 67-76.
- Thomas Gietzen, Robert Brylka, Jascha Achenbach, Katja zum Hebel, Elmar Schömer, Mario Botsch, Ulrich Schwanecke, Ralf Schulze:
  [A method for automatic forensic facial reconstruction based on dense statistics of soft tissue thickness](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0210257),
  PLoS ONE, 14(1), 2019. 

Please cite the first paper if you use the code in your research project.


## Quickstart

The project is based on the [PMP library](http://www.pmp-library.org/) for mesh processing, visualization, and GUI. It is automatically downloaded as a git submodule.

Fetch the repository:

    $ git clone --recursive https://github.com/mbotsch/multilinear_heads.git

Configure and build:

    $ cd multilinear_heads && mkdir build && cd build && cmake .. && make

Run the mesh processing app:

    $ ./mlmviewer


## Code Documentation

We provide a pre-built documentation in the `doc` directory:

    firefox multilinear_heads/doc/index.html

or using any other web browser.

You can also build the HTML documentation on your own as long as you have [Doxygen](www.doxygen.org/) installed:

    cd multilinear_heads/build/ && make doc

View the documentation by opening the file `build/html/index.html` with any web browser.


## Running the Application

After building (i.e. execution of `make`), still in the directory `build`, launch the program by executing the following command:

    ./mlmviewer <directory name where the multilinear model is stored>

or

    ./mlmviewer

which by default loads the restricted model with 7 parameters for skull shape and 4 parameters for FSTT distribution.


## License

Copyright (c) by Computer Graphics Group, Bielefeld University

This work is licensed under a
Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

You should have received a copy of the license along with this
work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

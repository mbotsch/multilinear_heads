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

#include "MultilinearModel.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace pmp;

//== IMPLEMENTATION ============================================================

MultilinearModel::
MultilinearModel()
{
}

//-----------------------------------------------------------------------------

bool
MultilinearModel::
load_means(const std::string& filenameMeanSkin, const std::string& filenameMeanSkull)
{
    // load mean skin
    pmp::SurfaceMesh meshMeanSkin;
    meshMeanSkin.read(filenameMeanSkin.c_str());
    if (meshMeanSkin.n_vertices() == 0)
    {
        std::cerr << "[ERROR] in 'MultilinearModel::loadMeans(...)' - Can't load means, since the skin mesh is empty" << std::endl;
        return false;
    }
    auto meshMeanSkinPoints = meshMeanSkin.vertex_property<pmp::Point>("v:point");


    // load mean skull
    pmp::SurfaceMesh meshMeanSkull;
    meshMeanSkull.read(filenameMeanSkull.c_str());
    if (meshMeanSkull.n_vertices() == 0)
    {
        std::cerr << "[ERROR] in 'MultilinearModel::loadMeans(...)' - Can't load means, since the skull mesh is empty" << std::endl;
        return false;
    }
    auto meshMeanSkullPoints = meshMeanSkull.vertex_property<pmp::Point>("v:point");


    // build mean vector from mean skin and mean skull
    const unsigned int dim0 = 3*meshMeanSkin.n_vertices() + 3*meshMeanSkull.n_vertices();
    mean_ = std::vector<double>(dim0, 0.0);

    unsigned int c = 0;
    for (auto v : meshMeanSkin.vertices())
    {
        const pmp::Point currentPoint = meshMeanSkinPoints[v];
        mean_[3*c + 0] = currentPoint[0];
        mean_[3*c + 1] = currentPoint[1];
        mean_[3*c + 2] = currentPoint[2];
        ++c;
    }

    for (auto v : meshMeanSkull.vertices())
    {
        const pmp::Point currentPoint = meshMeanSkullPoints[v];
        mean_[3*c + 0] = currentPoint[0];
        mean_[3*c + 1] = currentPoint[1];
        mean_[3*c + 2] = currentPoint[2];
        ++c;
    }

    return true;
}

//-----------------------------------------------------------------------------

bool
MultilinearModel::
load(const std::string& dirname)
{
    // load the multilinear model tensor
    std::string filename = dirname + "mlm_tensor.tensor";
    std::ifstream ifs(filename, std::ofstream::binary);
    if (!ifs)
    {
        std::cerr << "Cannot load tensor\n";
        return false;
    }
    ifs.read(reinterpret_cast<char *>(&dim0_), sizeof(dim0_));
    ifs.read(reinterpret_cast<char *>(&dim1_), sizeof(dim1_));
    ifs.read(reinterpret_cast<char *>(&dim2_), sizeof(dim2_));
    assert(dim0_ && dim1_ && dim2_);
    tensor_.resize(dim0_*dim1_*dim2_);
    ifs.read(reinterpret_cast<char *>(&tensor_[0]), dim0_*dim1_*dim2_*sizeof(double));
    ifs.close();


    // load matrix U_skull_ from file
    if (!load_matrix(U_skull_, dirname + "matrix_U_skull.matrix"))
        return false;
    assert(dim1_ == U_skull_.cols());


    // load matrix U_fstt_ from file
    if (!load_matrix(U_fstt_, dirname + "matrix_U_fstt.matrix"))
        return false;
    assert(dim2_ == U_fstt_.cols());


    // load eigenvalues_skull from file
    if (!load_vector(eigenvalues_skull_, dirname + "eigenvalues_skull.vector"))
    {
        std::cerr << "Cannot load skull eigenvalues\n";
        return false;
    }
    assert(dim1_ == eigenvalues_skull_.size());


    // load eigenvalues_fstt from file
    if (!load_vector(eigenvalues_fstt_, dirname + "eigenvalues_fstt.vector"))
    {
        std::cerr << "Cannot load FSTT eigenvalues\n";
        return false;
    }
    assert(dim2_ == eigenvalues_fstt_.size());


    return true;
}

//-----------------------------------------------------------------------------

bool
MultilinearModel::
evaluate(SurfaceMesh& skin, 
         SurfaceMesh& skull,
         const Eigen::VectorXd& w_skull, 
         const Eigen::VectorXd& w_fstt) const
{
    // check dimensions
    assert(mean_.size());
    assert(dim0_ && dim1_ && dim2_);
    assert(w_skull.size() == dim1_);
    assert(w_fstt.size()  == dim2_);
    assert(3*skin.n_vertices() + 3*skull.n_vertices() == dim0_);


    unsigned int i,j,k;


    // apply w_skull onto multilinear model, i.e., eliminate mode-1 for 'skull'
    Eigen::MatrixXd tempMatrix(dim0_, dim2_);
#pragma omp parallel for
    for (i=0; i<dim0_; ++i)
    {
        for (k=0; k<dim2_; ++k)
        {
            double c(0.0);
            for (j=0; j<dim1_; ++j)
                c += tensor(i,j,k) * w_skull(j);
            tempMatrix(i,k) = c;
        }
    }


    // apply w_fstt onto previously contracted tensor, i.e., eliminate mode-2 for 'fstt'
    Eigen::VectorXd tempVector(dim0_);
#pragma omp parallel for
    for (i=0; i<dim0_; ++i)
    {
        double c(0.0);
        for (k=0; k<dim2_; ++k)
            c += tempMatrix(i,k) * w_fstt(k);
        tempVector(i) = c;
    }


    // update skin mesh
    auto skin_points = skin.vertex_property<Point>("v:point");
    unsigned int c = 0;
    for (auto v : skin.vertices())
    {
        skin_points[v][0] = mean_[3*c + 0] + tempVector(3*c + 0);
        skin_points[v][1] = mean_[3*c + 1] + tempVector(3*c + 1);
        skin_points[v][2] = mean_[3*c + 2] + tempVector(3*c + 2);
        ++c;
    }


    // update skull mesh
    auto skull_points = skull.vertex_property<pmp::Point>("v:point");
    for (auto v : skull.vertices())
    {
        skull_points[v][0] = mean_[3*c + 0] + tempVector(3*c + 0);
        skull_points[v][1] = mean_[3*c + 1] + tempVector(3*c + 1);
        skull_points[v][2] = mean_[3*c + 2] + tempVector(3*c + 2);
        ++c;
    }


    return true;
}

//=============================================================================

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
#pragma once
//=============================================================================

//== INCLUDES =================================================================

#include <vector>
#include <string>
#include <Eigen/Dense>
#include <pmp/SurfaceMesh.h>


//== CLASS DEFINITION =========================================================

//! Multilinear model with three modes:
//! mesh vertices, skull shape, FSTT distribution
class MultilinearModel
{
public:

    //! constructor
    MultilinearModel();

    //! load mean skin and skull geometry for multilinear model
    bool load_means(const std::string& filenameMeanSkin, 
                    const std::string& filenameMeanSkull);

    //! load multilinear model: multilinear model tensor, matrix U_skull,
    //! matrix U_fstt, eigenvalues_skull, and eigenvalues_fstt
    bool load(const std::string& dirname);

    //! evaluate multilinear model, i.e., for given parameters 'wSkull'
    //! and 'wFstt', compute new skin/skull meshes
    bool evaluate(pmp::SurfaceMesh& meshSkin, pmp::SurfaceMesh& meshSkull,
                  const Eigen::VectorXd& wSkull, const Eigen::VectorXd& wFstt) const;

public:

    //! get dimension 0
    unsigned int dim0() const { return dim0_; }
    //! get dimension 1
    unsigned int dim1() const { return dim1_; }
    //! get dimension 2
    unsigned int dim2() const { return dim2_; }


    //! get matrix U_skull
    const Eigen::MatrixXd& U_skull() const
    {
        return U_skull_;
    }

    //! get matrix U_fstt
    const Eigen::MatrixXd& U_fstt() const
    {
        return U_fstt_;
    }

    //! get eigenvalues w.r.t. skull
    const Eigen::VectorXd& eigenvalues_skull() const
    {
        return eigenvalues_skull_;
    }

    //! get eigenvalues w.r.t. FSTT
    const Eigen::VectorXd& eigenvalues_fstt() const
    {
        return eigenvalues_fstt_;
    }


private: 


    //! write access to tensor data
    double& tensor(unsigned int i0, unsigned int i1, unsigned int i2) {
        return tensor_[i2 + i1*dim2_ + i0*dim1_*dim2_];
    }

    //! read access to tensor data
    double tensor(unsigned int i0, unsigned int i1, unsigned int i2) const {
        return tensor_[i2 + i1*dim2_ + i0*dim1_*dim2_];
    }


private:

    //! multilinear model tensor
    //! 0-mode: vertices for skull/skin
    //! 1-mode: different skulls
    //! 2-mode: different FSTTs each
    std::vector<double> tensor_;

    //! tensor dimensions
    unsigned int dim0_, dim1_, dim2_;

    //! matrix U_skull
    Eigen::MatrixXd U_skull_;
    //! matrix U_fstt
    Eigen::MatrixXd U_fstt_;

    //! eigenvalues w.r.t. skull
    Eigen::VectorXd eigenvalues_skull_;
    //! eigenvalues w.r.t. FSTT
    Eigen::VectorXd eigenvalues_fstt_;

    //! mean skin surface and skull
    std::vector< double > mean_;
};

//=============================================================================

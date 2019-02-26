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

#include <pmp/visualization/SurfaceMeshGL.h>
#include <pmp/visualization/TrackballViewer.h>

#include "MultilinearModel.h"

//=============================================================================

using namespace pmp;

//=============================================================================

//! Simple viewer for a multilinear model which maps from parameters for skull
//! shape and FSTT distribution to meshes for skull and skin surface
class MLMViewer : public TrackballViewer
{
public:

    //! constructor
    MLMViewer(const char* title, int width, int height, bool showgui = true);

    //! destructor
    virtual ~MLMViewer();

    //! load multilinear model from directory \c dirname
    bool load_mlm(const char* dirname);

    //! evaluate multilinear model
    void evaluate_mlm();

    //! update mesh normals and all buffers for OpenGL rendering. call this
    //! function whenever you change either the vertex positions or the
    //! triangulation of the meshes for skull or skin surface
    void update_meshes();

    //! update all buffers for OpenGL rendering. call this function whenever
    //! you change the vertex positions of the point set
    void update_points();

    //! draw the scene
    virtual void draw(const std::string& _draw_mode) override;

    //! handle ImGUI interface
    virtual void process_imgui() override;

private:

    //! initialize parameters for skull shape and FSTT distribution
    void init_parameters(const bool init_skull, const bool init_fstt);

    //! demonstrate fitting to a skull by loading preset parameters for skull
    //! shape and FSTT distribution so that the skull shape of the multilinear
    //! model fits a target point set of a skull
    void demo_skull_fit();

    //! demonstrate fitting to a skin surface by loading preset parameters for skull
    //! shape and FSTT distribution so that the skin surface shape of the multilinear
    //! model fits a target point set of a skin surface
    void demo_skin_fit();

protected:

    //! the skin mesh of the multilinear model
    SurfaceMeshGL skin_;
    //! the skull mesh of the multilinear model
    SurfaceMeshGL skull_;
    //! the target as a point set (to demonstrate fitting)
    SurfaceMeshGL points_;

    //! multilinear model
    MultilinearModel mlm_;

    //! parameters for skull shape
    Eigen::VectorXd w_skull_;
    //! parameters for FSTT distribution
    Eigen::VectorXd w_fstt_;

    //! switch: show mesh for skin surface
    bool show_skin_;
    //! switch: show mesh for skull
    bool show_skull_;
    //! switch: show point set
    bool show_points_;
    //! transparency value for skin rendering
    float alpha_;

    //! counter to save meshes with different filenames
    unsigned int conter_save_meshes_;
};

//=============================================================================

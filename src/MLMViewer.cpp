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
#include "utils.h"

#include <imgui.h>

#include <cfloat>
#include <iostream>
#include <sstream>

//=============================================================================

using namespace pmp;

//=============================================================================

MLMViewer::MLMViewer(const char* title, int width, int height, bool showgui)
    : TrackballViewer(title, width, height, showgui)
{
    // setup draw modes
    clear_draw_modes();
    add_draw_mode("Smooth Shading");
    add_draw_mode("Hidden Line");
    set_draw_mode("Smooth Shading");

    // set default visibility
    show_skin_   = true;
    show_skull_  = true;
    show_points_ = false;
    alpha_       = 1.0;

    conter_save_meshes_ = 1;

    // set colors and material
    skin_.set_front_color(vec3(1.0, 0.85, 0.8));
    skin_.set_diffuse(0.6);
    skin_.set_specular(0.1);
    skin_.set_shininess(50);

    skull_.set_front_color(vec3(0.7, 0.7, 0.7));
    skull_.set_diffuse(0.8);
    skull_.set_specular(0.1);
    skull_.set_shininess(100);

    points_.set_front_color(vec3(0.8, 0.2, 0.2));
    points_.set_specular(0.0);
}

//-----------------------------------------------------------------------------

MLMViewer::~MLMViewer()
{
}

//-----------------------------------------------------------------------------

bool MLMViewer::load_mlm(const char* dir)
{
    // load skin and skull meshes
    const std::string filenameSkin  = std::string(dir) + std::string("skin.off");
    const std::string filenameSkull = std::string(dir) + std::string("skull.off");
    if (!(skin_.read(filenameSkin) && skull_.read(filenameSkull)))
    {
        std::cerr << "Cannot load skin and skull meshes\n";
        return false;
    }


    // update scene center and bounds
    BoundingBox bb = skin_.bounds();
    set_scene(bb.center(), 0.5 * bb.size());

    // compute face & vertex normals, update face indices
    update_meshes();

    // print information
    std::cout << "Load " << filenameSkin << ": " 
        << skin_.n_vertices() << " vertices, " 
        << skin_.n_faces() << " faces\n";

    std::cout << "Load " << filenameSkull << ": " 
        << skull_.n_vertices() << " vertices, " 
        << skull_.n_faces() << " faces\n";


    // load means
    if (!mlm_.load_means(filenameSkin, filenameSkull))
    {
        std::cerr << "Cannot load means\n";
        return false;
    }


    // load mlm tensor, matrix U_skull, matrix U_fstt, eigenvalues_skull, eigenvalues_fstt
    // and compute multilinear model tensor
    std::cout << "Loading multilinear model (this may take a while) ..." << std::flush;
    if (!mlm_.load(dir))
    {
        std::cerr << "Cannot load multilinear model\n";
        return false;
    }
    std::cout << "done." << std::endl << std::flush;


    // initialize parameters and evaluate model
    init_parameters(true, true);
    mlm_.evaluate(skin_, skull_, w_skull_,  w_fstt_);
    update_meshes();


    return true;
}

//-----------------------------------------------------------------------------

void MLMViewer::evaluate_mlm()
{
    assert( skin_.n_vertices() == 24574);
    assert( skull_.n_vertices() == 69122);

    mlm_.evaluate(skin_, skull_, w_skull_,  w_fstt_);
}

//-----------------------------------------------------------------------------

void MLMViewer::update_meshes()
{
    // re-compute face and vertex normals
    skin_.update_opengl_buffers();

    // re-compute face and vertex normals
    skull_.update_opengl_buffers();
}

//-----------------------------------------------------------------------------                                                                                                                                                              
void MLMViewer::update_points()
{
    // update OpenGL buffers
    points_.update_opengl_buffers();
}

//-----------------------------------------------------------------------------

void MLMViewer::process_imgui()
{
    if (ImGui::CollapsingHeader("Visibility", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Show skin mesh", &show_skin_);
        ImGui::Checkbox("Show skull mesh", &show_skull_);        

        if (points_.n_vertices() > 0)
        {        
            ImGui::Checkbox("Show target points", &show_points_);
        }

        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("Transparency", &alpha_, 0.1f, 1.0f);
        ImGui::PopItemWidth();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Skull parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const double delta = 0.075;
        bool parametersChanged = false;

        ImGui::PushItemWidth(100);
        for (int i=0; i<7; ++i)
        {
            std::string s = std::string("Skull") + std::to_string(i+1);
            ImGui::Text( "%s", s.data() );
            ImGui::SameLine();
            ImGui::PushID(i);
            if (ImGui::Button("  +  "))
            {
                w_skull_(i) += delta;
                parametersChanged = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("  -  "))
            {
                w_skull_(i) -= delta;
                parametersChanged = true;
            }
            ImGui::PopID();
        }
        ImGui::PopItemWidth();

        if (parametersChanged)
        {
            evaluate_mlm();
            update_meshes();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("FSTT parameters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const double delta = 0.075;
        bool parametersChanged = false;

        ImGui::PushItemWidth(100);
        for (int i=0; i<4; ++i)
        {
            std::string s = std::string("FSTT") + std::to_string(i+1);
            ImGui::Text( "%s", s.data() );
            ImGui::SameLine();
            ImGui::PushID(7+i);
            if (ImGui::Button("  +  "))
            {
                w_fstt_(i) += delta;
                parametersChanged = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("  -  "))
            {
                w_fstt_(i) -= delta;
                parametersChanged = true;
            }
            ImGui::PopID();
        }
        ImGui::PopItemWidth();

        if (parametersChanged)
        {
            evaluate_mlm();
            update_meshes();
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Misc"))
    {
        if (ImGui::Button("Save meshes"))
        {
            const std::string filenameSkin  = "mesh_skin_" + std::to_string(conter_save_meshes_) + ".off";
            const std::string filenameSkull = "mesh_skull_" + std::to_string(conter_save_meshes_) + ".off";
            conter_save_meshes_++;
            skin_.write(filenameSkin);
            skull_.write(filenameSkull);
        }

        if (ImGui::Button("Reset parameters (Skull)"))
        {
            points_.clear();
            show_points_ = false;
            init_parameters(true, false); // skull only
            mlm_.evaluate(skin_, skull_, w_skull_,  w_fstt_);
            update_meshes();
        }

        if (ImGui::Button("Reset parameters (FSTT)"))
        {
            points_.clear();
            show_points_ = false;
            init_parameters(false, true); // FSTT only
            mlm_.evaluate(skin_, skull_, w_skull_,  w_fstt_);
            update_meshes();
        }

        if (ImGui::Button("Demo skull fit"))
        {
            demo_skull_fit();
        }

        if (ImGui::Button("Demo skin fit"))
        {
            demo_skin_fit();
        }
    }
}

//-----------------------------------------------------------------------------

void MLMViewer::draw(const std::string& drawMode)
{
    if (alpha_ < 1.0)
    {
        glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }

    // draw point set
    if (show_points_)
    {
        points_.draw(projection_matrix_, modelview_matrix_, "Points");
    }

    // draw skull
    if (show_skull_)
    {
        skull_.draw(projection_matrix_, modelview_matrix_, drawMode);
    }

    // draw skin
    if (show_skin_)
    {
        skin_.set_alpha(alpha_);
        skin_.draw(projection_matrix_, modelview_matrix_, drawMode);
    }

    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
}

//-----------------------------------------------------------------------------

void MLMViewer::init_parameters(const bool init_skull, const bool init_fstt)
{
    const unsigned int dimMode1 = mlm_.dim1();
    const unsigned int dimMode2 = mlm_.dim2();
    assert(dimMode1 && dimMode2);

    if (init_skull)
    {
        w_skull_ = Eigen::VectorXd::Zero(dimMode1);
        for (unsigned int i = 0; i < mlm_.U_skull().rows(); ++i)
        {
            for (unsigned int j = 0; j < mlm_.U_skull().cols(); ++j)
            {
                w_skull_(j) += mlm_.U_skull()(i, j);
            }
        }
        w_skull_ /= mlm_.U_skull().rows();
    }

    if (init_fstt)
    {
        w_fstt_ = Eigen::VectorXd::Zero(dimMode2);
        for (unsigned int i = 0; i < mlm_.U_fstt().rows(); ++i)
        {
            for (unsigned int j = 0; j < mlm_.U_fstt().cols(); ++j)
            {
                w_fstt_(j) += mlm_.U_fstt()(i, j);
            }
        }
        w_fstt_ /= mlm_.U_fstt().rows();
    }
}

//-----------------------------------------------------------------------------

void MLMViewer::demo_skull_fit()
{
    // load parameters
    if (!load_parameters(w_skull_, w_fstt_,
                         "../data/mlm_fits2skull/w_skull.scalars",
                         "../data/mlm_fits2skull/w_fstt.scalars"))
    {
        std::cerr << "Cannot load parameters for w_skull and w_fstt\n";
        return;
    }
    mlm_.evaluate(skin_, skull_, w_skull_,  w_fstt_);
    update_meshes();


    // load point set
    if (!points_.read("../data/mlm_fits2skull/demo_skull_ps.xyz"))
    {
        std::cerr << "Cannot load points\n";
        return;
    }
    update_points();


    // what to show
    show_skin_   = false;
    show_skull_  = true;
    show_points_ = true;

}

//-----------------------------------------------------------------------------

void MLMViewer::demo_skin_fit()
{
    // load parameters
    if (!load_parameters(w_skull_, w_fstt_,
                         "../data/mlm_fits2skin/w_skull.scalars",
                         "../data/mlm_fits2skin/w_fstt.scalars"))
    {
        std::cerr << "Cannot load parameters for w_skull and w_fstt\n";
        return;
    }
    mlm_.evaluate(skin_, skull_, w_skull_,  w_fstt_);
    update_meshes();


    // load point set
    if (!points_.read("../data/mlm_fits2skin/demo_head_ps.xyz"))
    {
        std::cerr << "Cannot load points\n";
        return;
    }
    update_points();


    // show skin and skull
    show_skin_   = true;
    show_skull_  = true;
    show_points_ = true;
}

//=============================================================================

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>

#include <boost/filesystem.hpp>

#include <glk/path.hpp>
#include <guik/recent_files.hpp>
#include <guik/model_control.hpp>
#include <guik/viewer/light_viewer.hpp>

namespace py = pybind11;


std::shared_ptr<guik::LightViewer> instance() {
  static bool is_first = true;
  if(is_first) {
    py::gil_scoped_acquire acquire;
    py::object pyridescence = py::module::import("pyridescence");
    boost::filesystem::path path(pyridescence.attr("__file__").cast<std::string>());

    glk::set_data_path(path.parent_path().string() + "/data");
    is_first = false;
  }

  return guik::LightViewer::instance();
}

void define_guik(py::module_& m) {
  py::module_ guik_ = m.def_submodule("guik", "");

  // classess
  py::class_<guik::ShaderSetting, std::shared_ptr<guik::ShaderSetting>>(guik_, "ShaderSetting")
    .def("add", &guik::ShaderSetting::add<float>)
    .def("make_transparent", &guik::ShaderSetting::make_transparent);

  py::class_<guik::Rainbow, guik::ShaderSetting, std::shared_ptr<guik::Rainbow>>(guik_, "Rainbow")
    .def(py::init<>())
    .def(py::init<Eigen::Matrix4f>());

  py::class_<guik::VertexColor, guik::ShaderSetting, std::shared_ptr<guik::VertexColor>>(guik_, "VertexColor")
    .def(py::init<>())
    .def(py::init<Eigen::Matrix4f>());

  py::class_<guik::FlatColor, guik::ShaderSetting, std::shared_ptr<guik::FlatColor>>(guik_, "FlatColor")
    .def(py::init<float, float, float, float>())
    .def(py::init<float, float, float, float, Eigen::Matrix4f>());

  py::class_<guik::ModelControl>(guik_, "ModelControl")
    .def(py::init<std::string, Eigen::Matrix4f>(), "")
    .def("draw_ui", &guik::ModelControl::draw_ui, "")
    .def("draw_gizmo_ui", &guik::ModelControl::draw_gizmo_ui, "")
    .def("draw_gizmo", [](guik::ModelControl& control) {
      auto viewer = guik::LightViewer::instance();
      control.draw_gizmo(0, 0, viewer->canvas_size().x(), viewer->canvas_size().y(), viewer->view_matrix(), viewer->projection_matrix()); }
    )
    .def("model_matrix", &guik::ModelControl::model_matrix, "")
    .def("set_model_matrix", &guik::ModelControl::set_model_matrix, "");

  // methods
  guik_.def("anon", &guik::anon, "");

  // LightViewerContext
  py::class_<guik::LightViewerContext, std::shared_ptr<guik::LightViewerContext>>(guik_, "LightViewerContext")
    .def("clear", &guik::LightViewerContext::clear)
    .def("clear_text", &guik::LightViewerContext::clear_text)
    .def("append_text", &guik::LightViewerContext::append_text)
    .def("remove_drawable", &guik::LightViewerContext::remove_drawable)
    .def("update_drawable", &guik::LightViewerContext::update_drawable)
    .def("reset_center", &guik::LightViewerContext::reset_center)
    .def("lookat", &guik::LightViewerContext::lookat)
    .def("set_draw_xy_grid", &guik::LightViewerContext::set_draw_xy_grid, "")
    .def("set_screen_effect", &guik::LightViewerContext::set_screen_effect, "")
    .def("enable_normal_buffer", &guik::LightViewerContext::enable_normal_buffer, "")
    .def("enable_info_buffer", &guik::LightViewerContext::enable_info_buffer, "")
    .def("use_orbit_camera_control", &guik::LightViewerContext::use_orbit_camera_control, "", py::arg("distance") = 80.0, py::arg("theta") = 0.0, py::arg("phi") = -60.0 * M_PI / 180.0)
    .def("use_orbit_camera_control_xz", &guik::LightViewerContext::use_orbit_camera_control_xz, "", py::arg("distance") = 80.0, py::arg("theta") = 0.0, py::arg("phi") = 0.0)
    .def("use_topdown_camera_control", &guik::LightViewerContext::use_topdown_camera_control, "", py::arg("distance") = 80.0, py::arg("theta") = 0.0)
    ;

  // LightViewer
  py::class_<guik::LightViewer, guik::LightViewerContext, std::shared_ptr<guik::LightViewer>>(guik_, "LightViewer")
    .def_static("instance", [] { return instance(); })
    .def("sub_viewer", [] (guik::LightViewer& viewer, const std::string& name) { return viewer.sub_viewer(name); })
    .def("sub_viewer", [] (guik::LightViewer& viewer, const std::string& name, const std::tuple<int, int>& size) { return viewer.sub_viewer(name, Eigen::Vector2i(std::get<0>(size), std::get<1>(size))); })

    .def("spin", &guik::LightViewer::spin, "")
    .def("spin_once", &guik::LightViewer::spin_once, "")
    .def("spin_until_click", &guik::LightViewer::spin_until_click, "")

    // LightViewerContext methods
    .def("clear", &guik::LightViewer::clear)
    .def("clear_text", &guik::LightViewer::clear_text)
    .def("append_text", &guik::LightViewer::append_text)
    .def("register_ui_callback", &guik::LightViewer::register_ui_callback)

    .def("remove_drawable", &guik::LightViewer::remove_drawable)
    .def("update_drawable", &guik::LightViewer::update_drawable)
    .def("reset_center", &guik::LightViewer::reset_center)
    .def("lookat", &guik::LightViewer::lookat)
    .def("set_draw_xy_grid", &guik::LightViewer::set_draw_xy_grid, "")
    .def("set_screen_effect", &guik::LightViewer::set_screen_effect, "")
    .def("enable_normal_buffer", &guik::LightViewer::enable_normal_buffer, "")
    .def("enable_info_buffer", &guik::LightViewer::enable_info_buffer, "")
    .def("use_orbit_camera_control", &guik::LightViewer::use_orbit_camera_control, "", py::arg("distance") = 80.0, py::arg("theta") = 0.0, py::arg("phi") = -60.0 * M_PI / 180.0)
    .def("use_orbit_camera_control_xz", &guik::LightViewer::use_orbit_camera_control_xz, "", py::arg("distance") = 80.0, py::arg("theta") = 0.0, py::arg("phi") = 0.0)
    .def("use_topdown_camera_control", &guik::LightViewer::use_topdown_camera_control, "", py::arg("distance") = 80.0, py::arg("theta") = 0.0)
    ;
}
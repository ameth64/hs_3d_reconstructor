set(3D_RECONSTRUCTOR_SOURCE
  "main.cpp"
  "main_window.cpp"
  "manager_pane.cpp"
  "blocks_tree_widget.cpp"
  "new_project_config_dialog.cpp"
  "photos_pane.cpp"
  "blocks_pane.cpp"
  "photos_tree_widget.cpp"
  "property_field_asignment_widget.cpp"
  "property_field_asignment_dialog.cpp"
  "photogroup_pos_configure_widget.cpp"
  "photogroup_info_widget.cpp"
  "photogroup_info_pos_wrap_widget.cpp"
  "photo_files_select_widget.cpp"
  "photos_import_configure_dialog.cpp"
  "photogroup_info_setting_widget.cpp"
  "photo_display_widget.cpp"
  "photo_import_check_widget.cpp"
  "photo_import_check_dialog.cpp"
  "image_container_widget.cpp"
  "image_opengl_window.cpp"
  "block_photos_select_widget.cpp"
  "block_photos_select_dialog.cpp"
  "block_info_widget.cpp"
  "feature_match_configure_widget.cpp"
  "photo_orientation_configure_widget.cpp"
  "photo_orientation_info_widget.cpp"
  "point_cloud_configure_widget.cpp"
  "surface_model_configure_widget.cpp"
  "texture_configure_widget.cpp"
  "feature_match_export_widget.cpp"
  "photo_orientation_export_widget.cpp"
  "point_cloud_export_widget.cpp"
  "surface_model_export_widget.cpp"
  "texture_export_widget.cpp"
  "workflow_configure_widget.cpp"
  "workflow_configure_dialog.cpp"
  "scene_window.cpp"
  "gcps_table_widget.cpp"
  "gcps_pane.cpp"
  "tiepoint_measure_widget.cpp"
  "image_measure_opengl_window.cpp"
  "settings_dialog.cpp"
  "start_up_dialog.cpp"
  "progress_dialog.cpp"
  "default_longitude_latitude_convertor.cpp"
  "photo_import_check_widget.cpp"
  "photo_import_check_dialog.cpp"
  "gcp_constrained_optimization_config_widget.cpp"
  "gcp_constrained_optimization_config_dialog.cpp"
)

if (MSVC)
  set_source_files_properties(
    "gcps_pane.cpp"
    PROPERTIES COMPILE_FLAGS "/bigobj")
endif()

set(3D_RECONSTRUCTOR_RESOURCE
  "resource/3d_reconstructor.qrc"
)

set(3D_RECONSTRUCTOR_UI_FILES
  "ui/main_window.ui"
)

set(3D_RECONSTRUCTOR_TS
  "i18n/zh_CN.ts"
)

find_package(OpenGL REQUIRED)

add_definitions(-DCGAL_NO_AUTOLINK)

hslib_add_qt_executable(hs_3d_reconstructor
  SOURCES ${3D_RECONSTRUCTOR_SOURCE}
  QT_MODULES "Widgets" "Core" "OpenGL" "LinguistTools"
  UI_FILES ${3D_RECONSTRUCTOR_UI_FILES}
  RESOURCES ${3D_RECONSTRUCTOR_RESOURCE}
  TS_FILES ${3D_RECONSTRUCTOR_TS}
  )
yong_add_dependence(hs_3d_reconstructor hs_3d_reconstructor
  DEPENDENT_LOCAL_LIBS hs_3d_reconstructor_database
  DEPENDENT_LOCAL_LIBS hs_3d_reconstructor_workflow
  DEPENDENT_PROJECT boost
    DEPENDENT_LIBS boost_system
                   boost_filesystem
                   boost_thread
                   boost_chrono
                   boost_regex
                   boost_serialization
  DEPENDENT_PROJECT cereal
    HEADER_ONLY
  DEPENDENT_PROJECT ceres
    DEPENDENT_LIBS ceres
  DEPENDENT_PROJECT CGAL
    DEPENDENT_LIBS CGAL
  DEPENDENT_PROJECT eigen
    HEADER_ONLY
  DEPENDENT_PROJECT exiv2
    DEPENDENT_LIBS exiv2
  DEPENDENT_PROJECT flann
    DEPENDENT_LIBS flann
  DEPENDENT_PROJECT jpeg_turbo
    DEPENDENT_LIBS jpeg_turbo
  DEPENDENT_PROJECT lemon
    DEPENDENT_LIBS lemon
  DEPENDENT_PROJECT OpenCV
    DEPENDENT_LIBS opencv_calib3d
                   opencv_core
                   opencv_cudaarithm
                   opencv_imgproc
                   opencv_features2d
                   opencv_highgui
                   opencv_flann
                   opencv_nonfree
  DEPENDENT_PROJECT openmvg
    DEPENDENT_LIBS openMVG_sfm
                   openMVG_multiview
                   openMVG_lInftyComputerVision
                   openMVG_numeric
                   openMVG_kvld
                   openMVG_features
                   openMVG_system
                   openMVG_stlplus
                   openMVG_image
  DEPENDENT_PROJECT osi_clp
    DEPENDENT_LIBS clp CoinUtils Osi OsiClpSolver
  DEPENDENT_PROJECT png
    DEPENDENT_LIBS png
  DEPENDENT_PROJECT proj
    DEPENDENT_LIBS proj
  DEPENDENT_PROJECT sqlite
    DEPENDENT_LIBS sqlite
  DEPENDENT_PROJECT tiff
    DEPENDENT_LIBS tiff
  DEPENDENT_PROJECT zlib
    DEPENDENT_LIBS zlib
  DEPENDENT_PROJECT hs_cartographics
    DEPENDENT_LIBS cartographics_qt
  DEPENDENT_PROJECT hs_graphics
    DEPENDENT_LIBS graphics_qt
  DEPENDENT_PROJECT hs_image_io
    DEPENDENT_LIBS whole_io
  DEPENDENT_PROJECT hs_math
    HEADER_ONLY
  DEPENDENT_PROJECT hs_optimizor
    HEADER_ONLY
  DEPENDENT_PROJECT hs_progress
    DEPENDENT_LIBS progress_utility
  DEPENDENT_PROJECT hs_sfm
    HEADER_ONLY
  DEPENDENT_PROJECT hs_texture
    HEADER_ONLY
  )
hslib_add_mkl_dep(hs_3d_reconstructor)
target_link_libraries(hs_3d_reconstructor ${OPENGL_LIBRARIES})
hslib_add_executable_end(hs_3d_reconstructor)

cmake_minimum_required(VERSION 2.8)

include("cmake/hslib.cmake")
include("cmake/hsprj.cmake")
include("cmake/hsgui.cmake")

set(HSPRJ_NAME "hs_3d_reconstructor")
hs_project(${HSPRJ_NAME})

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")
endif ()

include_directories("source")

find_package(OpenGL REQUIRED)

set(RECONSTRUCTOR_NAME "hs_3d_reconstructor")
set(RECONSTRUCTOR_SOURCES
  "source/gui/main.cpp"
  "source/gui/main_window.cpp"
  "source/gui/manager_pane.cpp"
  "source/gui/blocks_tree_widget.cpp"
  "source/gui/new_project_config_dialog.cpp"
  "source/gui/photos_pane.cpp"
  "source/gui/blocks_pane.cpp"
  "source/gui/photos_tree_widget.cpp"
  "source/gui/property_field_asignment_widget.cpp"
  "source/gui/property_field_asignment_dialog.cpp"
  "source/gui/photogroup_pos_configure_widget.cpp"
  "source/gui/photogroup_info_widget.cpp"
  "source/gui/photogroup_info_pos_wrap_widget.cpp"
  "source/gui/photo_files_select_widget.cpp"
  "source/gui/photos_import_configure_dialog.cpp"
  "source/gui/photogroup_info_setting_widget.cpp"
  "source/gui/photo_display_widget.cpp"
  "source/gui/image_container_widget.cpp"
  "source/gui/image_opengl_window.cpp"
  "source/gui/block_photos_select_widget.cpp"
  "source/gui/block_photos_select_dialog.cpp"
  "source/gui/block_info_widget.cpp"
  "source/gui/feature_match_configure_widget.cpp"
  "source/gui/photo_orientation_configure_widget.cpp"
  "source/gui/point_cloud_configure_widget.cpp"
  "source/gui/surface_model_configure_widget.cpp"
  "source/gui/texture_configure_widget.cpp"
  "source/gui/feature_match_export_widget.cpp"
  "source/gui/photo_orientation_export_widget.cpp"
  "source/gui/point_cloud_export_widget.cpp"
  "source/gui/surface_model_export_widget.cpp"
  "source/gui/texture_export_widget.cpp"
  "source/gui/workflow_configure_widget.cpp"
  "source/gui/workflow_configure_dialog.cpp"
  "source/gui/scene_window.cpp"
  "source/gui/gcps_table_widget.cpp"
  "source/gui/gcps_pane.cpp"
  "source/gui/tiepoint_measure_widget.cpp"
  "source/gui/image_measure_opengl_window.cpp"
  "source/gui/settings_dialog.cpp"
  )

set(RECONSTRUCTOR_QT_MODULES
  "Widgets"
  "Core"
  )

set(RECONSTRUCTOR_UI_FILES
  "ui/main_window.ui"
  )

set(RECONSTRUCTOR_RESOURCES
  "resource/3d_reconstructor.qrc"
  )

hs_add_qt_executable(${RECONSTRUCTOR_NAME}
  SOURCES ${RECONSTRUCTOR_SOURCES}
  QT_MODULES ${RECONSTRUCTOR_QT_MODULES}
  UI_FILES ${RECONSTRUCTOR_UI_FILES}
  RESOURCES ${RECONSTRUCTOR_RESOURCES}
  )

hslib_add_dep(${RECONSTRUCTOR_NAME}
  3RDPARTY_DEPS boost DEP_LIBS boost_filesystem boost_system
  3RDPARTY_DEPS eigen HEADONLY
  3RDPARTY_DEPS exiv2
  3RDPARTY_DEPS glew
  3RDPARTY_DEPS proj
  3RDPARTY_DEPS ceres DEP_LIBS ceres
  3RDPARTY_DEPS opencv
    DEP_LIBS opencv_calib3d
             opencv_core
             opencv_cudaarithm
             opencv_imgproc
             opencv_features2d
             opencv_highgui
             opencv_flann
             opencv_nonfree
  3RDPARTY_DEPS tiff DEP_LIBS tiff
  3RDPARTY_DEPS jpeg DEP_LIBS jpeg
  3RDPARTY_DEPS png DEP_LIBS png
  3RDPARTY_DEPS z DEP_LIBS z
  HS_DEPS hs_math HEADONLY
  HS_DEPS hs_sfm HEADONLY
  HS_DEPS hs_image_io DEP_LIBS whole_io
  HS_DEPS hs_graphics DEP_LIBS graphics_qt
  HS_DEPS hs_cartographics DEP_LIBS cartographics_qt
  HS_DEPS hs_progress DEP_LIBS progress_utility
  LOCAL_DEPS database
  LOCAL_DEPS workflow
  )
target_link_libraries(${RECONSTRUCTOR_NAME} ${OPENGL_LIBRARIES})

hslib_post_build(${RECONSTRUCTOR_NAME})

set(DATABASE_NAME "database")
set(DATABASE_SOURCES
  "source/database/database.cpp"
  "source/database/database_mediator.cpp"
  "source/database/field.cpp"
  "source/database/condition.cpp"
  "source/database/demo_resource.cpp"
  "source/database/block_resource.cpp"
  "source/database/photogroup_resource.cpp"
  "source/database/photo_resource.cpp"
  "source/database/ground_control_point_resource.cpp"
  "source/database/photo_measure_resource.cpp"
  "source/database/feature_match_resource.cpp"
  "source/database/photo_orientation_resource.cpp"
  "source/database/point_cloud_resource.cpp"
  "source/database/surface_model_resource.cpp"
  "source/database/texture_resource.cpp"
  "source/database/photo_block_relation_resource.cpp"
  )
add_definitions(-DBOOST_ALL_NO_LIB)
hslib_add_library(${DATABASE_NAME} ${HSPRJ_NAME} ${DATABASE_SOURCES})
hslib_add_dep(${DATABASE_NAME}
  3RDPARTY_DEPS boost DEP_LIBS boost_filesystem boost_system
  3RDPARTY_DEPS sqlite
  )
hslib_post_build(${DATABASE_NAME})

set(DATABASE_UTEST_SOURCES
  "unit_test/database/main.cpp"
  "unit_test/database/test_condition.cpp"
  "unit_test/database/test_demo_resource.cpp"
  "unit_test/database/test_block_resource.cpp"
  "unit_test/database/test_photogroup_resource.cpp"
  "unit_test/database/test_photo_resource.cpp"
  "unit_test/database/test_ground_control_point_resource.cpp"
  "unit_test/database/test_feature_match_resource.cpp"
  "unit_test/database/test_photo_orientation_resource.cpp"
  "unit_test/database/test_point_cloud_resource.cpp"
  "unit_test/database/test_surface_model_resource.cpp"
  "unit_test/database/test_texture_resource.cpp"
  "unit_test/database/test_photo_block_relation_resource.cpp"
  )
hslib_add_utest(${DATABASE_NAME} SOURCES ${DATABASE_UTEST_SOURCES})
hslib_add_dep(${DATABASE_NAME}_utest
  3RDPARTY_DEPS boost DEP_LIBS boost_filesystem boost_system
  3RDPARTY_DEPS sqlite
  )
hslib_post_build(${DATABASE_NAME}_utest)

set(WORKFLOW_NAME "workflow")
set(WORKFLOW_SOURCES
  "source/workflow/common/workflow_step.cpp"
  "source/workflow/feature_match/opencv_feature_match.cpp"
  "source/workflow/photo_orientation/incremental_photo_orientation.cpp"
  )
hslib_add_library(${WORKFLOW_NAME} ${HSPRJ_NAME} ${WORKFLOW_SOURCES})
hslib_add_dep(${WORKFLOW_NAME}
  3RDPARTY_DEPS eigen HEADONLY
  3RDPARTY_DEPS opencv
    DEP_LIBS opencv_calib3d
             opencv_core
             opencv_cudaarithm
             opencv_imgproc
             opencv_features2d
             opencv_highgui
             opencv_flann
             opencv_nonfree
             3RDPARTY_DEPS ceres DEP_LIBS ceres
                                 DEP_STATIC_LIBS ceres_miniglog
  3RDPARTY_DEPS tiff DEP_LIBS tiff
  3RDPARTY_DEPS jpeg DEP_LIBS jpeg
  3RDPARTY_DEPS png DEP_LIBS png
  3RDPARTY_DEPS z DEP_LIBS z
  HS_DEPS hs_image_io DEP_LIBS whole_io
  HS_DEPS hs_progress DEP_LIBS progress_utility
  HS_DEPS hs_optimizor HEADONLY
  HS_DEPS hs_fit HEADONLY
  HS_DEPS hs_math HEADONLY
  HS_DEPS hs_sfm HEADONLY
  )
if (MSVC)
  set_source_files_properties(
    "source/workflow/photo_orientation/incremental_photo_orientation.cpp"
    PROPERTIES COMPILE_FLAGS "/bigobj")
endif()


hs_add_qt_executable(test_manager_pane SOURCES
  "unit_test/gui/test_manager_pane.cpp"
  "unit_test/gui/action_tester.cpp"
  "source/gui/manager_pane.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_blocks_tree_widget SOURCES
  "unit_test/gui/test_utility.cpp"
  "unit_test/gui/test_blocks_tree_widget.cpp"
  "source/gui/blocks_tree_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_photos_tree_widget SOURCES
  "unit_test/gui/test_utility.cpp"
  "unit_test/gui/test_photos_tree_widget.cpp"
  "source/gui/photos_tree_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hslib_add_dep(test_photos_tree_widget
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  HS_DEPS hs_math HEADONLY
  HS_DEPS hs_sfm HEADONLY
  )
hslib_post_build(test_photos_tree_widget)

hs_add_qt_executable(test_opengl_widget SOURCES
  "unit_test/gui/test_opengl_widget.cpp"
  "source/gui/opengl_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core" "OpenGL"
  RESOURCES "resource/3d_reconstructor.qrc")
target_link_libraries(test_opengl_widget ${OPENGL_LIBRARIES})

hs_add_qt_executable(test_block_info_widget SOURCES
  "unit_test/gui/test_block_info_widget.cpp"
  "source/gui/block_info_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_feature_match_info_widget SOURCES
  "unit_test/gui/test_utility.cpp"
  "unit_test/gui/test_feature_match_info_widget.cpp"
  "source/gui/feature_match_info_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hs_add_qt_executable(test_photo_orientation_info_widget SOURCES
  "unit_test/gui/test_utility.cpp"
  "unit_test/gui/test_photo_orientation_info_widget.cpp"
  "source/gui/photo_orientation_info_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hs_add_qt_executable(test_image_opengl_window SOURCES
  "unit_test/gui/test_image_opengl_window.cpp"
  "source/gui/image_opengl_window.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hslib_add_dep(test_image_opengl_window
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  3RDPARTY_DEPS glew
  3RDPARTY_DEPS tiff
  3RDPARTY_DEPS jpeg
  3RDPARTY_DEPS png
  3RDPARTY_DEPS z
  HS_DEPS hs_math_HEADONLY
  HS_DEPS hs_image_io DEP_LIBS whole_io
  HS_DEPS hs_graphics HEADONLY
  )
target_link_libraries(test_image_opengl_window ${OPENGL_LIBRARIES})
hslib_post_build(test_image_opengl_window)

hs_add_qt_executable(test_tiepoint_measure_widget SOURCES
  "unit_test/gui/test_tiepoint_measure_widget.cpp"
  "source/gui/image_opengl_window.cpp"
  "source/gui/image_container_widget.cpp"
  "source/gui/tiepoint_measure_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core")
hslib_add_dep(test_tiepoint_measure_widget
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  3RDPARTY_DEPS glew
  3RDPARTY_DEPS tiff
  3RDPARTY_DEPS jpeg
  3RDPARTY_DEPS png
  3RDPARTY_DEPS z
  HS_DEPS hs_math_HEADONLY
  HS_DEPS hs_image_io DEP_LIBS whole_io
  HS_DEPS hs_graphics DEP_LIBS graphics_qt
  )
target_link_libraries(test_tiepoint_measure_widget ${OPENGL_LIBRARIES})
hslib_post_build(test_tiepoint_measure_widget)

hs_add_qt_executable(test_image_opengl_widget SOURCES
  "unit_test/gui/test_image_opengl_widget.cpp"
  "source/gui/opengl_widget.cpp"
  "source/gui/image_opengl_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core" "OpenGL")
hslib_add_dep(test_image_opengl_widget
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  3RDPARTY_DEPS glew
  3RDPARTY_DEPS tiff
  3RDPARTY_DEPS jpeg
  3RDPARTY_DEPS png
  3RDPARTY_DEPS z
  HS_DEPS hs_math_HEADONLY
  HS_DEPS hs_image_io DEP_LIBS whole_io
  HS_DEPS hs_graphics HEADONLY
  )
target_link_libraries(test_image_opengl_widget ${OPENGL_LIBRARIES})
hslib_post_build(test_image_widget)

hs_add_qt_executable(test_image_container_widget SOURCES
  "unit_test/gui/test_image_container_widget.cpp"
  "source/gui/image_container_widget.cpp"
  "source/gui/image_opengl_window.cpp"
  QT_MODULES "Test" "Widgets" "Core" "OpenGL")
hslib_add_dep(test_image_container_widget
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  3RDPARTY_DEPS glew
  3RDPARTY_DEPS tiff
  3RDPARTY_DEPS jpeg
  3RDPARTY_DEPS png
  3RDPARTY_DEPS z
  HS_DEPS hs_math_HEADONLY
  HS_DEPS hs_image_io DEP_LIBS whole_io
  HS_DEPS hs_graphics HEADONLY
  )
target_link_libraries(test_image_container_widget ${OPENGL_LIBRARIES})
hslib_post_build(test_image_container_widget)

hs_add_qt_executable(test_photo_files_select_widget SOURCES
  "unit_test/gui/test_photo_files_select_widget.cpp"
  "source/gui/photo_files_select_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hslib_add_dep(test_photo_files_select_widget
  3RDPARTY_DEPS exiv2
  HS_DEPS hs_image_io DEP_LIBS whole_io)
hslib_post_build(test_photo_files_select_widget)

#hs_add_qt_executable(test_photogroup_info_widget SOURCES
#  "unit_test/gui/test_photogroup_info_widget.cpp"
#  "source/gui/photogroup_info_widget.cpp"
#  QT_MODULES "Test" "Widgets" "Core"
#  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_photogroup_info_pos_wrap_widget SOURCES
  "unit_test/gui/test_photogroup_info_pos_wrap_widget.cpp"
  "source/gui/property_field_asignment_widget.cpp"
  "source/gui/property_field_asignment_dialog.cpp"
  "source/gui/photogroup_pos_configure_widget.cpp"
  "source/gui/photogroup_info_widget.cpp"
  "source/gui/photogroup_info_pos_wrap_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_photos_import_configure_dialog SOURCES
  "unit_test/gui/test_photos_import_configure_dialog.cpp"
  "source/gui/property_field_asignment_widget.cpp"
  "source/gui/property_field_asignment_dialog.cpp"
  "source/gui/photogroup_pos_configure_widget.cpp"
  "source/gui/photogroup_info_widget.cpp"
  "source/gui/photogroup_info_pos_wrap_widget.cpp"
  "source/gui/photo_files_select_widget.cpp"
  "source/gui/photos_import_configure_dialog.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hslib_add_dep(test_photos_import_configure_dialog
  3RDPARTY_DEPS exiv2
  HS_DEPS hs_image_io DEP_LIBS whole_io)
hslib_post_build(test_photos_import_configure_dialog)

#hs_add_qt_executable(test_property_field_asignment_dialog SOURCES
#  "unit_test/gui/test_property_field_asignment_dialog.cpp"
#  "source/gui/property_field_asignment_widget.cpp"
#  "source/gui/property_field_asignment_dialog.cpp"
#  QT_MODULES "Test" "Widgets" "Core"
#  RESOURCES "resource/3d_reconstructor.qrc")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)


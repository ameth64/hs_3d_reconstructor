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

set(RECONSTRUCTOR_NAME "hs_3d_reconstructor")
set(RECONSTRUCTOR_SOURCES
  "source/main.cpp"
  "source/main_window.cpp"
  "source/manager_pane.cpp"
  "source/blocks_tree_widget.cpp"
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
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  HS_DEPS hs_math HEADONLY
  HS_DEPS hs_sfm HEADONLY
  )

hslib_post_build(${RECONSTRUCTOR_NAME})


hs_add_qt_executable(test_manager_pane SOURCES
  "unit_test/test_manager_pane.cpp"
  "unit_test/action_tester.cpp"
  "source/manager_pane.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_blocks_tree_widget SOURCES
  "unit_test/test_utility.cpp"
  "unit_test/test_blocks_tree_widget.cpp"
  "source/blocks_tree_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

hs_add_qt_executable(test_photos_tree_widget SOURCES
  "unit_test/test_utility.cpp"
  "unit_test/test_photos_tree_widget.cpp"
  "source/photos_tree_widget.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")
hslib_add_dep(test_photos_tree_widget
  3RDPARTY_DEPS boost HEADONLY
  3RDPARTY_DEPS eigen HEADONLY
  HS_DEPS hs_math HEADONLY
  HS_DEPS hs_sfm HEADONLY
  )
hslib_post_build(test_photos_tree_widget)

hs_add_qt_executable(test_opengl_window SOURCES
  "unit_test/test_opengl_window.cpp"
  "source/opengl_window.cpp"
  QT_MODULES "Test" "Widgets" "Core"
  RESOURCES "resource/3d_reconstructor.qrc")

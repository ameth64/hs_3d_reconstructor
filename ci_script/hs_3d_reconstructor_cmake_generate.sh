#!/bin/bash

. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/initialize.sh "$@"

. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "boost"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "cereal"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost" "yong" "CGAL"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "eigen"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "eigen" "yong" "ceres"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "flann"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "glew"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "GLFW"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "gtest"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" \
  --extra_cmake_defs "-DNASM=${CI_NASM_PATH}" "yong" "jpeg_turbo"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "lemon"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "osi_clp"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "proj"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "sqlite"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" "yong" "zlib"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "zlib" "yong" "exiv2"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "zlib" "yong" "png"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "jpeg_turbo zlib" "yong" "tiff"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "yong_injector" --deps "cereal ceres eigen flann jpeg_turbo lemon osi_clp png tiff zlib" \
  "yong" "openmvg"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "jpeg_turbo png tiff zlib" "yong" "OpenCV"

. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "gtest" "yong" "hs_progress"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost gtest sqlite" "yong" "hs_database"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "jpeg_turbo png tiff zlib gtest" "yong" "hs_image_io"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --extra_cmake_defs "-DHSLIB_BUILD_DOC=0" \
  --submodules "ALL" --deps "boost gtest eigen cereal" "yong" "hs_math"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost cereal eigen gtest hs_math" "yong" "hs_fit"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost cereal eigen gtest proj hs_math" \
  "yong" "hs_cartographics"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost cereal eigen gtest hs_math" \
  "yong" "hs_test_utility"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost cereal eigen gtest hs_math hs_test_utility" \
  "yong" "hs_optimizor"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --extra_cmake_defs "-DHSLIB_BUILD_DOC=0" \
  --submodules "ALL" "yong" "hs_sfm"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" \
  --extra_cmake_defs "-DBUILD_QT=1" \
  --deps "boost cereal eigen glew GLFW gtest jpeg_turbo zlib OpenSceneGraph png tiff hs_image_io hs_math hs_test_utility hs_sfm" "yong" "hs_graphics"
. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/resolve_dependency.sh \
  --submodules "ALL" --deps "boost eigen cereal gtest jpeg_turbo zlib png tiff hs_image_io hs_math hs_progress hs_test_utility hs_graphics hs_sfm" "yong" "hs_texture"

. ${CI_PROJECT_DIR}/ci_script/hslib_ci_script/cmake_generate.sh \
  --extra_cmake_defs "-DHSLIB_BUILD_DOC=0" --deps "boost cereal ceres CGAL eigen exiv2 flann glew GLFW gtest jpeg_turbo lemon OpenCV openmvg OpenSceneGraph osi_clp png proj sqlite tiff zlib hs_cartographics hs_fit hs_graphics hs_image_io hs_math hs_optimizor hs_progress hs_sfm hs_test_utility hs_texture"

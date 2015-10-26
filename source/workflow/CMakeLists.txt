set(WORKFLOW_SOURCE
  "common/workflow_step.cpp"
  "feature_match/feature_match_config.cpp"
  "feature_match/feature_match_step.cpp"
  #"feature_match/openmvg_feature_match.cpp"
  "feature_match/opencv_feature_match.cpp"
  "photo_orientation/incremental_photo_orientation.cpp"
  "point_cloud/pmvs_point_cloud.cpp"
  #"mesh_surface/poisson_surface_model.cpp"
  "mesh_surface/surface_model_config.cpp"
  "mesh_surface/delaunay_surface_model.cpp"
  "texture/rough_texture.cpp"
  )
if (MSVC)
  set_source_files_properties(
    "photo_orientation/incremental_photo_orientation.cpp"
    #"feature_match/openmvg_feature_match.cpp"
    PROPERTIES COMPILE_FLAGS "/bigobj")
endif()

hslib_add_library(hs_3d_reconstructor_workflow
  SOURCES ${WORKFLOW_SOURCE}
  LIB_TYPE STATIC)
yong_add_dependence(hs_3d_reconstructor hs_3d_reconstructor_workflow
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
  DEPENDENT_PROJECT tiff
    DEPENDENT_LIBS tiff
  DEPENDENT_PROJECT zlib
    DEPENDENT_LIBS zlib
  DEPENDENT_PROJECT hs_fit
    HEADER_ONLY
  DEPENDENT_PROJECT hs_graphics
    HEADER_ONLY
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
hslib_add_mkl_dep(hs_3d_reconstructor_workflow)
hslib_add_library_end(hs_3d_reconstructor_workflow)

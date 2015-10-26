set(DATABASE_SOURCE
  "database.cpp"
  "database_mediator.cpp"
  "field.cpp"
  "condition.cpp"
  "demo_resource.cpp"
  "block_resource.cpp"
  "photogroup_resource.cpp"
  "photo_resource.cpp"
  "ground_control_point_resource.cpp"
  "photo_measure_resource.cpp"
  "feature_match_resource.cpp"
  "photo_orientation_resource.cpp"
  "point_cloud_resource.cpp"
  "surface_model_resource.cpp"
  "texture_resource.cpp"
  "photo_block_relation_resource.cpp"
  )

hslib_add_library(hs_3d_reconstructor_database
  SOURCES ${DATABASE_SOURCE}
  LIB_TYPE STATIC)
yong_add_dependence(hs_3d_reconstructor hs_3d_reconstructor_database
  DEPENDENT_PROJECT boost
    DEPENDENT_LIBS boost_system boost_filesystem
  DEPENDENT_PROJECT cereal
    HEADER_ONLY
  DEPENDENT_PROJECT eigen
    HEADER_ONLY
  DEPENDENT_PROJECT sqlite
    DEPENDENT_LIBS sqlite
  DEPENDENT_PROJECT hs_graphics
    HEADER_ONLY
  DEPENDENT_PROJECT hs_image_io
    DEPENDENT_LIBS whole_io
  DEPENDENT_PROJECT hs_math
    HEADER_ONLY
  DEPENDENT_PROJECT hs_progress
    DEPENDENT_LIBS progress_utility
  DEPENDENT_PROJECT hs_sfm
  )
hslib_add_mkl_dep(hs_3d_reconstructor_database)
hslib_add_library_end(hs_3d_reconstructor_database)

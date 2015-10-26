set(DATABASE_UTEST_SOURCES
  "main.cpp"
  "test_condition.cpp"
  "test_demo_resource.cpp"
  "test_block_resource.cpp"
  "test_photogroup_resource.cpp"
  "test_photo_resource.cpp"
  "test_ground_control_point_resource.cpp"
  "test_feature_match_resource.cpp"
  "test_photo_orientation_resource.cpp"
  "test_point_cloud_resource.cpp"
  "test_surface_model_resource.cpp"
  "test_texture_resource.cpp"
  "test_photo_block_relation_resource.cpp"
  )

hslib_add_utest(hs_3d_reconstructor_database SOURCES ${DATABASE_UTEST_SOURCES})
yong_add_dependence(hs_3d_reconstructor hs_3d_reconstructor_database_utest
  DEPENDENT_LOCAL_LIBS hs_3d_reconstructor_database
  DEPENDENT_PROJECT boost
    DEPENDENT_LIBS boost_system boost_filesystem
  DEPENDENT_PROJECT sqlite
    DEPENDENT_LIBS sqlite
  )
hslib_add_mkl_dep(hs_3d_reconstructor_database_utest)
hslib_add_utest_end(hs_3d_reconstructor_database)

#include "database/database_mediator.hpp"

namespace hs
{
namespace recon
{
namespace db
{

DatabaseMediator::DatabaseMediator()
  : block_resource_(nullptr)
  , photogroup_resource_(nullptr)
  , photo_resource_(nullptr)
  , ground_control_point_resource_(nullptr)
  , feature_match_resource_(nullptr)
  , photo_orientation_resource_(nullptr)
  , point_cloud_resource_(nullptr)
  , surface_model_resource_(nullptr)
  , texture_resource_(nullptr)
  , photo_block_relation_resource_(nullptr)
  , photo_measure_resource_(nullptr)
{
}

DatabaseMediator::~DatabaseMediator()
{
  Close();
}

int DatabaseMediator::RegisterObserver(DatabaseObserver* observer)
{
  auto itr_observer = observers_.find(observer);
  if (itr_observer == observers_.end())
  {
    observers_.insert(observer);
    return 0;
  }
  else
  {
    return -1;
  }
}

int DatabaseMediator::Open(const std::string& database_file)
{
  int result = Close();
  if (result != NO_ERROR)
  {
    return result;
  }
  result = database_.Open(database_file);
  if (result != NO_ERROR)
  {
    return result;
  }

  return RegisterResources();
}

int DatabaseMediator::Create(const std::string& database_directory)
{
  int result = Close();
  if (result != NO_ERROR)
  {
    return result;
  }
  result = database_.Create(database_directory);
  if (result != 0)
  {
    return result;
  }

  return RegisterResources();
}

int DatabaseMediator::Close()
{
  int result = database_.Close();

#ifdef MEDIATOR_RELEASE_RESOURCE
#undef MEDIATOR_RELEASE_RESOURCE
#endif
#define MEDIATOR_RELEASE_RESOURCE(resource) \
  if (resource)\
  {\
    delete resource;\
    resource = nullptr;\
  }\

  MEDIATOR_RELEASE_RESOURCE(block_resource_)
  MEDIATOR_RELEASE_RESOURCE(photogroup_resource_)
  MEDIATOR_RELEASE_RESOURCE(photo_resource_)
  MEDIATOR_RELEASE_RESOURCE(ground_control_point_resource_)
  MEDIATOR_RELEASE_RESOURCE(feature_match_resource_)
  MEDIATOR_RELEASE_RESOURCE(photo_orientation_resource_)
  MEDIATOR_RELEASE_RESOURCE(point_cloud_resource_)
  MEDIATOR_RELEASE_RESOURCE(surface_model_resource_)
  MEDIATOR_RELEASE_RESOURCE(texture_resource_)
  MEDIATOR_RELEASE_RESOURCE(photo_block_relation_resource_)
  MEDIATOR_RELEASE_RESOURCE(photo_measure_resource_)

  return result;
}

int DatabaseMediator::RegisterResources()
{
#ifdef MEDIATOR_REGISTER_RESOURCE
#undef MEDIATOR_REGISTER_RESOURCE
#endif
#define MEDIATOR_REGISTER_RESOURCE(ResourceType, resource) \
  resource = ResourceType::RegisterToDatabase(database_);\
  if (resource == nullptr)\
  {\
    result = ERROR_FAIL_TO_REGISTER_RESOURCE;\
    break;\
  }\

  int result = NO_ERROR;
  while (1)
  {
    MEDIATOR_REGISTER_RESOURCE(BlockResource,
                               block_resource_)
    MEDIATOR_REGISTER_RESOURCE(PhotogroupResource,
                               photogroup_resource_)
    MEDIATOR_REGISTER_RESOURCE(PhotoResource,
                               photo_resource_)
    MEDIATOR_REGISTER_RESOURCE(GroundControlPointResource,
                               ground_control_point_resource_)
    MEDIATOR_REGISTER_RESOURCE(FeatureMatchResource,
                               feature_match_resource_)
    MEDIATOR_REGISTER_RESOURCE(PhotoOrientationResource,
                               photo_orientation_resource_)
    MEDIATOR_REGISTER_RESOURCE(PointCloudResource,
                               point_cloud_resource_)
    MEDIATOR_REGISTER_RESOURCE(SurfaceModelResource,
                               surface_model_resource_)
    MEDIATOR_REGISTER_RESOURCE(TextureResource,
                               texture_resource_)
    MEDIATOR_REGISTER_RESOURCE(PhotoBlockRelationResource,
                               photo_block_relation_resource_)
    MEDIATOR_REGISTER_RESOURCE(PhotoMeasureResource,
                               photo_measure_resource_)
    break;
  }
  return result;
}

}
}
}

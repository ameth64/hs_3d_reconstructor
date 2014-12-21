#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_MEDIATOR_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_MEDIATOR_HPP_

#include <set>

#include "database/database.hpp"
#include "database/block_resource.hpp"
#include "database/photogroup_resource.hpp"
#include "database/photo_resource.hpp"
#include "database/ground_control_point_resource.hpp"
#include "database/photo_measure_resource.hpp"
#include "database/feature_match_resource.hpp"
#include "database/photo_orientation_resource.hpp"
#include "database/point_cloud_resource.hpp"
#include "database/surface_model_resource.hpp"
#include "database/texture_resource.hpp"
#include "database/photo_block_relation_resource.hpp"
#include "database/gcp_photo_measure_relation_resource.hpp"

namespace hs
{
namespace recon
{
namespace db
{

template <typename _RequestType, typename _ResponseType>
class DatabaseRequestHandler;

/**
 *  观察者模式中的观察者类。
 */
class DatabaseObserver
{
public:
  virtual void Response(int request_flag, void* response) = 0;
};


class DatabaseMediator
{
public:
  enum ErrorCode
  {
    NO_ERROR = 0,
    ERROR_FAIL_TO_REGISTER_RESOURCE = Database::NUMBER_OF_ERROR_CODE
  };

  enum RequestFlag
  {
    REQUEST_CREATE_DATABASE = 0,
    REQUEST_OPEN_DATABASE,
    REQUEST_CLOSE_DATABASE,
    REQUEST_ADD_PHOTOGROUP,
    NUMBER_OF_REQUEST_FLAGS
  };
public:
  DatabaseMediator();
  ~DatabaseMediator();

  template <typename _RequestType, typename _ResponseType>
  friend class DatabaseRequestHandler;

  template <typename _RequestType, typename _ResponseType>
  int Request(DatabaseObserver* sender,
              RequestFlag request_flag,
              const _RequestType& request, _ResponseType& response,
              bool broadcast = false)
  {
    typedef DatabaseRequestHandler<_RequestType, _ResponseType> Handler;
    Handler handler;
    int result = handler(request, response, *this);
    auto itr_observer = observers_.begin();
    auto itr_observer_end = observers_.end();
    for (; itr_observer != itr_observer_end; ++itr_observer)
    {
      if ((*itr_observer) != sender)
      {
        (*itr_observer)->Response(request_flag, (void*)(&response));
      }
    }
    return result;
  }

  int RegisterObserver(DatabaseObserver* observer);
private:
  int Open(const std::string& database_file);

  int Create(const std::string& database_directory);

  int Close();

  int RegisterResources();
private:
  Database database_;

  std::set<DatabaseObserver*> observers_;

  BlockResource* block_resource_;
  PhotogroupResource* photogroup_resource_;
  PhotoResource* photo_resource_;
  GroundControlPointResource* ground_control_point_resource_;
  PhotoMeasureResource* photo_measure_resource_;
  FeatureMatchResource* feature_match_resource_;
  PhotoOrientationResource* photo_orientation_resource_;
  PointCloudResource* point_cloud_resource_;
  SurfaceModelResource* surface_model_resource_;
  TextureResource* texture_resource_;
  PhotoBlockRelationResource* photo_block_relation_resource_;
  GCPPhotoMeasureRelationResource* gcp_photo_measure_relation_resource_;
};

#define REQUEST_HEADER
#define RESPONSE_HEADER int error_code;


//Request Create Database
struct RequestCreateDatabase
{
  REQUEST_HEADER
  std::string database_directory;
};

struct ResponseCreateDatabase
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestCreateDatabase, ResponseCreateDatabase>
{
  int operator () (const RequestCreateDatabase& request,
                   ResponseCreateDatabase& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.Create(request.database_directory);
    return response.error_code;
  }
};

//Request Open Database
struct RequestOpenDatabase
{
  REQUEST_HEADER
  std::string database_file;
};

struct ResponseOpenDatabase
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestOpenDatabase, ResponseOpenDatabase>
{
  int operator () (const RequestOpenDatabase& request,
                   ResponseOpenDatabase& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.Open(request.database_file);
    return response.error_code;
  }
};

//Request Close Database
struct RequestCloseDatabase
{
  REQUEST_HEADER
};

struct ResponseCloseDatabase
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestCloseDatabase, ResponseCloseDatabase>
{
  int operator () (const RequestCloseDatabase& request,
                   ResponseCloseDatabase& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.Close();
    return response.error_code;
  }
};

//Request Add Photogroup
struct RequestAddPhotogroup
{
  REQUEST_HEADER
  typedef Database::Float Float;
  std::string name;
  std::string description;
  int width;
  int height;
  Float pixel_x_size;
  Float pixel_y_size;
  Float focal_length;
  Float principal_x;
  Float principal_y;
  Float skew;
  Float radial1;
  Float radial2;
  Float radial3;
  Float decentering1;
  Float decentering2;
};

struct ResponseAddPhotogroup
{
  RESPONSE_HEADER
  Database::Identifier added_id;
};

template <>
struct DatabaseRequestHandler<RequestAddPhotogroup, ResponseAddPhotogroup>
{
  int operator () (const RequestAddPhotogroup& request,
                   ResponseAddPhotogroup& response,
                   DatabaseMediator& database_mediator)
  {
    PhotogroupResource::AddRequest add_request;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_NAME] =
      request.name;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_DESCRIPTION] =
      request.description;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_WIDTH] =
      request.width;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_HEIGHT] =
      request.height;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_X_SIZE] =
      request.pixel_x_size;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_PIXEL_Y_SIZE] =
      request.pixel_y_size;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_FOCAL_LENGTH] =
      request.focal_length;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_X] =
      request.principal_x;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_PRINCIPAL_Y] =
      request.principal_y;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_SKEW] =
      request.skew;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_RADIAL1] =
      request.radial1;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_RADIAL2] =
      request.radial2;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_RADIAL3] =
      request.radial3;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING1] =
      request.decentering1;
    add_request[PhotogroupResource::PHOTOGROUP_FIELD_DECENTERING2] =
      request.decentering2;
    PhotogroupResource::AddRequestContainer requests;
    requests.push_back(request);
    PhotogroupResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photogroup_resource_->Add(requests, added_records);
    if (!added_records.empty())
    {
      response.added_id = added_records.begin()->first;
    }

    return response.error_code;
  }
};

}
}
}

#endif

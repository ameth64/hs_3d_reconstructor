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
    REQUEST_ADD_PHOTOS,
    REQUEST_GET_ALL_PHOTOGROUPS,
    REQUEST_GET_ALL_PHOTOS,
    REQUEST_REMOVE_PHOTOS,
    REQUEST_DELETE_PHOTOGROUPS,
    REQUEST_GET_PHOTOGROUP,
    REQUEST_UPDATE_PHOTOGROUPS,
    REQUEST_GET_PHOTO,
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
    PhotogroupResource::AddRequestContainer add_requests;
    add_requests.push_back(add_request);
    PhotogroupResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photogroup_resource_->Add(add_requests, added_records);
    if (!added_records.empty())
    {
      response.added_id = added_records.begin()->first;
    }

    return response.error_code;
  }
};

//Request Add Photos
struct RequestAddPhotos
{
  REQUEST_HEADER
  typedef Database::Float Float;
  struct PhotoEntry
  {
    int photogroup_id;
    std::string path;
    Float pos_x;
    Float pos_y;
    Float pos_z;
    Float pitch;
    Float roll;
    Float heading;
  };
  typedef std::vector<PhotoEntry> PhotoEntryContainer;

  PhotoEntryContainer photos;
};

struct ResponseAddPhotos
{
  RESPONSE_HEADER
  typedef Database::Identifier Identifier;
  typedef std::map<Identifier, RequestAddPhotos::PhotoEntry>
          AddedPhotoEntryContainer;

  AddedPhotoEntryContainer added_photos;
};

template <>
struct DatabaseRequestHandler<RequestAddPhotos, ResponseAddPhotos>
{
  int operator() (const RequestAddPhotos& request,
                  ResponseAddPhotos& response,
                  DatabaseMediator& database_mediator)
  {
    //添加照片
    PhotoResource::AddRequestContainer add_requests;
    auto itr_photo_entry = request.photos.begin();
    auto itr_photo_entry_end = request.photos.end();
    for (; itr_photo_entry != itr_photo_entry_end; ++itr_photo_entry)
    {
      PhotoResource::AddRequest add_request;

      add_request[PhotoResource::PHOTO_FIELD_PHOTOGROUP_ID] =
        itr_photo_entry->photogroup_id;
      add_request[PhotoResource::PHOTO_FIELD_PATH] =
        itr_photo_entry->path;
      add_request[PhotoResource::PHOTO_FIELD_POS_X] =
        itr_photo_entry->pos_x;
      add_request[PhotoResource::PHOTO_FIELD_POS_Y] =
        itr_photo_entry->pos_y;
      add_request[PhotoResource::PHOTO_FIELD_POS_Z] =
        itr_photo_entry->pos_z;
      add_request[PhotoResource::PHOTO_FIELD_PITCH] =
        itr_photo_entry->pitch;
      add_request[PhotoResource::PHOTO_FIELD_ROLL] =
        itr_photo_entry->roll;
      add_request[PhotoResource::PHOTO_FIELD_HEADING] =
        itr_photo_entry->heading;

      add_requests.push_back(add_request);
    }
    PhotoResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photo_resource_->Add(add_requests, added_records);

    //将加入的照片数据填充至response变量中
    auto itr_added_record = added_records.begin();
    auto itr_added_record_end = added_records.end();
    for (; itr_added_record != itr_added_record_end; ++itr_added_record)
    {
      RequestAddPhotos::PhotoEntry photo_entry;
      const PhotoResource::AddedRecord& added_record =
        itr_added_record->second;
      photo_entry.photogroup_id =
        added_record[PhotoResource::PHOTO_FIELD_PHOTOGROUP_ID].ToInt();
      photo_entry.path =
        added_record[PhotoResource::PHOTO_FIELD_PATH].ToString();
      photo_entry.pos_x =
        added_record[PhotoResource::PHOTO_FIELD_POS_X].ToFloat();
      photo_entry.pos_y =
        added_record[PhotoResource::PHOTO_FIELD_POS_Y].ToFloat();
      photo_entry.pos_z =
        added_record[PhotoResource::PHOTO_FIELD_POS_Z].ToFloat();
      photo_entry.pitch =
        added_record[PhotoResource::PHOTO_FIELD_PITCH].ToFloat();
      photo_entry.roll =
        added_record[PhotoResource::PHOTO_FIELD_ROLL].ToFloat();
      photo_entry.heading =
        added_record[PhotoResource::PHOTO_FIELD_HEADING].ToFloat();
      response.added_photos[itr_added_record->first] = photo_entry;
    }
    return response.error_code;
  }
};

//Request Get All Photogroups
struct RequestGetAllPhotogroups
{
  REQUEST_HEADER
};

struct ResponseGetAllPhotogroups
{
  RESPONSE_HEADER
  PhotogroupResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetAllPhotogroups,
                              ResponseGetAllPhotogroups>
{
  int operator() (const RequestGetAllPhotogroups& request,
                  ResponseGetAllPhotogroups& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photogroup_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Get All Photos
struct RequestGetAllPhotos
{
  REQUEST_HEADER
};

struct ResponseGetAllPhotos
{
  RESPONSE_HEADER
  PhotoResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetAllPhotos,
                              ResponseGetAllPhotos>
{
  int operator() (const RequestGetAllPhotos& request,
                  ResponseGetAllPhotos& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photo_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Remove Photos
struct RequestRemovePhotos
{
  REQUEST_HEADER
  std::vector<Database::Identifier> ids;
};

struct ResponseRemovePhotos
{
  RESPONSE_HEADER
  std::vector<Database::Identifier> removed_ids;
};

template <>
struct DatabaseRequestHandler<RequestRemovePhotos,
                              ResponseRemovePhotos>
{
  int operator() (const RequestRemovePhotos& request,
                  ResponseRemovePhotos& response,
                  DatabaseMediator& database_mediator)
  {
    auto itr_id = request.ids.begin();
    auto itr_id_end = request.ids.end();
    for (; itr_id != itr_id_end; ++itr_id)
    {
      if (database_mediator.photo_resource_->Delete(
            EqualTo(PhotoResource::fields_[0], Value(int(*itr_id)))) == 0)
      {
        response.removed_ids.push_back(*itr_id);
      }
    }
    response.error_code = 0;
    return 0;
  }
};

//Request Delete Photogroups
struct RequestDeletePhotogroups
{
  REQUEST_HEADER
  std::vector<Database::Identifier> ids;
};

struct ResponseDeletePhotogroups
{
  RESPONSE_HEADER
  std::vector<Database::Identifier> deleted_ids;
};

template <>
struct DatabaseRequestHandler<RequestDeletePhotogroups,
                              ResponseDeletePhotogroups>
{
  int operator() (const RequestDeletePhotogroups& request,
                  ResponseDeletePhotogroups& response,
                  DatabaseMediator& database_mediator)
  {
    auto itr_id = request.ids.begin();
    auto itr_id_end = request.ids.end();
    for (; itr_id != itr_id_end; ++itr_id)
    {
      if (database_mediator.photogroup_resource_->Delete(
            EqualTo(PhotogroupResource::fields_[0], Value(int(*itr_id)))) == 0)
      {
        response.deleted_ids.push_back(*itr_id);
        database_mediator.photo_resource_->Delete(
          EqualTo(
            PhotoResource::fields_[PhotoResource::PHOTO_FIELD_PHOTOGROUP_ID],
            Value(int(*itr_id))));
      }
    }
    response.error_code = 0;
    return 0;
  }
};

//Request Get Photogroup
struct RequestGetPhotogroup
{
  REQUEST_HEADER
  Database::Identifier id;
};

struct ResponseGetPhotogroup
{
  RESPONSE_HEADER
  PhotogroupResource::Record record;
};

template <>
struct DatabaseRequestHandler<RequestGetPhotogroup,
                              ResponseGetPhotogroup>
{
  int operator () (const RequestGetPhotogroup& request,
                   ResponseGetPhotogroup& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photogroup_resource_->GetById(request.id,
                                                      response.record);
    return response.error_code;
  }
};

//Request Update Photogroups
struct RequestUpdatePhotogroups
{
  REQUEST_HEADER
  PhotogroupResource::UpdateRequestContainer update_requests;
};

struct ResponseUpdatePhotogroups
{
  RESPONSE_HEADER
  PhotogroupResource::UpdatedRecordContainer updated_records;
};

template <>
struct DatabaseRequestHandler<RequestUpdatePhotogroups,
                              ResponseUpdatePhotogroups>
{
  int operator() (const RequestUpdatePhotogroups& request,
                  ResponseUpdatePhotogroups& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photogroup_resource_->Update(request.update_requests,
                                                     response.updated_records);
    return response.error_code;
  }
};

//Request Get Photogroup
struct RequestGetPhoto
{
  REQUEST_HEADER
  Database::Identifier id;
};

struct ResponseGetPhoto
{
  RESPONSE_HEADER
  PhotoResource::Record record;
};

template <>
struct DatabaseRequestHandler<RequestGetPhoto,
                              ResponseGetPhoto>
{
  int operator () (const RequestGetPhoto& request,
                   ResponseGetPhoto& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photo_resource_->GetById(request.id,
                                                 response.record);
    return response.error_code;
  }
};

}
}
}

#endif

#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_MEDIATOR_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_MEDIATOR_HPP_

#include <set>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "hs_image_io/whole_io/image_io.hpp"

#include "hs_3d_reconstructor/config/hs_config.hpp"
#include "database/database.hpp"
#include "database/block_resource.hpp"
#include "database/photogroup_resource.hpp"
#include "database/photo_resource.hpp"
#include "database/ground_control_point_resource.hpp"
#include "database/feature_match_resource.hpp"
#include "database/photo_orientation_resource.hpp"
#include "database/point_cloud_resource.hpp"
#include "database/surface_model_resource.hpp"
#include "database/texture_resource.hpp"
#include "database/photo_block_relation_resource.hpp"
#include "database/photo_measure_resource.hpp"

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
class HS_EXPORT DatabaseObserver
{
public:
  virtual void Response(int request_flag, void* response) = 0;
};


class HS_EXPORT DatabaseMediator
{
public:
  enum ErrorCode
  {
    NO_ERROR = 0,
    ERROR_FAIL_TO_REGISTER_RESOURCE = Database::NUMBER_OF_ERROR_CODE,
    ERROR_FAIL_TO_CREATE_DIRECTORY
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
    REQUEST_ADD_BLOCK,
    REQUEST_ADD_PHOTOS_TO_BLOCK,
    REQUEST_ADD_FEATURE_MATCH,
    REQUEST_GET_FEATURE_MATCH,
    REQUEST_GET_PHOTOS_IN_BLOCK,
    REQUEST_ADD_PHOTO_ORIENTATION,
    REQUEST_GET_PHOTO_ORIENTATION,
    REQUEST_UPDATE_FEATURE_MATCH_FLAG,
    REQUEST_UPDATE_PHOTO_ORIENTATION_FLAG,
    REQUEST_GET_ALL_BLOCKS,
    REQUEST_GET_ALL_FEATURE_MATCHES,
    REQUEST_GET_ALL_PHOTO_ORIENTATIONS,
    REQUEST_ADD_GCPS,
    REQUEST_GET_ALL_GCPS,
    REQUEST_ADD_PHOTO_MEASURE,
    REQUEST_UPDATE_PHOTO_MEASURE_POS,
    REQUEST_GET_PHOTO_MEASURES_IN_PHOTO_ORIENTATION,
    REQUEST_UPDATE_GCP,
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
  FeatureMatchResource* feature_match_resource_;
  PhotoOrientationResource* photo_orientation_resource_;
  PointCloudResource* point_cloud_resource_;
  SurfaceModelResource* surface_model_resource_;
  TextureResource* texture_resource_;
  PhotoBlockRelationResource* photo_block_relation_resource_;
  PhotoMeasureResource* photo_measure_resource_;
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
    std::string thumbnail_path;
    Float pos_x;
    Float pos_y;
    Float pos_z;
    Float pitch;
    Float roll;
    Float heading;
    std::string coordinate_system;
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
      add_request[PhotoResource::PHOTO_FIELD_THUMBNAIL_PATH] =
        std::string("");
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
      add_request[PhotoResource::PHOTO_FIELD_COORDINATE_SYSTEM] =
        itr_photo_entry->coordinate_system;

      add_requests.push_back(add_request);
    }
    PhotoResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photo_resource_->Add(add_requests, added_records);

    //将加入的照片数据填充至response变量中，并创建缩略图
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
      photo_entry.coordinate_system =
        added_record[PhotoResource::PHOTO_FIELD_COORDINATE_SYSTEM].ToString();

      std::string thumbnail_directory =
        database_mediator.database_.ThumbnailPath();

      std::string thumbnail_path =
        boost::str(boost::format("%1%/%2%.jpg") %
                   thumbnail_directory % itr_added_record->first);
      hs::imgio::whole::ImageIO image_io;
      hs::imgio::whole::ImageData image_data;
      image_io.LoadImage(photo_entry.path, image_data);
      hs::imgio::whole::ImageData thumbnail_data(image_data.width() / 20,
                                                 image_data.height() / 20,
                                                 image_data.channel(),
                                                 image_data.color_type(),
                                                 image_data.bit_depth());
      for (int row = 0; row < thumbnail_data.height(); row++)
      {
        for (int col = 0; col < thumbnail_data.width(); col++)
        {
          for (int channel = 0; channel < thumbnail_data.channel(); channel++)
          {
            thumbnail_data.GetByte(row, col, channel) =
              image_data.GetByte(row * 20, col * 20, channel);
          }
        }
      }
      if (image_io.SaveImage(thumbnail_path, thumbnail_data) != 0)
      {
        continue;
      }

      PhotoResource::UpdateRequest update_request;
      update_request[PhotoResource::PHOTO_FIELD_THUMBNAIL_PATH] =
        thumbnail_path;
      PhotoResource::UpdateRequestContainer update_requests;
      update_requests[itr_added_record->first] = update_request;
      PhotoResource::UpdatedRecordContainer updated_records;
      int update_result =
        database_mediator.photo_resource_->Update(update_requests,
                                                  updated_records);
      if (update_result != DatabaseMediator::NO_ERROR)
      {
        continue;
      }

      photo_entry.thumbnail_path = thumbnail_path;

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

//Request Get Photo
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

//Request Add Block
struct RequestAddBlock
{
  REQUEST_HEADER
  std::string name;
  std::string description;
};

struct ResponseAddBlock
{
  RESPONSE_HEADER
  Database::Identifier added_id;
};

template <>
struct DatabaseRequestHandler<RequestAddBlock, ResponseAddBlock>
{
  int operator() (const RequestAddBlock& request,
                  ResponseAddBlock& response,
                  DatabaseMediator& database_mediator)
  {
    BlockResource::AddRequestContainer add_requests;
    BlockResource::AddRequest add_request;
    add_request[BlockResource::BLOCK_FIELD_NAME] = request.name;
    add_request[BlockResource::BLOCK_FIELD_DESCRIPTION] = request.description;
    add_requests.push_back(add_request);
    BlockResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.block_resource_->Add(add_requests, added_records);
    if (!added_records.empty())
    {
      response.added_id = added_records.begin()->first;
    }
    return response.error_code;
  }
};

//Request Add Photos to Block
struct RequestAddPhotosToBlock
{
  REQUEST_HEADER
  Identifier block_id;
  std::vector<Identifier> photo_ids;
};

struct ResponseAddPhotosToBlock
{
  RESPONSE_HEADER
  Identifier block_id;
  std::vector<Identifier> photo_ids;
};

template <>
struct DatabaseRequestHandler<RequestAddPhotosToBlock, ResponseAddPhotosToBlock>
{
  int operator() (const RequestAddPhotosToBlock& request,
                  ResponseAddPhotosToBlock& response,
                  DatabaseMediator& database_mediator)
  {
    PhotoBlockRelationResource::AddRequestContainer add_requests;
    auto itr_photo_id = request.photo_ids.begin();
    auto itr_photo_id_end = request.photo_ids.end();
    for (; itr_photo_id != itr_photo_id_end; ++itr_photo_id)
    {
      //TODO: 需加入照片或块是否存在的检查，最好应由外键约束完成。
      PhotoBlockRelationResource::AddRequest add_request;
      add_request[PhotoBlockRelationResource::PBR_FIELD_BLOCK_ID] =
        int(request.block_id);
      add_request[PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID] =
        int(*itr_photo_id);
      BlockResource::Record record_block;
      PhotoResource::Record record_photo;
      PhotoBlockRelationResource::SelectMask select_mask;
      select_mask.set(PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID);
      select_mask.set(PhotoBlockRelationResource::PBR_FIELD_BLOCK_ID);
      PhotoBlockRelationResource::SelectedRecordContainer selected_records;
      database_mediator.photo_block_relation_resource_->Select(
        select_mask,
        And(EqualTo(database_mediator.photo_block_relation_resource_->fields_[
                      PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID],
                    Value(int(*itr_photo_id))),
            EqualTo(database_mediator.photo_block_relation_resource_->fields_[
                      PhotoBlockRelationResource::PBR_FIELD_BLOCK_ID],
                    Value(int(request.block_id)))),
        selected_records);
      if (database_mediator.block_resource_->GetById(request.block_id,
                                                     record_block) == 0 &&
          database_mediator.photo_resource_->GetById(*itr_photo_id,
                                                     record_photo) == 0 &&
          selected_records.empty())
      {
        add_requests.push_back(add_request);
      }
    }
    PhotoBlockRelationResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photo_block_relation_resource_->Add(add_requests,
                                                            added_records);
    if (response.error_code == DatabaseMediator::NO_ERROR)
    {
      response.block_id = request.block_id;
      auto itr_added_record = added_records.begin();
      auto itr_added_record_end = added_records.end();
      response.photo_ids.clear();
      for (; itr_added_record != itr_added_record_end; ++itr_added_record)
      {
        const PhotoBlockRelationResource::AddedRecord added_record =
          itr_added_record->second;
        int photo_id =
          added_record[PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID].ToInt();
        response.photo_ids.push_back(Database::Identifier(photo_id));
      }
    }
    return response.error_code;
  }
};

//Request Add Feature Match
struct RequestAddFeatureMatch
{
  REQUEST_HEADER
  typedef Database::Identifier Identifier;

  Identifier block_id;
};

struct ResponseAddFeatureMatch
{
  RESPONSE_HEADER
  typedef Database::Identifier Identifier;
  Identifier feature_match_id;
  Identifier block_id;
  std::string name;
  std::string path;
};

template<>
struct DatabaseRequestHandler<RequestAddFeatureMatch, ResponseAddFeatureMatch>
{
  int operator () (const RequestAddFeatureMatch& request,
                   ResponseAddFeatureMatch& response,
                   DatabaseMediator& database_mediator)
  {
    typedef Database::Identifier Identifier;
    FeatureMatchResource::AddRequest add_request;
    add_request[FeatureMatchResource::FEATURE_MATCH_FIELD_BLOCK_ID] =
      int(request.block_id);
    add_request[FeatureMatchResource::FEATURE_MATCH_FIELD_NAME] =
      std::string("");
    add_request[FeatureMatchResource::FEATURE_MATCH_FIELD_PATH] =
      std::string("");
    add_request[FeatureMatchResource::FEATURE_MATCH_FIELD_FLAG] =
      int(FeatureMatchResource::FLAG_NOT_COMPLETED);
    FeatureMatchResource::AddRequestContainer add_requests;
    add_requests.push_back(add_request);
    FeatureMatchResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.feature_match_resource_->Add(add_requests,
                                                     added_records);
    if (response.error_code != DatabaseMediator::NO_ERROR ||
        added_records.empty())
    {
      return response.error_code;
    }

    Identifier feature_match_id = added_records.begin()->first;
    std::string feature_match_name =
      boost::str(boost::format("feature_match_%1%") % feature_match_id);
    boost::filesystem::path feature_match_path =
      boost::str(boost::format("%1%/%2%/") %
                 database_mediator.database_.FeatureMatchPath() %
                 feature_match_id);
    if (!boost::filesystem::create_directory(feature_match_path))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
      return response.error_code;
    }

    FeatureMatchResource::UpdateRequest update_request;
    update_request[FeatureMatchResource::FEATURE_MATCH_FIELD_NAME] =
      feature_match_name;
    update_request[FeatureMatchResource::FEATURE_MATCH_FIELD_PATH] =
      feature_match_path.string();
    FeatureMatchResource::UpdateRequestContainer update_requests;
    update_requests[feature_match_id] = update_request;
    FeatureMatchResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.feature_match_resource_->Update(update_requests,
                                                        updated_records);

    if (response.error_code == DatabaseMediator::NO_ERROR)
    {
      response.feature_match_id = feature_match_id;
      response.block_id = request.block_id;
      response.name = feature_match_name;
      response.path = feature_match_path.string();
    }

    return response.error_code;
  }
};

//Request Get Feature Match
struct RequestGetFeatureMatch
{
  REQUEST_HEADER
  Database::Identifier id;
};

struct ResponseGetFeatureMatch
{
  RESPONSE_HEADER
  FeatureMatchResource::Record record;
};

template <>
struct DatabaseRequestHandler<RequestGetFeatureMatch, ResponseGetFeatureMatch>
{
  int operator () (const RequestGetFeatureMatch& request,
                   ResponseGetFeatureMatch& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.feature_match_resource_->GetById(request.id,
                                                         response.record);
    return response.error_code;
  }
};

//Get Photos In Block
struct RequestGetPhotosInBlock
{
  REQUEST_HEADER
  Database::Identifier block_id;
};

struct ResponseGetPhotosInBlock
{
  RESPONSE_HEADER
  PhotoResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetPhotosInBlock, ResponseGetPhotosInBlock>
{
  int operator() (const RequestGetPhotosInBlock& request,
                  ResponseGetPhotosInBlock& response,
                  DatabaseMediator& database_mediator)
  {
    typedef Database::Identifier Identifier;
    PhotoBlockRelationResource::SelectMask select_mask;
    select_mask.set(PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID);
    PhotoBlockRelationResource::SelectedRecordContainer selected_records;
    response.error_code =
      database_mediator.photo_block_relation_resource_->Select(
        select_mask,
        EqualTo(database_mediator.photo_block_relation_resource_->fields_[
                  PhotoBlockRelationResource::PBR_FIELD_BLOCK_ID],
                Value(int(request.block_id))),
        selected_records);
    auto itr_selected_record = selected_records.begin();
    auto itr_selected_record_end = selected_records.end();
    for (; itr_selected_record != itr_selected_record_end;
         ++itr_selected_record)
    {
      int photo_id =
        itr_selected_record->second[
          PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID].ToInt();
      PhotoResource::Record photo_record;
      response.error_code =
        database_mediator.photo_resource_->GetById(photo_id, photo_record);
      if (response.error_code != Database::NO_ERROR)
      {
        break;
      }
      response.records[Identifier(photo_id)] = photo_record;
    }
    return response.error_code;
  }
};

//Request Add Photo Orientation
struct RequestAddPhotoOrientation
{
  REQUEST_HEADER
  typedef Database::Identifier Identifier;
  Identifier feature_match_id;
};

struct ResponseAddPhotoOrientation
{
  RESPONSE_HEADER
  typedef Database::Identifier Identifier;
  Identifier photo_orientation_id;
  Identifier feature_match_id;
  std::string name;
  std::string path;
};

template <>
struct DatabaseRequestHandler<RequestAddPhotoOrientation,
                              ResponseAddPhotoOrientation>
{
  int operator() (const RequestAddPhotoOrientation& request,
                  ResponseAddPhotoOrientation& response,
                  DatabaseMediator& database_mediator)
  {
    typedef Database::Identifier Identifier;
    PhotoOrientationResource::AddRequest add_request;
    add_request[
      PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FEATURE_MATCH_ID] =
      int(request.feature_match_id);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_NAME] =
      std::string("");
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_PATH] =
      std::string("");
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FLAG] =
      int(PhotoOrientationResource::FLAG_NOT_COMPLETED);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_SCALE] =
      Float(1);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_ROTATION_X] =
      Float(0);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_ROTATION_Y] =
      Float(0);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_ROTATION_Z] =
      Float(0);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_TRANSLATE_X] =
      Float(0);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_TRANSLATE_Y] =
      Float(0);
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_TRANSLATE_Z] =
      Float(0);
    PhotoOrientationResource::AddRequestContainer add_requests;
    add_requests.push_back(add_request);
    PhotoOrientationResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photo_orientation_resource_->Add(add_requests,
                                                         added_records);
    if (response.error_code != DatabaseMediator::NO_ERROR ||
        added_records.empty())
    {
      return response.error_code;
    }

    Identifier photo_orientation_id = added_records.begin()->first;
    std::string photo_orientation_name =
      boost::str(boost::format("photo_orientation_%1%") % photo_orientation_id);
    boost::filesystem::path photo_orientation_path =
      boost::str(boost::format("%1%/%2%/") %
                 database_mediator.database_.PhotoOrientationPath() %
                 photo_orientation_id);
    if (!boost::filesystem::create_directory(photo_orientation_path))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
      return response.error_code;
    }

    PhotoOrientationResource::UpdateRequest update_request;
    update_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_NAME] =
      photo_orientation_name;
    update_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_PATH] =
      photo_orientation_path.string();
    PhotoOrientationResource::UpdateRequestContainer update_requests;
    update_requests[photo_orientation_id] = update_request;
    PhotoOrientationResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.photo_orientation_resource_->Update(update_requests,
                                                            updated_records);

    if (response.error_code == DatabaseMediator::NO_ERROR)
    {
      response.photo_orientation_id = photo_orientation_id;;
      response.feature_match_id = request.feature_match_id;
      response.name = photo_orientation_name;
      response.path = photo_orientation_path.string();
    }

    return response.error_code;
  }
};

//Reequest Get Photo Orientation
struct RequestGetPhotoOrientation
{
  REQUEST_HEADER
  Database::Identifier id;
};

struct ResponseGetPhotoOrientation
{
  RESPONSE_HEADER
  PhotoOrientationResource::Record record;
  std::string intrinsic_path;
  std::string extrinsic_path;
  std::string point_cloud_path;
};

template <>
struct DatabaseRequestHandler<RequestGetPhotoOrientation,
                              ResponseGetPhotoOrientation>
{
  int operator () (const RequestGetPhotoOrientation& request,
                   ResponseGetPhotoOrientation& response,
                   DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photo_orientation_resource_->GetById(request.id,
                                                             response.record);

    std::string photo_orientation_path =
      response.record[
        PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_PATH].ToString();

    response.intrinsic_path =
      photo_orientation_path + "intrinsic.txt";
    response.extrinsic_path =
      photo_orientation_path + "extrinsic.txt";
    response.point_cloud_path =
      photo_orientation_path + "sparse_point_cloud.txt";

    return response.error_code;
  }
};

//Request Update Feature Match Flag
struct RequestUpdateFeatureMatchFlag
{
  REQUEST_HEADER
  Database::Identifier id;
  int flag;
};

struct ResponseUpdateFeatureMatchFlag
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdateFeatureMatchFlag,
                              ResponseUpdateFeatureMatchFlag>
{
  int operator() (const RequestUpdateFeatureMatchFlag& request,
                  ResponseUpdateFeatureMatchFlag& response,
                  DatabaseMediator& database_mediator)
  {
    FeatureMatchResource::UpdateRequest update_request;
    update_request[FeatureMatchResource::FEATURE_MATCH_FIELD_FLAG] =
      request.flag;
    FeatureMatchResource::UpdateRequestContainer update_requests;
    update_requests[request.id] = update_request;
    FeatureMatchResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.feature_match_resource_->Update(update_requests,
                                                        updated_records);
    return response.error_code;
  }
};

//Request Update Photo Orientation Flag
struct RequestUpdatePhotoOrientationFlag
{
  REQUEST_HEADER
  Database::Identifier id;
  int flag;
};

struct ResponseUpdatePhotoOrientationFlag
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdatePhotoOrientationFlag,
                              ResponseUpdatePhotoOrientationFlag>
{
  int operator() (const RequestUpdatePhotoOrientationFlag& request,
                  ResponseUpdatePhotoOrientationFlag& response,
                  DatabaseMediator& database_mediator)
  {
    PhotoOrientationResource::UpdateRequest update_request;
    update_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FLAG] =
      request.flag;
    PhotoOrientationResource::UpdateRequestContainer update_requests;
    update_requests[request.id] = update_request;
    PhotoOrientationResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.photo_orientation_resource_->Update(update_requests,
                                                            updated_records);
    return response.error_code;
  }
};

//Request Get All Blocks
struct RequestGetAllBlocks
{
  REQUEST_HEADER
};

struct ResponseGetAllBlocks
{
  RESPONSE_HEADER
  BlockResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetAllBlocks, ResponseGetAllBlocks>
{
  int operator() (const RequestGetAllBlocks& request,
                  ResponseGetAllBlocks& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.block_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Get All Feature Matches
struct RequestGetAllFeatureMatches
{
  REQUEST_HEADER
};

struct ResponseGetAllFeatureMatches
{
  RESPONSE_HEADER
  FeatureMatchResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetAllFeatureMatches,
                              ResponseGetAllFeatureMatches>
{
  int operator() (const RequestGetAllFeatureMatches& request,
                  ResponseGetAllFeatureMatches& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.feature_match_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Get All Photo Orientations
struct RequestGetAllPhotoOrientations
{
  REQUEST_HEADER
};

struct ResponseGetAllPhotoOrientations
{
  RESPONSE_HEADER
  PhotoOrientationResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetAllPhotoOrientations,
                              ResponseGetAllPhotoOrientations>
{
  int operator() (const RequestGetAllPhotoOrientations& request,
                  ResponseGetAllPhotoOrientations& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.photo_orientation_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Add GCPs
struct RequestAddGCPs
{
  REQUEST_HEADER
  typedef Database::Float Float;
  struct GCPEntry
  {
    std::string name;
    std::string description;
    Float x;
    Float y;
    Float z;
  };
  typedef std::vector<GCPEntry> GCPEntryContainer;

  GCPEntryContainer gcps;
};

struct ResponseAddGCPs
{
  RESPONSE_HEADER
  //RequestAddGCPs::GCPEntryContainer gcps_not_added;
  GroundControlPointResource::AddedRecordContainer added_records;
};

template <>
struct DatabaseRequestHandler<RequestAddGCPs, ResponseAddGCPs>
{
  int operator() (const RequestAddGCPs& request,
                  ResponseAddGCPs& response,
                  DatabaseMediator& database_mediator)
  {
    GroundControlPointResource::AddRequestContainer add_requests;
    auto itr_gcp_entry = request.gcps.begin();
    auto itr_gcp_entry_end = request.gcps.end();
    for (; itr_gcp_entry != itr_gcp_entry_end; ++itr_gcp_entry)
    {
      GroundControlPointResource::AddRequest add_request;
      add_request[GroundControlPointResource::GCP_FIELD_NAME] =
        itr_gcp_entry->name;
      add_request[GroundControlPointResource::GCP_FIELD_DESCRIPTION] =
        itr_gcp_entry->description;
      add_request[GroundControlPointResource::GCP_FIELD_X] =
        itr_gcp_entry->x;
      add_request[GroundControlPointResource::GCP_FIELD_Y] =
        itr_gcp_entry->y;
      add_request[GroundControlPointResource::GCP_FIELD_Z] =
        itr_gcp_entry->z;

      add_requests.push_back(add_request);
    }

    response.error_code =
      database_mediator.ground_control_point_resource_->Add(
        add_requests, response.added_records);

    //itr_gcp_entry = request.gcps.begin();
    //itr_gcp_entry_end = request.gcps.end();
    //for (; itr_gcp_entry != itr_gcp_entry_end; ++itr_gcp_entry)
    //{
    //  auto itr_added_record = response.added_records.begin();
    //  auto itr_added_record_end = response.added_records.end();
    //  bool found = false;
    //  for (; itr_added_record != itr_added_record_end; ++itr_added_record)
    //  {
    //    if (itr_gcp_entry->name ==
    //        itr_added_record->second[
    //          GroundControlPointResource::GCP_FIELD_NAME].ToString() &&
    //        itr_gcp_entry->description ==
    //        itr_added_record->second[
    //          GroundControlPointResource::GCP_FIELD_DESCRIPTION].ToString() &&
    //        itr_gcp_entry->x ==
    //        itr_added_record->second[
    //          GroundControlPointResource::GCP_FIELD_X].ToFloat() &&
    //        itr_gcp_entry->y ==
    //        itr_added_record->second[
    //          GroundControlPointResource::GCP_FIELD_Y].ToFloat() &&
    //        itr_gcp_entry->z ==
    //        itr_added_record->second[
    //          GroundControlPointResource::GCP_FIELD_Z].ToFloat())
    //    {
    //      found = true;
    //      break;
    //    }
    //  }
    //  if (!found)
    //  {
    //    response.gcps_not_added.push_back(*itr_gcp_entry);
    //  }
    //}

    return response.error_code;
  }
};

//Request Get All GCPS
struct RequestGetAllGCPs
{
  REQUEST_HEADER
};

struct ResponseGetAllGCPs
{
  RESPONSE_HEADER
  GroundControlPointResource::RecordContainer records;
};

template <>
struct DatabaseRequestHandler<RequestGetAllGCPs, ResponseGetAllGCPs>
{
  int operator() (const RequestGetAllGCPs& request,
                  ResponseGetAllGCPs& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.ground_control_point_resource_->GetAll(
                                                          response.records);
    return response.error_code;
  }
};

//Request Add Photo Measure
struct RequestAddPhotoMeasure
{
  REQUEST_HEADER
  Identifier photo_orientation_id;
  Identifier gcp_id;
  Identifier photo_id;
  int gcp_type;
  Float x;
  Float y;
};

struct ResponseAddPhotoMeasure
{
  RESPONSE_HEADER
  Identifier photo_measure_id;
};

template <>
struct DatabaseRequestHandler<RequestAddPhotoMeasure, ResponseAddPhotoMeasure>
{
  int operator() (const RequestAddPhotoMeasure& request,
                  ResponseAddPhotoMeasure& response,
                  DatabaseMediator& database_mediator)
  {
    //TODO:需加入照片或地面控制点或照片定向是否存在的检查，最好应由外键约束完成
    PhotoMeasureResource::AddRequestContainer add_requests;
    PhotoMeasureResource::AddRequest add_request;
    add_request[
      PhotoMeasureResource::PHOTO_MEASURE_FIELD_PHOTO_ORIENTATION_ID] =
        int(request.photo_orientation_id);
    add_request[
      PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_ID] =
        int(request.gcp_id);
    add_request[
      PhotoMeasureResource::PHOTO_MEASURE_FIELD_PHOTO_ID] =
        int (request.photo_id);
    add_request[
      PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_TYPE] =
        request.gcp_type;
    add_request[
      PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_X] =
        request.x;
    add_request[
      PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_Y] =
        request.y;
    add_requests.push_back(add_request);
    PhotoMeasureResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photo_measure_resource_->Add(add_requests,
                                                     added_records);
    if (response.error_code == DatabaseMediator::NO_ERROR)
    {
      if (added_records.size() == 1)
      {
        response.photo_measure_id = added_records.begin()->first;
      }
      else
      {
        response.error_code = -1;
      }
    }

    return response.error_code;
  }
};

//Request Update Photo Measure
struct RequestUpdatePhotoMeasurePos
{
  REQUEST_HEADER
  Identifier photo_measure_id;
  Float x;
  Float y;
};

struct ResponseUpdatePhotoMeasurePos
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdatePhotoMeasurePos,
                              ResponseUpdatePhotoMeasurePos>
{
  int operator() (const RequestUpdatePhotoMeasurePos& request,
                  ResponseUpdatePhotoMeasurePos& response,
                  DatabaseMediator& database_mediator)
  {
    PhotoMeasureResource::UpdateRequestContainer update_requests;
    PhotoMeasureResource::UpdateRequest update_request;
    update_request[PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_X] =
      request.x;
    update_request[PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_Y] =
      request.y;
    update_requests[request.photo_measure_id] = update_request;

    PhotoMeasureResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.photo_measure_resource_->Update(update_requests,
                                                        updated_records);

    return response.error_code;
  }
};

//Request Get Photo Measures In Photo Orientation
struct RequestGetPhotoMeasuresInPhotoOrientation
{
  REQUEST_HEADER
  Identifier photo_orientation_id;
};

struct ResponseGetPhotoMeasuresInPhotoOrientation
{
  RESPONSE_HEADER
  struct PhotoMeasure
  {
    Identifier photo_measure_id;
    Identifier photo_id;
    Float x;
    Float y;
  };
  struct GCPMeasure
  {
    Identifier gcp_id;
    int type;
    std::map<Identifier, PhotoMeasure> photo_measures;
  };
  typedef std::map<Identifier, GCPMeasure> GCPMeasureContainer;

  GCPMeasureContainer gcp_measures;
};

template <>
struct DatabaseRequestHandler<RequestGetPhotoMeasuresInPhotoOrientation,
                              ResponseGetPhotoMeasuresInPhotoOrientation>
{
  int operator() (const RequestGetPhotoMeasuresInPhotoOrientation& request,
                  ResponseGetPhotoMeasuresInPhotoOrientation& response,
                  DatabaseMediator& database_mediator)
  {
    PhotoMeasureResource::SelectMask select_mask;
    select_mask.set(PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_ID);
    select_mask.set(PhotoMeasureResource::PHOTO_MEASURE_FIELD_PHOTO_ID);
    select_mask.set(PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_TYPE);
    select_mask.set(PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_X);
    select_mask.set(PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_Y);
    PhotoMeasureResource::SelectedRecordContainer selected_records;
    response.error_code =
      database_mediator.photo_measure_resource_->Select(
        select_mask,
        EqualTo(database_mediator.photo_measure_resource_->fields_[
                  PhotoMeasureResource::
                  PHOTO_MEASURE_FIELD_PHOTO_ORIENTATION_ID],
                Value(int(request.photo_orientation_id))),
        selected_records);
    if (response.error_code == Database::NO_ERROR)
    {
      auto itr_selected_record = selected_records.begin();
      auto itr_selected_record_end = selected_records.end();
      for (; itr_selected_record != itr_selected_record_end;
           ++itr_selected_record)
      {
        int gcp_id =
          itr_selected_record->second[
            PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_ID].ToInt();
        int photo_id =
          itr_selected_record->second[
            PhotoMeasureResource::PHOTO_MEASURE_FIELD_PHOTO_ID].ToInt();
        int gcp_type =
          itr_selected_record->second[
            PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_TYPE].ToInt();
        Float pos_x =
          itr_selected_record->second[
            PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_X].ToFloat();
        Float pos_y =
          itr_selected_record->second[
            PhotoMeasureResource::PHOTO_MEASURE_FIELD_POS_Y].ToFloat();
        int result = -1;
        while (1)
        {
          auto itr_gcp_measure = response.gcp_measures.find(Identifier(gcp_id));
          if (itr_gcp_measure == response.gcp_measures.end())
          {
            ResponseGetPhotoMeasuresInPhotoOrientation::GCPMeasure gcp_measure;
            gcp_measure.gcp_id = Identifier(gcp_id);
            gcp_measure.type = gcp_type;
            response.gcp_measures[Identifier(gcp_id)] = gcp_measure;
            itr_gcp_measure = response.gcp_measures.find(Identifier(gcp_id));
          }

          if (itr_gcp_measure->second.type != gcp_type)
          {
            break;
          }

          auto itr_photo_measure =
            itr_gcp_measure->second.photo_measures.find(Identifier(photo_id));
          if (itr_photo_measure == itr_gcp_measure->second.photo_measures.end())
          {
            ResponseGetPhotoMeasuresInPhotoOrientation::PhotoMeasure
              photo_measure;
            photo_measure.photo_measure_id = itr_selected_record->first;
            photo_measure.photo_id = Identifier(photo_id);
            photo_measure.x = pos_x;
            photo_measure.y = pos_y;
            itr_gcp_measure->second.photo_measures[Identifier(photo_id)] =
              photo_measure;
          }
          else
          {
            break;
          }

          result = DatabaseMediator::NO_ERROR;
          break;
        }
        if (result != DatabaseMediator::NO_ERROR)
        {
          response.error_code = result;
          break;
        }
      }
    }
    return response.error_code;
  }
};

//Request Update GCP
struct RequestUpdateGCP
{
  REQUEST_HEADER
  Identifier id;
  Identifier photo_orientation_id;
  std::string name;
  std::string description;
  Float x;
  Float y;
  Float z;
  int type;
  bool update_photo_measure;
};

//Response Update GCP
struct ResponseUpdateGCP
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdateGCP, ResponseUpdateGCP>
{
  int operator() (const RequestUpdateGCP& request,
                  ResponseUpdateGCP& response,
                  DatabaseMediator& database_mediator)
  {
    GroundControlPointResource::UpdateRequestContainer update_requests;
    GroundControlPointResource::UpdateRequest update_request;
    update_request[GroundControlPointResource::GCP_FIELD_NAME] =
      request.name;
    update_request[GroundControlPointResource::GCP_FIELD_DESCRIPTION] =
      request.description;
    update_request[GroundControlPointResource::GCP_FIELD_X] = request.x;
    update_request[GroundControlPointResource::GCP_FIELD_Y] = request.y;
    update_request[GroundControlPointResource::GCP_FIELD_Z] = request.z;
    update_requests[request.id] = update_request;
    GroundControlPointResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.ground_control_point_resource_->Update(
        update_requests, updated_records);
    if (response.error_code == DatabaseMediator::NO_ERROR &&
        request.update_photo_measure)
    {
      PhotoMeasureResource::SelectMask select_mask;
      select_mask.set(PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_TYPE);
      PhotoMeasureResource::SelectedRecordContainer selected_records;
      response.error_code =
        database_mediator.photo_measure_resource_->Select(
          select_mask,
          And(EqualTo(database_mediator.photo_measure_resource_->fields_[
                        PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_ID],
                      Value(int(request.id))),
              EqualTo(database_mediator.photo_measure_resource_->fields_[
                        PhotoMeasureResource::
                        PHOTO_MEASURE_FIELD_PHOTO_ORIENTATION_ID],
                      Value(int(request.photo_orientation_id)))),
          selected_records);
      if (response.error_code == Database::NO_ERROR)
      {
        auto itr_selected_record = selected_records.begin();
        auto itr_selected_record_end = selected_records.end();
        for (; itr_selected_record != itr_selected_record_end;
             ++itr_selected_record)
        {
          itr_selected_record->second[
            PhotoMeasureResource::PHOTO_MEASURE_FIELD_GCP_TYPE] = request.type;
        }
        PhotoMeasureResource::UpdatedRecordContainer
          updated_records_photo_measure;
        response.error_code =
          database_mediator.photo_measure_resource_->Update(
            selected_records, updated_records_photo_measure);
      }
    }
    return response.error_code;
  }
};

}
}
}

#endif

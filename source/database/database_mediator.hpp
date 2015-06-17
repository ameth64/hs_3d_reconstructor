#ifndef _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_MEDIATOR_HPP_
#define _HS_3D_RECONSTRUCTOR_DATABASE_DATABASE_MEDIATOR_HPP_

#include <set>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <cereal/archives/portable_binary.hpp>

#include "hs_math/geometry/rotation.hpp"
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
#include "hs_progress/progress_utility/progress_manager.hpp"

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
  typedef Database::Identifier Identifier;
  enum ErrorCode
  {
    NO_ERROR = 0,
    ERROR_FAIL_TO_REGISTER_RESOURCE = Database::NUMBER_OF_ERROR_CODE,
    ERROR_FAIL_TO_CREATE_DIRECTORY,
    ERROR_FAIL_TO_COPY_DIRECTORY,
    ERROR_FAIL_TO_REMOVE_DIRECTORY
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
    REQUEST_ADD_POINT_CLOUD,
    REQUEST_GET_POINT_CLOUD,
    REQUEST_UPDATE_PHOTO_PATH,
    REQUEST_UPDATE_FEATURE_MATCH_FLAG,
    REQUEST_UPDATE_PHOTO_ORIENTATION_FLAG,
    REQUEST_UPDATE_PHOTO_ORIENTATION_TRANSFORM,
    REQUEST_UPDATE_PHOTO_ORIENTATION_COORDINATE_SYSTEM,
    REQUEST_UPDATE_POINT_CLOUD_FLAG,
    REQUEST_GET_ALL_BLOCKS,
    REQUEST_GET_ALL_FEATURE_MATCHES,
    REQUEST_GET_ALL_PHOTO_ORIENTATIONS,
    REQUEST_GET_ALL_POINT_CLOUDS,
    REQUEST_ADD_GCPS,
    REQUEST_GET_ALL_GCPS,
    REQUEST_ADD_PHOTO_MEASURE,
    REQUEST_UPDATE_PHOTO_MEASURE_POS,
    REQUEST_GET_PHOTO_MEASURES_IN_PHOTO_ORIENTATION,
    REQUEST_UPDATE_GCP,
    REQUEST_ADD_SURFACE_MODEL,
    REQUEST_ADD_TEXTURE,
    REQUEST_GET_SURFACE_MODEL,
    REQUEST_GET_TEXTURE,
    REQUEST_GET_ALL_SURFACE_MODELS,
    REQUEST_GET_ALL_TEXTURES,
    REQUEST_UPDATE_SURFACE_MODEL_FLAG,
    REQUEST_UPDATE_TEXTURE_FLAG,
    REQUEST_COPY_BLOCK,
    REQUEST_COPY_FEATURE_MATCH,
    REQUEST_COPY_PHOTO_ORIENTATION,
    REQUEST_COPY_POINT_CLOUD,
    REQUEST_COPY_SURFACE_MODEL,
    REQUEST_COPY_TEXTURE,
    REQUEST_REMOVE_BLOCK,
    REQUEST_REMOVE_FEATURE_MATCH,
    REQUEST_REMOVE_PHOTO_ORIENTATION,
    REQUEST_REMOVE_POINT_CLOUD,
    REQUEST_REMOVE_SURFACE_MODEL,
    REQUEST_REMOVE_TEXTURE,
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

  std::string GetThumbnailPath(Identifier photo_id) const;
  std::string GetFeatureMatchPath(Identifier feature_match_id) const;
  std::string GetPhotoOrientationPath(Identifier photo_orientation_id) const;
  std::string GetPointCloudPath(Identifier point_cloud_id) const;
  std::string GetSurfaceModelPath(Identifier surface_model_id) const;
  std::string GetTexturePath(Identifier texture_id) const;
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
  PhotoResource::RecordContainer photo_records;
  std::map<int,std::string> error_photo_id_paths;
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
    if (response.error_code != Database::NO_ERROR)
    {
      return response.error_code;
    }
    //检查all photo是否存在
    response.error_code = 
      database_mediator.photo_resource_->GetAll(response.photo_records);
    if(response.error_code != Database::NO_ERROR)
    {
      return response.error_code;
    }
    for(auto iter = response.photo_records.begin();
             iter != response.photo_records.end();
             ++iter)
    {
      std::string photo_path = iter->second[
        db::PhotoResource::PHOTO_FIELD_PATH].ToString();
      boost::filesystem::path path_file(photo_path);
      if((!boost::filesystem::exists(path_file)) ||
         (!boost::filesystem::is_regular_file(path_file)))
      {
        response.error_photo_id_paths[iter->first] = photo_path;
      }
    }
    if(!response.error_photo_id_paths.empty())
    {
      //database_mediator.Close();
      return Database::ERROR_PHOTO_PATH_NEEDED_MODIFY;
    }

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
//Request Update Photos Path
struct RequestUpdatePhotoPaths
{
  REQUEST_HEADER
  std::map<int, std::string> photo_id_paths;
};
struct ResponseUpdatePhotoPaths
{
  RESPONSE_HEADER
};
template <>
struct DatabaseRequestHandler < RequestUpdatePhotoPaths, ResponseUpdatePhotoPaths >
{
  int operator() (const RequestUpdatePhotoPaths& request,
                  ResponseUpdatePhotoPaths& response,
                  DatabaseMediator& database_mediator)
  {
    PhotoResource::UpdateRequestContainer update_requests;
    PhotoResource::UpdateRequest update_request;
    for (auto iter = request.photo_id_paths.begin();
         iter != request.photo_id_paths.end(); ++iter)
    {
      update_request[PhotoResource::PHOTO_FIELD_PATH] = 
        iter->second;
      update_requests[iter->first] = update_request;
    }
    PhotoResource::UpdatedRecordContainer updated_records;
    response.error_code =
    database_mediator.photo_resource_->Update(update_requests,updated_records);
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
  hs::progress::ProgressManager* progress_manager;
};

struct ResponseAddPhotos
{
  RESPONSE_HEADER
  typedef Database::Identifier Identifier;
  typedef std::map<Identifier, RequestAddPhotos::PhotoEntry>
          AddedPhotoEntryContainer;

  AddedPhotoEntryContainer added_photos;
  //hs::progress::ProgressManager progress_manager;
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
      add_request[PhotoResource::PHOTO_FIELD_COORDINATE_SYSTEM] =
        itr_photo_entry->coordinate_system;

      add_requests.push_back(add_request);
    }
    PhotoResource::AddedRecordContainer added_records;
    response.error_code =
      database_mediator.photo_resource_->Add(add_requests, added_records);

    //将加入的照片数据填充至response变量中
    auto itr_added_record = added_records.begin();
    auto itr_added_record_end = added_records.end();
    int run_index = 0;
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
        database_mediator.GetThumbnailPath(itr_added_record->first);
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

      photo_entry.thumbnail_path = thumbnail_path;

      response.added_photos[itr_added_record->first] = photo_entry;
      run_index++;
      request.progress_manager->SetCurrentSubProgressCompleteRatio(
        float(run_index) / float(added_records.size()));
      
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
      database_mediator.GetFeatureMatchPath(feature_match_id);
    if (!boost::filesystem::create_directory(feature_match_path))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
      return response.error_code;
    }

    FeatureMatchResource::UpdateRequest update_request;
    update_request[FeatureMatchResource::FEATURE_MATCH_FIELD_NAME] =
      feature_match_name;
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
  std::string feature_match_path;
  std::string keysets_path;
  std::string matches_path;
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
    response.feature_match_path =
      database_mediator.GetFeatureMatchPath(request.id);
    response.keysets_path = response.feature_match_path + "keysets.bin";
    response.matches_path = response.feature_match_path + "matches.bin";
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
    add_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FLAG] =
      int(PhotoOrientationResource::FLAG_NOT_COMPLETED);
    add_request[
      PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_COORDINATE_SYSTEM] =
        std::string("");
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
      database_mediator.GetPhotoOrientationPath(photo_orientation_id);
    if (!boost::filesystem::create_directory(photo_orientation_path))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
      return response.error_code;
    }

    PhotoOrientationResource::UpdateRequest update_request;
    update_request[PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_NAME] =
      photo_orientation_name;
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
  std::string tracks_path;
  std::string similar_transform_path;
  std::string workspace_path;
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
      database_mediator.GetPhotoOrientationPath(request.id);

    response.intrinsic_path =
      photo_orientation_path + "intrinsic.bin";
    response.extrinsic_path =
      photo_orientation_path + "extrinsic.bin";
    response.point_cloud_path =
      photo_orientation_path + "sparse_point_cloud.bin";
    response.tracks_path =
      photo_orientation_path + "tracks.bin";
    response.similar_transform_path =
      photo_orientation_path + "similar_transform.bin";
    response.workspace_path = photo_orientation_path;
    return response.error_code;
  }
};

//Request Add Point Cloud
struct RequestAddPointCloud
{
  REQUEST_HEADER
    typedef Database::Identifier Identifier;
  Identifier photo_orientation_id;
};

struct ResponseAddPointCloud
{
  RESPONSE_HEADER
    typedef Database::Identifier Identifier;
  Identifier point_cloud_id;
  Identifier photo_orientation_id;
  std::string name;
  std::string path;
};

template <>
struct DatabaseRequestHandler<RequestAddPointCloud,
                              ResponseAddPointCloud>
{
  int operator() (const RequestAddPointCloud& request,
                  ResponseAddPointCloud& response,
                  DatabaseMediator& database_mediator)
  {
    typedef Database::Identifier Identifier;
    PointCloudResource::AddRequest add_request;
    add_request[
      PointCloudResource::POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID] =
        int(request.photo_orientation_id);
      add_request[PointCloudResource::POINT_CLOUD_FIELD_NAME] =
        std::string("");
      add_request[PointCloudResource::POINT_CLOUD_FIELD_FLAG] =
        int(PointCloudResource::FLAG_NOT_COMPLETED);
      PointCloudResource::AddRequestContainer add_requests;
      add_requests.push_back(add_request);
      PointCloudResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.point_cloud_resource_->Add(add_requests,
        added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR ||
        added_records.empty())
      {
        return response.error_code;
      }

      Identifier point_cloud_id = added_records.begin()->first;
      std::string point_cloud_name =
        boost::str(boost::format("point_cloud_%1%") % point_cloud_id);
      boost::filesystem::path point_cloud_path =
        database_mediator.GetPointCloudPath(point_cloud_id);
      if (!boost::filesystem::create_directory(point_cloud_path))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
        return response.error_code;
      }

      PointCloudResource::UpdateRequest update_request;
      update_request[PointCloudResource::POINT_CLOUD_FIELD_NAME] =
        point_cloud_name;
      PointCloudResource::UpdateRequestContainer update_requests;
      update_requests[point_cloud_id] = update_request;
      PointCloudResource::UpdatedRecordContainer updated_records;
      response.error_code =
        database_mediator.point_cloud_resource_->Update(update_requests,
                                                        updated_records);

      if (response.error_code == DatabaseMediator::NO_ERROR)
      {
        response.point_cloud_id = point_cloud_id;;
        response.photo_orientation_id = request.photo_orientation_id;
        response.name = point_cloud_name;
        response.path = point_cloud_path.string();
      }

      return response.error_code;
  }
};

//Request Get Point Cloud
struct RequestGetPointCloud
{
  REQUEST_HEADER
    Database::Identifier id;
};

struct ResponseGetPointCloud
{
  RESPONSE_HEADER
    typedef Database::Identifier Identifier;
  PointCloudResource::Record record;
  std::string dense_pointcloud_path;
};

template <>
struct DatabaseRequestHandler<RequestGetPointCloud,
  ResponseGetPointCloud>
{
  int operator () (const RequestGetPointCloud& request,
    ResponseGetPointCloud& response,
    DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.point_cloud_resource_->GetById(request.id,
      response.record);

    std::string point_cloud_path =
      database_mediator.GetPointCloudPath(request.id);

    response.dense_pointcloud_path = point_cloud_path + "dense_pointcloud.ply";

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

//Request Update Photo Orientation Transform
struct RequestUpdatePhotoOrientationTransform
{
  REQUEST_HEADER
  Identifier id;
  Float scale;
  Float translate_x;
  Float translate_y;
  Float translate_z;
  Float rotation_x;
  Float rotation_y;
  Float rotation_z;
};

struct ResponseUpdatePhotoOrientationTransform
{
  RESPONSE_HEADER
  Float scale;
  Float translate_x;
  Float translate_y;
  Float translate_z;
  Float rotation_x;
  Float rotation_y;
  Float rotation_z;
};


template <>
struct DatabaseRequestHandler<RequestUpdatePhotoOrientationTransform,
                              ResponseUpdatePhotoOrientationTransform>
{
  int operator() (const RequestUpdatePhotoOrientationTransform& request,
                  ResponseUpdatePhotoOrientationTransform& response,
                  DatabaseMediator& database_mediator)
  {
    typedef hs::math::geometry::Rotation3D<double> Rotation;
    typedef EIGEN_VECTOR(double, 3) Vector3;

    PhotoOrientationResource::Record record;
    response.error_code =
      database_mediator.photo_orientation_resource_->GetById(request.id,
                                                             record);

    if (response.error_code == DatabaseMediator::NO_ERROR)
    {
      std::string photo_orientation_path =
        database_mediator.GetPhotoOrientationPath(request.id);
      std::string similar_transform_path =
        photo_orientation_path + "similar_transform.bin";

      double similar_scale = request.scale;
      Rotation similar_rotation;
      similar_rotation[0] = double(request.rotation_x);
      similar_rotation[1] = double(request.rotation_y);
      similar_rotation[2] = double(request.rotation_z);
      Vector3 similar_translate;
      similar_translate << double(request.translate_x),
                           double(request.translate_y),
                           double(request.translate_z);
      {
        std::ofstream similar_file(similar_transform_path, std::ios::binary);
        cereal::PortableBinaryOutputArchive archive(similar_file);
        archive(similar_scale, similar_rotation, similar_translate);
      }

      response.scale = request.scale;
      response.rotation_x = request.rotation_x;
      response.rotation_y = request.rotation_y;
      response.rotation_z = request.rotation_z;
      response.translate_x = request.translate_x;
      response.translate_y = request.translate_y;
      response.translate_z = request.translate_z;
    }

    return response.error_code;
  }
};

//Request Update Photo Orientation Coordinate System
struct RequestUpdatePhotoOrientationCoordinateSystem
{
  REQUEST_HEADER
  Identifier id;
  std::string coordinate_system;
};

struct ResponseUpdatePhotoOrientationCoordinateSystem
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdatePhotoOrientationCoordinateSystem,
                              ResponseUpdatePhotoOrientationCoordinateSystem>
{
  int operator() (const RequestUpdatePhotoOrientationCoordinateSystem& request,
                  ResponseUpdatePhotoOrientationCoordinateSystem& response,
                  DatabaseMediator& database_mediator)
  {
    PhotoOrientationResource::UpdateRequest update_request;
    update_request[
      PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_COORDINATE_SYSTEM] =
        request.coordinate_system;
    PhotoOrientationResource::UpdateRequestContainer update_requests;
    update_requests[request.id] = update_request;
    PhotoOrientationResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.photo_orientation_resource_->Update(update_requests,
                                                            updated_records);
    return response.error_code;
  }
};

//Request Update Point Cloud Flag
struct RequestUpdatePointCloudFlag
{
  REQUEST_HEADER
    Database::Identifier id;
  int flag;
};

struct ResponseUpdatePointCloudFlag
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdatePointCloudFlag,
                              ResponseUpdatePointCloudFlag>
{
  int operator() (const RequestUpdatePointCloudFlag& request,
                  ResponseUpdatePointCloudFlag& response,
    DatabaseMediator& database_mediator)
  {
    PointCloudResource::UpdateRequest update_request;
    update_request[PointCloudResource::POINT_CLOUD_FIELD_FLAG] =
      request.flag;
    PointCloudResource::UpdateRequestContainer update_requests;
    update_requests[request.id] = update_request;
    PointCloudResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.point_cloud_resource_->Update(update_requests,
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

//Request Get All Point Cloud
struct RequestGetAllPointClouds
{
  REQUEST_HEADER
};
struct ResponseGetAllPointClouds
{
  RESPONSE_HEADER
  PointCloudResource::RecordContainer records;
};
template <>
struct DatabaseRequestHandler<RequestGetAllPointClouds,
                              ResponseGetAllPointClouds>
{
  int operator() (const RequestGetAllPointClouds& request,
                  ResponseGetAllPointClouds& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.point_cloud_resource_->GetAll(response.records);
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

//Request Add Surface Model
struct RequestAddSurfaceModel
{
  REQUEST_HEADER
    typedef Database::Identifier Identifier;
  Identifier point_cloud_id;
};

struct ResponseAddSurfaceModel
{
  RESPONSE_HEADER
    typedef Database::Identifier Identifier;
  Identifier surface_model_id;
  Identifier point_cloud_id;
  std::string name;
  std::string path;
};

template <>
struct DatabaseRequestHandler<RequestAddSurfaceModel,
  ResponseAddSurfaceModel>
{
  int operator() (const RequestAddSurfaceModel& request,
    ResponseAddSurfaceModel& response,
    DatabaseMediator& database_mediator)
  {
    typedef Database::Identifier Identifier;
    SurfaceModelResource::AddRequest add_request;
    add_request[
      SurfaceModelResource::SURFACE_MODEL_FIELD_POINT_CLOUD_ID] =
        int(request.point_cloud_id);
      add_request[SurfaceModelResource::SURFACE_MODEL_FIELD_NAME] =
        std::string("");
      add_request[SurfaceModelResource::SURFACE_MODEL_FIELD_FLAG] =
        SurfaceModelResource::FLAG_NOT_COMPLETED;
      SurfaceModelResource::AddRequestContainer add_requests;
      add_requests.push_back(add_request);
      SurfaceModelResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.surface_model_resource_->Add(add_requests,
        added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR ||
        added_records.empty())
      {
        return response.error_code;
      }

      Identifier surface_model_id = added_records.begin()->first;
      std::string surface_model_name =
        boost::str(boost::format("surface_model_%1%") % surface_model_id);
      boost::filesystem::path surface_model_path =
        database_mediator.GetSurfaceModelPath(surface_model_id);
      if (!boost::filesystem::create_directory(surface_model_path))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
        return response.error_code;
      }

      SurfaceModelResource::UpdateRequest update_request;
      update_request[SurfaceModelResource::SURFACE_MODEL_FIELD_NAME] =
        surface_model_name;
      SurfaceModelResource::UpdateRequestContainer update_requests;
      update_requests[surface_model_id] = update_request;
      SurfaceModelResource::UpdatedRecordContainer updated_records;
      response.error_code =
        database_mediator.surface_model_resource_->Update(update_requests,
        updated_records);

      if (response.error_code == DatabaseMediator::NO_ERROR)
      {
        response.surface_model_id = surface_model_id;;
        response.point_cloud_id = request.point_cloud_id;
        response.name = surface_model_name;
        response.path = surface_model_path.string();
      }

      return response.error_code;
  }
};

//Request Add Texture
struct RequestAddTexture
{
  REQUEST_HEADER
  typedef Database::Identifier Identifier;
  Identifier surface_model_id;
};

struct ResponseAddTexture
{
  RESPONSE_HEADER
  typedef Database::Identifier Identifier;
  Identifier texture_id;
  Identifier surface_model_id;
  std::string name;
  std::string path;
};

template <>
struct DatabaseRequestHandler<RequestAddTexture,
                              ResponseAddTexture>
{
  int operator() (const RequestAddTexture& request,
                  ResponseAddTexture& response,
                  DatabaseMediator& database_mediator)
  {
    typedef Database::Identifier Identifier;
    TextureResource::AddRequest add_request;
    add_request[
      TextureResource::TEXTURE_FIELD_SURFACE_MODEL_ID] =
        int(request.surface_model_id);
      add_request[TextureResource::TEXTURE_FIELD_NAME] =
        std::string("");
      add_request[TextureResource::TEXTURE_FIELD_FLAG] =
        TextureResource::FLAG_NOT_COMPLETED;
      TextureResource::AddRequestContainer add_requests;
      add_requests.push_back(add_request);
      TextureResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.texture_resource_->Add(add_requests,
        added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR ||
        added_records.empty())
      {
        return response.error_code;
      }

      Identifier texture_id = added_records.begin()->first;
      std::string texture_name =
        boost::str(boost::format("texture_%1%") % texture_id);
      boost::filesystem::path texture_path =
        database_mediator.GetTexturePath(texture_id);
      if (!boost::filesystem::create_directory(texture_path))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_CREATE_DIRECTORY;
        return response.error_code;
      }

      TextureResource::UpdateRequest update_request;
      update_request[TextureResource::TEXTURE_FIELD_NAME] =
        texture_name;
      TextureResource::UpdateRequestContainer update_requests;
      update_requests[texture_id] = update_request;
      TextureResource::UpdatedRecordContainer updated_records;
      response.error_code =
        database_mediator.texture_resource_->Update(update_requests,
        updated_records);

      if (response.error_code == DatabaseMediator::NO_ERROR)
      {
        response.texture_id = texture_id;;
        response.surface_model_id = request.surface_model_id;
        response.name = texture_name;
        response.path = texture_path.string();
      }

      return response.error_code;
  }
};

//Reequest Get Surface Model
struct RequestGetSurfaceModel
{
  REQUEST_HEADER
    Database::Identifier id;
};

struct ResponseGetSurfaceModel
{
  RESPONSE_HEADER
  SurfaceModelResource::Record record;
  std::string mesh_path;
  std::string output_xml_path;
};

template <>
struct DatabaseRequestHandler<RequestGetSurfaceModel,
  ResponseGetSurfaceModel>
{
  int operator () (const RequestGetSurfaceModel& request,
    ResponseGetSurfaceModel& response,
    DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.surface_model_resource_->GetById(request.id,
      response.record);

    std::string surface_model_path =
      database_mediator.GetSurfaceModelPath(request.id);

        response.mesh_path =
          surface_model_path + "mesh.ply";
        response.output_xml_path =
          surface_model_path + "output.xml";

        return response.error_code;
  }
};

//Reequest Get Texture
struct RequestGetTexture
{
  REQUEST_HEADER
  Database::Identifier id;
};

struct ResponseGetTexture
{
  RESPONSE_HEADER
  TextureResource::Record record;
};

template <>
struct DatabaseRequestHandler<RequestGetTexture, ResponseGetTexture>
{
  int operator () (const RequestGetTexture& request,
                   ResponseGetTexture& response,
    DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.texture_resource_->GetById(request.id,
                                                   response.record);

    return response.error_code;
  }
};

//Request Update SurfaceModel Flag
struct RequestUpdateSurfaceModelFlag
{
  REQUEST_HEADER
    Database::Identifier id;
  int flag;
};

struct ResponseUpdateSurfaceModelFlag
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdateSurfaceModelFlag,
  ResponseUpdateSurfaceModelFlag>
{
  int operator() (const RequestUpdateSurfaceModelFlag& request,
    ResponseUpdateSurfaceModelFlag& response,
    DatabaseMediator& database_mediator)
  {
    SurfaceModelResource::UpdateRequest update_request;
    update_request[SurfaceModelResource::SURFACE_MODEL_FIELD_FLAG] =
      request.flag;
    SurfaceModelResource::UpdateRequestContainer update_requests;
    update_requests[request.id] = update_request;
    SurfaceModelResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.surface_model_resource_->Update(update_requests,
      updated_records);
    return response.error_code;
  }
};

//Request Update Texture Flag
struct RequestUpdateTextureFlag
{
  REQUEST_HEADER
  Database::Identifier id;
  int flag;
};

struct ResponseUpdateTextureFlag
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler<RequestUpdateTextureFlag,
                              ResponseUpdateTextureFlag>
{
  int operator() (const RequestUpdateTextureFlag& request,
                  ResponseUpdateTextureFlag& response,
                  DatabaseMediator& database_mediator)
  {
    TextureResource::UpdateRequest update_request;
    update_request[TextureResource::TEXTURE_FIELD_FLAG] =
      request.flag;
    TextureResource::UpdateRequestContainer update_requests;
    update_requests[request.id] = update_request;
    TextureResource::UpdatedRecordContainer updated_records;
    response.error_code =
      database_mediator.texture_resource_->Update(update_requests,
                                                  updated_records);
    return response.error_code;
  }
};

//Request Get All Surface Models
struct RequestGetAllSurfaceModels
{
  REQUEST_HEADER
};
struct ResponseGetAllSurfaceModels
{
  RESPONSE_HEADER
    SurfaceModelResource::RecordContainer records;
};
template <>
struct DatabaseRequestHandler<RequestGetAllSurfaceModels,
  ResponseGetAllSurfaceModels>
{
  int operator() (const RequestGetAllSurfaceModels& request,
    ResponseGetAllSurfaceModels& response,
    DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.surface_model_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Get All Texture
struct RequestGetAllTextures
{
  REQUEST_HEADER
};
struct ResponseGetAllTextures
{
  RESPONSE_HEADER
  TextureResource::RecordContainer records;
};
template <>
struct DatabaseRequestHandler<RequestGetAllTextures,
                              ResponseGetAllTextures>
{
  int operator() (const RequestGetAllTextures& request,
                  ResponseGetAllTextures& response,
                  DatabaseMediator& database_mediator)
  {
    response.error_code =
      database_mediator.texture_resource_->GetAll(response.records);
    return response.error_code;
  }
};

//Request Copy Block
struct RequestCopyBlock
{
  REQUEST_HEADER
  Identifier block_id;
};

struct ResponseCopyBlock
{
  RESPONSE_HEADER
  Identifier copied_block_id;
  std::string copied_block_name;
  std::vector<Identifier> photo_ids;
};

template <>
struct DatabaseRequestHandler <RequestCopyBlock,
                               ResponseCopyBlock>
{
  int operator() (const RequestCopyBlock& request,
                  ResponseCopyBlock& response,
                  DatabaseMediator& database_mediator)
  {
    while (1)
    {
      BlockResource::AddRequest block;
      response.error_code =
        database_mediator.block_resource_->GetById(
        request.block_id, block);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_block_name =
        block[BlockResource::BLOCK_FIELD_NAME].ToString();
      response.copied_block_name += "_copied";
      int repeat = 0;
      std::string base_name = response.copied_block_name;
      do
      {
        BlockResource::SelectedRecordContainer selected_records;
        BlockResource::SelectMask select_mask;
        select_mask.set();
        database_mediator.block_resource_->Select(
          select_mask,
          EqualTo(database_mediator.block_resource_->fields_[
                    BlockResource::BLOCK_FIELD_NAME],
                  Value(response.copied_block_name)),
          selected_records);
        if (selected_records.empty())
        {
          break;
        }

        repeat++;
        std::stringstream ss;
        ss << base_name << "(" << repeat << ")";
        ss >> response.copied_block_name;
      } while (1);

      block[BlockResource::BLOCK_FIELD_NAME] = response.copied_block_name;
      BlockResource::AddRequestContainer add_requests(1, block);
      BlockResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.block_resource_->Add(add_requests, added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_block_id = added_records.begin()->first;

      //复制块与照片的关系

      PhotoBlockRelationResource::SelectMask select_mask;
      select_mask.set();
      PhotoBlockRelationResource::SelectedRecordContainer selected_records;
      database_mediator.photo_block_relation_resource_->Select(
        select_mask,
        EqualTo(database_mediator.photo_block_relation_resource_->fields_[
                  PhotoBlockRelationResource::PBR_FIELD_BLOCK_ID],
                Value(int(request.block_id))),
        selected_records);
      PhotoBlockRelationResource::AddRequestContainer pbr_add_requests;
      for (const auto& record : selected_records)
      {
        PhotoBlockRelationResource::AddRequest pbr_add_request;
        pbr_add_request[PhotoBlockRelationResource::PBR_FIELD_BLOCK_ID] =
          int(response.copied_block_id);
        auto itr_photo_id_value = record.second.find(
          int(PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID));
        pbr_add_request[PhotoBlockRelationResource::PBR_FIELD_PHOTO_ID] =
          itr_photo_id_value->second;
        pbr_add_requests.push_back(pbr_add_request);
        response.photo_ids.push_back(
          Identifier(itr_photo_id_value->second.ToInt()));
      }

      PhotoBlockRelationResource::AddedRecordContainer pbr_added_records;
      response.error_code =
        database_mediator.photo_block_relation_resource_->Add(
          pbr_add_requests, pbr_added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }

      break;
    }

    return response.error_code;
  }
};

bool HS_EXPORT CopyDirectory(boost::filesystem::path const & source,
                   boost::filesystem::path const & destination);

//Request Copy Feature Match
struct RequestCopyFeatureMatch
{
  REQUEST_HEADER
  Identifier feature_match_id;
};

struct ResponseCopyFeatureMatch
{
  RESPONSE_HEADER
  Identifier copied_feature_match_id;
  Identifier block_id;
  std::string copied_feature_match_name;
};

template <>
struct DatabaseRequestHandler <RequestCopyFeatureMatch,
                               ResponseCopyFeatureMatch>
{
  int operator() (const RequestCopyFeatureMatch& request,
                  ResponseCopyFeatureMatch& response,
                  DatabaseMediator& database_mediator)
  {
    while (1)
    {
      FeatureMatchResource::AddRequest feature_match;
      response.error_code =
        database_mediator.feature_match_resource_->GetById(
          request.feature_match_id, feature_match);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.block_id =
        Identifier(feature_match[
          FeatureMatchResource::FEATURE_MATCH_FIELD_BLOCK_ID].ToInt());
      response.copied_feature_match_name =
        feature_match[
          FeatureMatchResource::FEATURE_MATCH_FIELD_NAME].ToString();
      response.copied_feature_match_name += "_copied";
      int repeat = 0;
      std::string base_name = response.copied_feature_match_name;
      do
      {
        FeatureMatchResource::SelectedRecordContainer selected_records;
        FeatureMatchResource::SelectMask select_mask;
        select_mask.set();
        database_mediator.feature_match_resource_->Select(
          select_mask,
          EqualTo(database_mediator.feature_match_resource_->fields_[
                    FeatureMatchResource::FEATURE_MATCH_FIELD_NAME],
                  Value(response.copied_feature_match_name)),
          selected_records);
        if (selected_records.empty())
        {
          break;
        }
        repeat++;
        std::stringstream ss;
        ss << base_name << "(" << repeat << ")";
        ss >> response.copied_feature_match_name;
      } while (1);

      feature_match[
        FeatureMatchResource::FEATURE_MATCH_FIELD_NAME] =
          response.copied_feature_match_name;
      FeatureMatchResource::AddRequestContainer add_requests(
        1, feature_match);
      FeatureMatchResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.feature_match_resource_->Add(
          add_requests, added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_feature_match_id = added_records.begin()->first;

      std::string feature_match_path =
        database_mediator.GetFeatureMatchPath(
          request.feature_match_id);
      std::string feature_match_path_copied =
        database_mediator.GetFeatureMatchPath(
          response.copied_feature_match_id);

      if (!CopyDirectory(
            boost::filesystem::path(feature_match_path),
            boost::filesystem::path(feature_match_path_copied)))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_COPY_DIRECTORY;
        break;
      }

      break;
    }
    return response.error_code;
  }
};

//Request Copy PhotoOrientation
struct RequestCopyPhotoOrientation
{
  REQUEST_HEADER
  Identifier photo_orientation_id;
};

struct ResponseCopyPhotoOrientation
{
  RESPONSE_HEADER
  Identifier copied_photo_orientation_id;
  Identifier feature_match_id;
  std::string copied_photo_orientation_name;
};

template <>
struct DatabaseRequestHandler <RequestCopyPhotoOrientation,
                               ResponseCopyPhotoOrientation>
{
  int operator() (const RequestCopyPhotoOrientation& request,
                  ResponseCopyPhotoOrientation& response,
                  DatabaseMediator& database_mediator)
  {
    while (1)
    {
      PhotoOrientationResource::AddRequest photo_orientation;
      response.error_code =
        database_mediator.photo_orientation_resource_->GetById(
          request.photo_orientation_id, photo_orientation);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.feature_match_id =
        Identifier(photo_orientation[
          PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_FEATURE_MATCH_ID].ToInt());
      response.copied_photo_orientation_name =
        photo_orientation[
          PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_NAME].ToString();
      response.copied_photo_orientation_name += "_copied";
      int repeat = 0;
      std::string base_name = response.copied_photo_orientation_name;
      do
      {
        PhotoOrientationResource::SelectedRecordContainer selected_records;
        PhotoOrientationResource::SelectMask select_mask;
        select_mask.set();
        database_mediator.photo_orientation_resource_->Select(
          select_mask,
          EqualTo(database_mediator.photo_orientation_resource_->fields_[
                    PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_NAME],
                  Value(response.copied_photo_orientation_name)),
          selected_records);
        if (selected_records.empty())
        {
          break;
        }
        repeat++;
        std::stringstream ss;
        ss << base_name << "(" << repeat << ")";
        ss >> response.copied_photo_orientation_name;
      } while (1);

      photo_orientation[
        PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_NAME] =
          response.copied_photo_orientation_name;
      PhotoOrientationResource::AddRequestContainer add_requests(
        1, photo_orientation);
      PhotoOrientationResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.photo_orientation_resource_->Add(
          add_requests, added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_photo_orientation_id = added_records.begin()->first;

      std::string photo_orientation_path =
        database_mediator.GetPhotoOrientationPath(
          request.photo_orientation_id);
      std::string photo_orientation_path_copied =
        database_mediator.GetPhotoOrientationPath(
          response.copied_photo_orientation_id);

      if (!CopyDirectory(
            boost::filesystem::path(photo_orientation_path),
            boost::filesystem::path(photo_orientation_path_copied)))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_COPY_DIRECTORY;
        break;
      }

      //拷贝所有照片测量
      {
        db::PhotoMeasureResource::SelectMask select_mask;
        select_mask.set();
        db::PhotoMeasureResource::SelectedRecordContainer selected_records;
        database_mediator.photo_measure_resource_->Select(
          select_mask,
          EqualTo(database_mediator.photo_measure_resource_->fields_[
                    db::PhotoMeasureResource::
                      PHOTO_MEASURE_FIELD_PHOTO_ORIENTATION_ID],
                   Value(int(request.photo_orientation_id))),
          selected_records);

        db::PhotoMeasureResource::AddRequestContainer add_requests;
        db::PhotoMeasureResource::AddedRecordContainer added_records;
        for (auto& record : selected_records)
        {
          db::PhotoMeasureResource::AddRequest add_request;
          for (int field_id =
                 db::PhotoMeasureResource::PHOTO_MEASURE_FIELD_ID + 1;
               field_id <
                 db::PhotoMeasureResource::NUMBER_OF_PHOTO_MEASURE_FIELDS;
               field_id++)
          {
            add_request[field_id] = record.second[field_id];
          }
          add_request[
            db::PhotoMeasureResource::
              PHOTO_MEASURE_FIELD_PHOTO_ORIENTATION_ID] =
            int(response.copied_photo_orientation_id);
          add_requests.push_back(add_request);
        }
        database_mediator.photo_measure_resource_->Add(
          add_requests, added_records);
      }

      break;
    }
    return response.error_code;
  }
};

//Request Copy Point Cloud
struct RequestCopyPointCloud
{
  REQUEST_HEADER
  Identifier point_cloud_id;
};

struct ResponseCopyPointCloud
{
  RESPONSE_HEADER
  Identifier copied_point_cloud_id;
  Identifier photo_orientation_id;
  std::string copied_point_cloud_name;
};

template <>
struct DatabaseRequestHandler <RequestCopyPointCloud,
                               ResponseCopyPointCloud>
{
  int operator() (const RequestCopyPointCloud& request,
                  ResponseCopyPointCloud& response,
                  DatabaseMediator& database_mediator)
  {
    while (1)
    {
      PointCloudResource::AddRequest point_cloud;
      response.error_code =
        database_mediator.point_cloud_resource_->GetById(
          request.point_cloud_id, point_cloud);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.photo_orientation_id =
        Identifier(point_cloud[
          PointCloudResource::POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID].ToInt());
      response.copied_point_cloud_name =
        point_cloud[
          PointCloudResource::POINT_CLOUD_FIELD_NAME].ToString();
      response.copied_point_cloud_name += "_copied";
      int repeat = 0;
      std::string base_name = response.copied_point_cloud_name;
      do
      {
        PointCloudResource::SelectedRecordContainer selected_records;
        PointCloudResource::SelectMask select_mask;
        select_mask.set();
        database_mediator.point_cloud_resource_->Select(
          select_mask,
          EqualTo(database_mediator.point_cloud_resource_->fields_[
                    PointCloudResource::POINT_CLOUD_FIELD_NAME],
                  Value(response.copied_point_cloud_name)),
          selected_records);
        if (selected_records.empty())
        {
          break;
        }
        repeat++;
        std::stringstream ss;
        ss << base_name << "(" << repeat << ")";
        ss >> response.copied_point_cloud_name;
      } while (1);

      point_cloud[
        PointCloudResource::POINT_CLOUD_FIELD_NAME] =
          response.copied_point_cloud_name;
      PointCloudResource::AddRequestContainer add_requests(
        1, point_cloud);
      PointCloudResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.point_cloud_resource_->Add(
          add_requests, added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_point_cloud_id = added_records.begin()->first;

      std::string point_cloud_path =
        database_mediator.GetPointCloudPath(
          request.point_cloud_id);
      std::string point_cloud_path_copied =
        database_mediator.GetPointCloudPath(
          response.copied_point_cloud_id);

      if (!CopyDirectory(
            boost::filesystem::path(point_cloud_path),
            boost::filesystem::path(point_cloud_path_copied)))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_COPY_DIRECTORY;
        break;
      }

      break;
    }
    return response.error_code;
  }
};

//Request Copy Surface Model
struct RequestCopySurfaceModel
{
  REQUEST_HEADER
  Identifier surface_model_id;
};

struct ResponseCopySurfaceModel
{
  RESPONSE_HEADER
  Identifier copied_surface_model_id;
  Identifier point_cloud_id;
  std::string copied_surface_model_name;
};

template <>
struct DatabaseRequestHandler <RequestCopySurfaceModel,
                               ResponseCopySurfaceModel>
{
  int operator() (const RequestCopySurfaceModel& request,
                  ResponseCopySurfaceModel& response,
                  DatabaseMediator& database_mediator)
  {
    while (1)
    {
      SurfaceModelResource::AddRequest surface_model;
      response.error_code =
        database_mediator.surface_model_resource_->GetById(
          request.surface_model_id, surface_model);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.point_cloud_id =
        Identifier(surface_model[
          SurfaceModelResource::SURFACE_MODEL_FIELD_POINT_CLOUD_ID].ToInt());
      response.copied_surface_model_name =
        surface_model[
          SurfaceModelResource::SURFACE_MODEL_FIELD_NAME].ToString();
      response.copied_surface_model_name += "_copied";
      int repeat = 0;
      std::string base_name = response.copied_surface_model_name;
      do
      {
        SurfaceModelResource::SelectedRecordContainer selected_records;
        SurfaceModelResource::SelectMask select_mask;
        select_mask.set();
        database_mediator.surface_model_resource_->Select(
          select_mask,
          EqualTo(database_mediator.surface_model_resource_->fields_[
                    SurfaceModelResource::SURFACE_MODEL_FIELD_NAME],
                  Value(response.copied_surface_model_name)),
          selected_records);
        if (selected_records.empty())
        {
          break;
        }
        repeat++;
        std::stringstream ss;
        ss << base_name << "(" << repeat << ")";
        ss >> response.copied_surface_model_name;
      } while (1);

      surface_model[
        SurfaceModelResource::SURFACE_MODEL_FIELD_NAME] =
          response.copied_surface_model_name;
      SurfaceModelResource::AddRequestContainer add_requests(
        1, surface_model);
      SurfaceModelResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.surface_model_resource_->Add(
          add_requests, added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_surface_model_id = added_records.begin()->first;

      std::string surface_model_path =
        database_mediator.GetSurfaceModelPath(
          request.surface_model_id);
      std::string surface_model_path_copied =
        database_mediator.GetSurfaceModelPath(
          response.copied_surface_model_id);

      if (!CopyDirectory(
            boost::filesystem::path(surface_model_path),
            boost::filesystem::path(surface_model_path_copied)))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_COPY_DIRECTORY;
        break;
      }

      break;
    }
    return response.error_code;
  }
};

//Request Copy Texture
struct RequestCopyTexture
{
  REQUEST_HEADER
  Identifier texture_id;
};

struct ResponseCopyTexture
{
  RESPONSE_HEADER
  Identifier copied_texture_id;
  Identifier surface_model_id;
  std::string copied_texture_name;
};

template <>
struct DatabaseRequestHandler <RequestCopyTexture,
                               ResponseCopyTexture>
{
  int operator() (const RequestCopyTexture& request,
                  ResponseCopyTexture& response,
                  DatabaseMediator& database_mediator)
  {
    while (1)
    {
      TextureResource::AddRequest texture;
      response.error_code =
        database_mediator.texture_resource_->GetById(
          request.texture_id, texture);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.surface_model_id =
        Identifier(texture[
          TextureResource::TEXTURE_FIELD_SURFACE_MODEL_ID].ToInt());
      response.copied_texture_name =
        texture[
          TextureResource::TEXTURE_FIELD_NAME].ToString();
      response.copied_texture_name += "_copied";
      int repeat = 0;
      std::string base_name = response.copied_texture_name;
      do
      {
        TextureResource::SelectedRecordContainer selected_records;
        TextureResource::SelectMask select_mask;
        select_mask.set();
        database_mediator.texture_resource_->Select(
          select_mask,
          EqualTo(database_mediator.texture_resource_->fields_[
                    TextureResource::TEXTURE_FIELD_NAME],
                  Value(response.copied_texture_name)),
          selected_records);
        if (selected_records.empty())
        {
          break;
        }
        repeat++;
        std::stringstream ss;
        ss << base_name << "(" << repeat << ")";
        ss >> response.copied_texture_name;
      } while (1);

      texture[
        TextureResource::TEXTURE_FIELD_NAME] =
          response.copied_texture_name;
      TextureResource::AddRequestContainer add_requests(
        1, texture);
      TextureResource::AddedRecordContainer added_records;
      response.error_code =
        database_mediator.texture_resource_->Add(
          add_requests, added_records);
      if (response.error_code != DatabaseMediator::NO_ERROR)
      {
        break;
      }
      response.copied_texture_id = added_records.begin()->first;

      std::string texture_path =
        database_mediator.GetTexturePath(
          request.texture_id);
      std::string texture_path_copied =
        database_mediator.GetTexturePath(
          response.copied_texture_id);

      if (!CopyDirectory(
            boost::filesystem::path(texture_path),
            boost::filesystem::path(texture_path_copied)))
      {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_COPY_DIRECTORY;
        break;
      }

      break;
    }
    return response.error_code;
  }
};

bool HS_EXPORT RemoveDirectory(boost::filesystem::path const & destination);

//Request Remove texture
struct RequestRemoveTextrue
{
  REQUEST_HEADER
  Identifier id;
};

struct ResponseRemoveTextrue
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler < RequestRemoveTextrue,
  ResponseRemoveTextrue >
{
  int operator() (const RequestRemoveTextrue& request,
                  ResponseRemoveTextrue& response,
                  DatabaseMediator& database_mediator)
  {
    //删除TEXTURE文件夹
    std::string texture_path = database_mediator.GetTexturePath(request.id);
    if(!RemoveDirectory(boost::filesystem::path(texture_path)))
    {
        response.error_code = DatabaseMediator::ERROR_FAIL_TO_REMOVE_DIRECTORY;
    }

    //删除TEXTURE数据库项
    response.error_code =
      database_mediator.texture_resource_->Delete(
      EqualTo(BlockResource::fields_[0], Value(int(request.id))));
    return response.error_code;
  }
};

//Request Remove Surface Model
struct RequestRemoveSurfaceModel
{
  REQUEST_HEADER
  Identifier id;
};

struct ResponseRemoveSurfaceModel
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler < RequestRemoveSurfaceModel,
  ResponseRemoveSurfaceModel >
{
  int operator() (const RequestRemoveSurfaceModel& request,
                  ResponseRemoveSurfaceModel& response,
                  DatabaseMediator& database_mediator)
  {
    //删除SURFACEMODEL对应的TEXTURE
    TextureResource::RecordContainer records;
    database_mediator.texture_resource_->GetAll(records);
    for(auto iter = records.begin();iter != records.end(); ++iter)
    {
      if(iter->second[TextureResource::TEXTURE_FIELD_SURFACE_MODEL_ID].ToInt()
           == request.id)
      {
        RequestRemoveTextrue remove_request;
        remove_request.id =
          iter->second[TextureResource::TEXTURE_FIELD_ID].ToInt();
        ResponseRemoveTextrue remove_reponse;
        response.error_code = database_mediator.Request(
          nullptr, DatabaseMediator::REQUEST_REMOVE_TEXTURE,
          remove_request, remove_reponse, false);
        if (response.error_code != DatabaseMediator::NO_ERROR)
        {
          return response.error_code;
        }
      }
    }

    //删除SURFACEMODEL文件夹
    std::string surface_model_path =
      database_mediator.GetSurfaceModelPath(request.id);

    if(!RemoveDirectory(boost::filesystem::path(surface_model_path)))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_REMOVE_DIRECTORY;
    }

    //删除SURFACEMODEL数据库项
    response.error_code =
      database_mediator.surface_model_resource_->Delete(
      EqualTo(BlockResource::fields_[0], Value(int(request.id))));
    return response.error_code;
  }
};

//Request Remove Point Cloud
struct RequestRemovePointCloud
{
  REQUEST_HEADER
  Identifier id;
};

struct ResponseRemovePointCloud
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler < RequestRemovePointCloud,
  ResponseRemovePointCloud >
{
  int operator() (const RequestRemovePointCloud& request,
                  ResponseRemovePointCloud& response,
                  DatabaseMediator& database_mediator)
  {
    //删除POINTCLOUD对应的SURFACEMODEL
    SurfaceModelResource::RecordContainer records;
    database_mediator.surface_model_resource_->GetAll(records);
    for(auto iter = records.begin(); iter != records.end(); ++iter)
    {
      if(iter->second[SurfaceModelResource::SURFACE_MODEL_FIELD_POINT_CLOUD_ID].ToInt()
           == request.id)
      {
        RequestRemoveSurfaceModel remove_request;
        remove_request.id = iter->second[
          SurfaceModelResource::SURFACE_MODEL_FIELD_ID].ToInt();
        ResponseRemoveSurfaceModel remove_reponse;
        response.error_code = database_mediator.Request(
          nullptr, DatabaseMediator::REQUEST_REMOVE_SURFACE_MODEL,
          remove_request, remove_reponse, false);
        if(response.error_code != DatabaseMediator::NO_ERROR)
        {
          return response.error_code;
        }
      }
    }

    //删除POINTCLOUD文件夹
    std::string pointcloud_path =
      database_mediator.GetPointCloudPath(request.id);

    if(!RemoveDirectory(boost::filesystem::path(pointcloud_path)))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_REMOVE_DIRECTORY;
    }

    //删除POINTCLOUD数据库项
    response.error_code =
      database_mediator.point_cloud_resource_->Delete(
      EqualTo(BlockResource::fields_[0], Value(int(request.id))));
    return response.error_code;
  }
};

//Request Remove Photo Orientation
struct RequestRemovePhotoOrientation
{
  REQUEST_HEADER
  Identifier id;
};

struct ResponseRemovePhotoOrientation
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler < RequestRemovePhotoOrientation,
  ResponseRemovePhotoOrientation >
{
  int operator() (const RequestRemovePhotoOrientation& request,
                  ResponseRemovePhotoOrientation& response,
                  DatabaseMediator& database_mediator)
  {
    //删除PHOTOORIENTATION对应的POINTCLOUD
    PointCloudResource::RecordContainer records;
    database_mediator.point_cloud_resource_->GetAll(records);
    for(auto iter = records.begin(); iter != records.end(); ++iter)
    {
      if(iter->second[PointCloudResource::
        POINT_CLOUD_FIELD_PHOTO_ORIENTATION_ID].ToInt() == request.id)
      {
        RequestRemovePointCloud remove_request;
        remove_request.id =
          iter->second[PointCloudResource::POINT_CLOUD_FIELD_ID].ToInt();
        ResponseRemovePointCloud remove_reponse;
        response.error_code = database_mediator.Request(
          nullptr, DatabaseMediator::REQUEST_REMOVE_POINT_CLOUD,
          remove_request, remove_reponse, false);
        if(response.error_code != DatabaseMediator::NO_ERROR)
        {
          return response.error_code;
        }
      }
    }

    //删除PHOTOORIENTATION文件夹
    std::string photo_orientation_path =
      database_mediator.GetPhotoOrientationPath(request.id);

    if(!RemoveDirectory(boost::filesystem::path(photo_orientation_path)))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_REMOVE_DIRECTORY;
    }

    //删除PHOTOORIENTATION数据库项
    response.error_code =
      database_mediator.photo_orientation_resource_->Delete(
      EqualTo(BlockResource::fields_[0], Value(int(request.id))));
    return response.error_code;
  }
};

//Request Remove Feature match
struct RequestRemoveFeatureMatch
{
  REQUEST_HEADER
  Identifier id;
};

struct ResponseRemoveFeatureMatch
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler < RequestRemoveFeatureMatch,
  ResponseRemoveFeatureMatch >
{
  int operator() (const RequestRemoveFeatureMatch& request,
                  ResponseRemoveFeatureMatch& response,
                  DatabaseMediator& database_mediator)
  {
    //删除FEATUREMATCH对应的PHOTOORIENTATION
    PhotoOrientationResource::RecordContainer records;
    database_mediator.photo_orientation_resource_->GetAll(records);
    for(auto iter = records.begin(); iter != records.end(); ++iter)
    {
      if(iter->second[PhotoOrientationResource::
        PHOTO_ORIENTATION_FIELD_FEATURE_MATCH_ID].ToInt() == request.id)
      {
        RequestRemovePhotoOrientation remove_request;
        remove_request.id = iter->second[
          PhotoOrientationResource::PHOTO_ORIENTATION_FIELD_ID].ToInt();
        ResponseRemovePhotoOrientation remove_reponse;
        response.error_code = database_mediator.Request(
          nullptr, DatabaseMediator::REQUEST_REMOVE_PHOTO_ORIENTATION,
          remove_request, remove_reponse, false);
        if(response.error_code != DatabaseMediator::NO_ERROR)
        {
          return response.error_code;
        }
      }
    }

    //删除FEATUREMATCH文件夹
    std::string feature_match_path =
      database_mediator.GetFeatureMatchPath(request.id);

    if(!RemoveDirectory(boost::filesystem::path(feature_match_path)))
    {
      response.error_code = DatabaseMediator::ERROR_FAIL_TO_REMOVE_DIRECTORY;
    }

    //删除FEATUREMATCH数据库项
    response.error_code =
      database_mediator.feature_match_resource_->Delete(
      EqualTo(BlockResource::fields_[0], Value(int(request.id))));
    return response.error_code;
  }
};

//Request Remove Block
struct RequestRemoveBlock
{
  REQUEST_HEADER
  Identifier id;
};

struct ResponseRemoveBlock
{
  RESPONSE_HEADER
};

template <>
struct DatabaseRequestHandler < RequestRemoveBlock,
  ResponseRemoveBlock >
{
  int operator() (const RequestRemoveBlock& request,
                  ResponseRemoveBlock& response,
                  DatabaseMediator& database_mediator)
  {
    //删除BLOCK对应的FEATUREMATCH
    FeatureMatchResource::RecordContainer records;
    database_mediator.feature_match_resource_->GetAll(records);
    for(auto iter = records.begin(); iter != records.end(); ++iter)
    {
      if(iter->second[FeatureMatchResource::
        FEATURE_MATCH_FIELD_BLOCK_ID].ToInt() == request.id)
      {
        RequestRemoveFeatureMatch remove_request;
        remove_request.id = iter->second[
          FeatureMatchResource::FEATURE_MATCH_FIELD_ID].ToInt();
        ResponseRemoveFeatureMatch remove_reponse;
        response.error_code = database_mediator.Request(
          nullptr, DatabaseMediator::REQUEST_REMOVE_FEATURE_MATCH,
          remove_request, remove_reponse, false);
        if(response.error_code != DatabaseMediator::NO_ERROR)
        {
          return response.error_code;
        }
      }
    }

    //删除BLOCK数据库项
    response.error_code =
      database_mediator.block_resource_->Delete(
      EqualTo(BlockResource::fields_[0], Value(int(request.id))));
    return response.error_code;
  }
};

}
}
}

#endif

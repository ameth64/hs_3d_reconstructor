#include <fstream>
#include <sstream>
#include <limits>

#include "workflow/texture/rough_texture.hpp"

#include "hs_texture/texture_multiview/dem_generator.hpp"
#include "hs_texture/texture_multiview/dem_split_tiff_engine.hpp"

namespace hs
{
namespace recon
{
namespace workflow
{

TextureConfig::TextureConfig()
{
  type_ = STEP_TEXTURE;
}

void TextureConfig::set_dem_x_scale(double dem_x_scale)
{
  dem_x_scale_ = dem_x_scale;
}
void TextureConfig::set_dem_y_scale(double dem_y_scale)
{
  dem_y_scale_ = dem_y_scale;
}
void TextureConfig::set_dem_path(const std::string& dem_path)
{
  dem_path_ = dem_path;
}
void TextureConfig::set_dem_tile_x_size(int dem_tile_x_size)
{
  dem_tile_x_size_ = dem_tile_x_size;
}
void TextureConfig::set_dem_tile_y_size(int dem_tile_y_size)
{
  dem_tile_y_size_ = dem_tile_y_size;
}
void TextureConfig::set_surface_model_path(
  const std::string& surface_model_math)
{
  surface_model_math_ = surface_model_math;
}
void TextureConfig::set_similar_transform(
  const SimilarTransform& similar_transform)
{
  similar_transform_ = similar_transform;
}

double TextureConfig::dem_x_scale() const
{
  return dem_x_scale_;
}
double TextureConfig::dem_y_scale() const
{
  return dem_y_scale_;
}
const std::string& TextureConfig::dem_path() const
{
  return dem_path_;
}
int TextureConfig::dem_tile_x_size() const
{
  return dem_tile_x_size_;
}
int TextureConfig::dem_tile_y_size() const
{
  return dem_tile_y_size_;
}
const std::string& TextureConfig::surface_model_path() const
{
  return surface_model_math_;
}
const TextureConfig::SimilarTransform& TextureConfig::similar_transform() const
{
  return similar_transform_;
}

RoughTexture::RoughTexture()
{
  type_ = STEP_TEXTURE;
}

int RoughTexture::LoadSurfaceModel(WorkflowStepConfig* config,
                                   VertexContainer& vertices,
                                   TriangleContainer& triangles)
{
  TextureConfig* texture_config = static_cast<TextureConfig*>(config);
  std::string surface_model_path = texture_config->surface_model_path();
  const TextureConfig::SimilarTransform& similar_transform =
    texture_config->similar_transform();

  const double threshold = 1e-9;
  if (std::abs(similar_transform.scale - 1.0) < threshold &&
      std::abs(similar_transform.rotation[0]) < threshold &&
      std::abs(similar_transform.rotation[1]) < threshold &&
      std::abs(similar_transform.rotation[2]) < threshold &&
      std::abs(similar_transform.translate[0]) < threshold &&
      std::abs(similar_transform.translate[1]) < threshold &&
      std::abs(similar_transform.translate[2]) < threshold) return -1;

  std::ifstream surface_file(surface_model_path);
  if (!surface_file) return -1;
  std::stringstream ss;
  std::string line;

#define TEXTURE_GETLINE(file, line, ss) \
  std::getline(file, line); \
  ss.clear(); \
  ss.str(line); \

  TEXTURE_GETLINE(surface_file, line, ss);
  TEXTURE_GETLINE(surface_file, line, ss);
  TEXTURE_GETLINE(surface_file, line, ss);

  std::string dummy;
  size_t number_of_vertices;
  ss>>dummy>>dummy>>number_of_vertices;
  TEXTURE_GETLINE(surface_file, line, ss);
  TEXTURE_GETLINE(surface_file, line, ss);
  TEXTURE_GETLINE(surface_file, line, ss);
  TEXTURE_GETLINE(surface_file, line, ss);
  size_t number_of_triangles;
  ss>>dummy>>dummy>>number_of_triangles;
  TEXTURE_GETLINE(surface_file, line, ss);
  TEXTURE_GETLINE(surface_file, line, ss);

  vertices.resize(number_of_vertices);
  triangles.resize(number_of_triangles);

  for (size_t i = 0; i < number_of_vertices; i++)
  {
    TEXTURE_GETLINE(surface_file, line, ss);
    ss>>vertices[i][0]>>vertices[i][1]>>vertices[i][2];
    vertices[i] = similar_transform.scale * (similar_transform.rotation *
                                             vertices[i]) +
                  similar_transform.translate;
  }

  for (size_t i = 0; i < number_of_triangles; i++)
  {
    TEXTURE_GETLINE(surface_file, line, ss);
    size_t three;
    ss>>three>>triangles[i][0]>>triangles[i][1]>>triangles[i][2];
  }

  surface_file.close();

#undef TEXTURE_GETLINE

  return 0;
}

int RoughTexture::GenerateDEM(WorkflowStepConfig* config,
                              const VertexContainer& vertices,
                              const TriangleContainer& triangles)
{
  typedef hs::texture::multiview::DEMGenerator<Scalar> Generator;
  typedef hs::texture::multiview::DEMSplitTiffEngine<Scalar> Engine;

  PlanarPoint min, max;
  min << std::numeric_limits<Scalar>::max(),
         std::numeric_limits<Scalar>::max();
  max << -std::numeric_limits<Scalar>::max(),
         -std::numeric_limits<Scalar>::max();
  for (size_t i = 0; i < vertices.size(); i++)
  {
    min[0] = std::min(min[0], vertices[i][0]);
    min[1] = std::min(min[1], vertices[i][1]);
    max[0] = std::max(max[0], vertices[i][0]);
    max[1] = std::max(max[1], vertices[i][1]);
  }

  TextureConfig* texture_config = static_cast<TextureConfig*>(config);
  const std::string& dem_path = texture_config->dem_path();
  size_t tile_width = size_t(texture_config->dem_tile_x_size());
  size_t tile_height = size_t(texture_config->dem_tile_y_size());
  Scalar scale_x = texture_config->dem_x_scale();
  Scalar scale_y = texture_config->dem_y_scale();

  Generator generator;
  Engine engine;

  return generator(dem_path, engine, vertices, triangles, min, max,
                   tile_width, tile_height, Scalar(-32767),
                   scale_x, scale_y, Scalar(0), &progress_manager_);
}

int RoughTexture::RunImplement(WorkflowStepConfig* config)
{
  int result = 0;

  while (1)
  {
    VertexContainer vertices;
    TriangleContainer triangles;
    progress_manager_.AddSubProgress(0.1f);
    result = LoadSurfaceModel(config, vertices, triangles);
    if (result != 0) break;
    progress_manager_.FinishCurrentSubProgress();

    progress_manager_.AddSubProgress(0.9f);
    result = GenerateDEM(config, vertices, triangles);
    if (result != 0) break;
    progress_manager_.FinishCurrentSubProgress();

    break;
  }

  return result;
}

}
}
}
////////////////////////////////////////////////////////
//  File			:	"CTextureManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	04/05/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN
//
//  Purpose			:	Wrapper class for manageing Textures for
//  Direct3D
////////////////////////////////////////////////////////
#pragma once

#include "components/rendering/sprite.h"
#include "core/containers.h"
#include "rendering/bitmap_font.h"

#include <filesystem>
#include <string>

#include <cstdint>

// DirectX Forward Declares
struct DIRECT3DTEXTURE9;
struct DIRECT3DDEVICE9;
struct D3DXSPRITE;

namespace scd {
struct texture {
  DIRECT3DTEXTURE9* texture{nullptr};
  int width{0};
  int height{0};

  ~texture();
};

class texture_manager {
public:
  texture_manager(DIRECT3DDEVICE9* d3d_device, D3DXSPRITE* d3d_sprite);
  ~texture_manager();

  /**
   * @brief Loads a texture from a file.
   *
   * Image dimensions must be a power of 2 (e.g. 256x64).
   * Supports .bmp, .dds, .dib, .hdr, .jpg, .pfm, .ppm, and .tga files.
   *
   * @param filename The path of the texture file.
   * @param color The color key of the texture file.
   * @return std::shared_ptr<texture> The loaded texture.
   */
  std::shared_ptr<texture>
  load_texture(const std::filesystem::path& filename, std::uint32_t color = 0);

  /**
   * @brief Draws a texture to the screen
   *
   * @note Drawing a section of an image will only work properly if that image
   * is a power of 2
   *
   * @param data The sprite data
   * @return true
   * @return false
   */
  bool draw(const sprite_data& data);

  std::shared_ptr<scd::component::sprite>
  create_sprite(object& owner, sprite_data data, bool active);

  std::shared_ptr<bitmap_font> create_font(
      object& owner,
      std::string word,
      bitmap_font font,
      int x,
      int y,
      float scale,
      std::uint32_t color,
      bool active);

  void render_sprites();

  // Event callback functions
  void RenderSpritesCallback(IEvent* e, IComponent* comp);

private:
  scd::map<std::string, std::weak_ptr<texture>> _textures;
  DIRECT3DDEVICE9* _d3d_device; // A pointer to the Direct3D device.
  D3DXSPRITE* _d3d_sprite;      // A pointer to the sprite interface.

  scd::set<std::weak_ptr<component::sprite>> _sprites;
  scd::set<std::weak_ptr<bitmap_font>> _fonts;
};

} // namespace scd

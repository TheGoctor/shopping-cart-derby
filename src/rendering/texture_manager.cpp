////////////////////////////////////////////////////////
//  File			:	"scd::texture_manager.cpp"
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

#include "texture_manager.h"

#include "components/rendering/sprite.h"
#include "core/object.h"
#include "events/event_manager.h"
#include "events/events.h"
#include "rendering/bitmap_font.h"

#include <string>

#include <cassert>

// Macro to safely Release COM Objects.
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
  if (p) {              \
    p->Release();       \
    p = nullptr;        \
  }
#endif

//	Macro for reporting Wrapper errors
#ifndef DXERROR
#define DXERROR(a)           \
  {                          \
    OutputDebugString(a);    \
    OutputDebugString("\n"); \
    return false;            \
  }
#endif

scd::texture::~texture() {
  if (texture) {
    // texture->Release();
    texture = nullptr;
  }
}

scd::texture_manager::texture_manager(
    DIRECT3DDEVICE9* _lpDevice,
    D3DXSPRITE* _lpSprite) {
  _d3d_device = _lpDevice;
  _d3d_device->AddRef();

  _d3d_sprite = _lpSprite;
  _d3d_sprite->AddRef();

  event_manager* event_manager;

  // Register for Events
  event_manager->register_event("RenderSprites", RenderSpritesCallback);
}

scd::texture_manager::~texture_manager() {
  _textures.clear();
  _sprites.clear();
  _fonts.clear();

  // Release our references to the sprite interface and d3d device
  SAFE_RELEASE(m_lpSprite);
  SAFE_RELEASE(m_lpDevice);
}

std::shared_ptr<scd::texture> scd::texture_manager::load_texture(
    std::filesystem::path filename,
    std::uint32_t _dwColorkey) {
  // Make sure the texture isn't already loaded.
  for (unsigned int i = 0; i < _textures.size(); i++) {
    // compare strings without caring about upper or lowercase.
    if (stricmp(_szFilename, _textures[i].filename)
        == 0) // 0 means they are equal.
    {
      _textures[i].ref++; // add a reference to this texture.
      return i;           // return the index.
    }
  }

  // Look for an open spot.
  int nID = -1;
  for (unsigned int i = 0; i < _textures.size(); i++) {
    if (_textures[i].ref == 0) {
      nID = i;
      break;
    }
  }

  // if we didn't find an open spot, load it in a new one
  if (nID == -1) {
    // A temp texture object.
    texture loaded;

    // Copy the filename of the loaded texture.
    strcpy(loaded.filename, _szFilename);

    // Load the texture from the given file.
    HRESULT hr = D3DXCreateTextureFromFileEx(
        m_lpDevice,
        _szFilename,
        0,
        0,
        D3DX_DEFAULT,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        _dwColorkey,
        0,
        0,
        &loaded.texture);

    if (FAILED(hr)) {
      // Failed.
      char szBuffer[256] = {0};
      sprintf(szBuffer, "Failed to Create Texture - %s", _szFilename);
      MessageBox(0, szBuffer, "TextureManager Error", MB_OK);
      return -1;
    }

    // AddRef.
    loaded.ref = 1;

    // Get surface description (to find Width/Height of the texture)
    D3DSURFACE_DESC d3dSurfDesc{};

    loaded.texture->GetLevelDesc(0, &d3dSurfDesc);

    // Remember the Width and Height
    loaded.width = d3dSurfDesc.Width;
    loaded.height = d3dSurfDesc.Height;

    // Put the texture into the list.
    int nIdx = m_Textures.size();
    m_Textures[nIdx] = loaded;

    // Return the nID of the texture.
    return nIdx;
  }
  // we found an open spot
  else {
    // Make sure the texture has been released.
    SAFE_RELEASE(m_Textures[nID].texture);

    // Load the texture from the given file.
    HRESULT hr = D3DXCreateTextureFromFileEx(
        m_lpDevice,
        _szFilename,
        0,
        0,
        D3DX_DEFAULT,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        _dwColorkey,
        0,
        0,
        &m_Textures[nID].texture);

    if (FAILED(hr)) {
      // Failed.
      char szBuffer[256] = {0};
      sprintf(szBuffer, "Failed to Create Texture - %s", _szFilename);
      MessageBox(0, szBuffer, "TextureManager Error", MB_OK);
      return -1;
    }

    // Get surface description (to find Width/Height of the texture)
    D3DSURFACE_DESC d3dSurfDesc{};

    m_Textures[nID].texture->GetLevelDesc(0, &d3dSurfDesc);

    // Remember the Width and Height
    m_Textures[nID].Width = d3dSurfDesc.Width;
    m_Textures[nID].Height = d3dSurfDesc.Height;

    // AddRef
    m_Textures[nID].ref = 1;

    // Return the nID of the texture.
    return nID;
  }
}

bool scd::texture_manager::draw(const sprite_data& data) {
  std::shared_ptr<texture> texture_data;
  // Make sure that the texture is valid
  assert(
      texture_data->texture != nullptr
      && "Attempting to draw released texture id");

  // Make sure the sprite was created and we have a valid texture.
  if (!_d3d_sprite)
    return false;

  scd::transform combined = scd::math::matrix_identity();

  // Rotate the sprite.
  combined = math::matrix_translate(
      combined,
      {-data.rotation_center.x * data.scale.x,
       -data.rotation_center.y * data.scale.y,
       0.0f});

  combined *= math::matrix_axis_angle({0, 0, 1}, data.rotation_angle);
  combined = math::matrix_translate(
      combined,
      {data.rotation_center.x * data.scale.x,
       data.rotation_center.y * data.scale.y,
       0.0f});

  // Translate the sprite
  combined *= math::matrix_translate(combined, {data.x, data.y, 0.0f});

  // Apply the transform.
  _d3d_sprite->SetTransform(&combined);

  _d3d_sprite->Begin(D3DXSPRITE_ALPHABLEND);

  // Draw the sprite.
  if (FAILED(_d3d_sprite->Draw(
          texture_data->texture, data.rectangle, nullptr, nullptr, data.color)))
    DXERROR("Failed to draw the texture.");

  _d3d_sprite->End();

  // Move the world back to identity.
  combined = math::matrix_identity();
  _d3d_sprite->SetTransform(&combined);

  // success.
  return true;
}

std::shared_ptr<scd::component::sprite> scd::texture_manager::create_sprite(
    scd::object& owner,
    sprite_data data,
    bool active) {
  auto component = owner.create_component<component::sprite>(data);

  component->is_rendering(active);

  _sprites.emplace(component);

  return component;
}

std::shared_ptr<scd::bitmap_font> scd::texture_manager::create_font(
    object& owner,
    std::string message,
    scd::bitmap_font font,
    int x,
    int y,
    float scale,
    std::uint32_t color,
    bool active) {
  // Create Component
  std::shared_ptr<scd::bitmap_font> component =
      owner.create_component<bitmap_font>(font, message, x, y, scale, color);

  component->SetActive(active);

  // Insert Component
  _fonts.emplace(component);

  // Return Component
  return component;
}

void scd::texture_manager::render_sprites() {
  for (auto& weak_sprite : _sprites) {
    auto sprite = weak_sprite.lock();
    if (sprite && sprite->is_rendering() && sprite->data().z < 100) {
      sprite->draw(*this);
    }
  }

  for (auto& weak_font : _fonts) {
    auto font = weak_font.lock();
    if (font && font->is_active()) {
      font->draw_string();
    }
  }

  for (auto& weak_sprite : _sprites) {
    auto sprite = weak_sprite.lock();
    if (sprite && sprite->is_rendering() && sprite->data().z >= 100) {
      sprite->draw(*this);
    }
  }
}

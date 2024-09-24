///
/// Project: VenomEngine
/// @file Texture.cc
/// @date Sep, 17 2024
/// @brief 
/// @author Pruvost Kevin | pruvostkevin (pruvostkevin0@gmail.com)
///
#include <venom/common/plugin/graphics/Texture.h>
#include <venom/common/Log.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace venom
{
namespace common
{
Texture::Texture()
    : GraphicsPluginObject()
{
}

Texture::~Texture()
{
}

Texture* Texture::Create()
{
    return GraphicsPlugin::Get()->CreateTexture();
}

vc::Error Texture::LoadImageFromFile(const char* path)
{
    int width, height, channels;
    unsigned char * pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
        vc::Log::Error("Failed to load image from file: %s", path);
        return vc::Error::Failure;
    }
    if (__LoadImage(pixels, width, height, channels) != vc::Error::Success) {
        vc::Log::Error("Failed to load image from file: %s", path);
        return vc::Error::Failure;
    }
    stbi_image_free(pixels);
    return vc::Error::Success;
}
}
}

///
/// Project: VenomEngine
/// @file GraphicsPlugin.h
/// @date Aug, 26 2024
/// @brief 
/// @author Pruvost Kevin | pruvostkevin (pruvostkevin0@gmail.com)
///
#pragma once
#include <venom/common/plugin/Plugin.h>
#include <venom/common/plugin/PluginObject.h>

namespace venom
{
namespace common
{

class GraphicsApplication;
class Model;
class Mesh;
class Texture;

class VENOM_COMMON_API GraphicsPluginObject : public PluginObject
{
public:
    GraphicsPluginObject();
    ~GraphicsPluginObject() override;
};

class VENOM_COMMON_API GraphicsPlugin : public Plugin
{
public:
    enum class GraphicsPluginType
    {
        Vulkan,
        Metal,
        DirectX12
    };

public:
    GraphicsPlugin();
    ~GraphicsPlugin() override;

    static GraphicsPlugin * Get();

    virtual GraphicsApplication * CreateGraphicsApplication() = 0;
    virtual Model * CreateModel() = 0;
    virtual Mesh * CreateMesh() = 0;
    virtual Texture * CreateTexture() = 0;
};

}
}
#include "TextureFactory.hpp"

#include "Resource/ResourceManager.hpp"

#include "Common/FileSystem.hpp"

#include "../Flatbuffers/Texture_generated.h"


namespace Engine
{
    TextureFactory::TextureFactory(const Vulkan::Device& device)
        : device(device) { }

    ResourceId TextureFactory::Create(std::filesystem::path destination, TextureSpec& spec)
    {
        ResourceId id;

        flatbuffers::TextureT texture;

        texture.width = spec.width;
        texture.height = spec.height;
        texture.component = spec.component;
        texture.image = spec.image;

        flatbuffers::FlatBufferBuilder builder(1024);

        auto offset = flatbuffers::Texture::Pack(builder, &texture);

        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });

        return id;
    }

    std::shared_ptr<Texture> TextureFactory::Load(std::filesystem::path source)
    {
        auto file = FileSystem::ReadFile(source);

        auto texture = flatbuffers::GetTexture(file.c_str());

        auto image = texture->image();

        return std::make_shared<Texture>(device, texture->width(), texture->height(), std::vector<uint8_t>{ image->data(), image->data() + image->size() });
    }
}
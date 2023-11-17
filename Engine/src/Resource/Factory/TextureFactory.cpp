#include "TextureFactory.hpp"

#include "Resource/ResourceManager.hpp"

#include "Common/FileSystem.hpp"

#include "../Flatbuffers/Texture_generated.h"

namespace Engine
{
    TextureFactory::TextureFactory(Vulkan::Device& device)
        : device(device) { }

    void TextureFactory::Create(std::filesystem::path destination, Texture& texture)
    {
        flatbuffers::TextureT flatbuffer{};
        flatbuffer.data = texture.GetData();

        auto& mipmaps = texture.GetMipmaps();
        for (auto& mipmap : mipmaps) {
            flatbuffer.mipmaps.push_back({
                mipmap.level,
                mipmap.offset,
                mipmap.extent.width,
                mipmap.extent.height
            });
        }

        flatbuffers::FlatBufferBuilder builder(1024);
        auto offset = flatbuffers::Texture::Pack(builder, &flatbuffer);
        builder.Finish(offset);

        uint8_t* buffer = builder.GetBufferPointer();
        size_t size = builder.GetSize();

        FileSystem::WriteFile(destination, { buffer, buffer + size });
    }

    std::shared_ptr<Texture> TextureFactory::Load(std::filesystem::path source)
    {
        auto file = FileSystem::ReadFile(source);

        auto texture = flatbuffers::GetTexture(file.c_str());

        std::vector<uint8_t> data{
            texture->data()->data(),
            texture->data()->data() + texture->data()->size()
        };

        std::vector<Mipmap> mipmaps;
        for (auto it = texture->mipmaps()->begin(); it != texture->mipmaps()->end(); it++)
        {
            mipmaps.push_back({
                it->level(),
                it->offset(),
                {
                    it->width(),
                    it->height(),
                    1
                }
            });
        }

        return std::make_shared<Texture>(std::move(data), std::move(mipmaps));
    }
}
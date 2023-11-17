#include "TextureImporter.hpp"

#include "Resource/ResourceManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine
{
    TextureImporter::TextureImporter(Vulkan::Device& device)
        : device(device)
    {
    }

    void TextureImporter::ImportDefault(std::filesystem::path path)
    {
        auto texture = LoadDefault(path);

        ResourceManager::Get().CreateResource<Texture>(path.stem(), *texture);
    }

    std::shared_ptr<Texture> TextureImporter::LoadDefault(std::filesystem::path path)
    {
        int width, height, channels;

        auto string = path.string();

        auto pixels = stbi_load(string.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        uint32_t size = width * height * 4;

        std::vector<uint8_t> data{ pixels, pixels + size };
        std::vector<Mipmap> mipmaps{
            Mipmap{
                0,
                0,
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height),
                    1
                }
            }
        };

        stbi_image_free(pixels);

        auto texture = std::make_shared<Texture>(std::move(data), std::move(mipmaps));
        texture->GenerateMipmaps();

        return texture;
    }

    void TextureImporter::ImportCubemap(std::filesystem::path path)
    {
        auto cubemap = LoadCubemap(path);

        ResourceManager::Get().CreateResource<Cubemap>(path.stem(), *cubemap);
    }

    std::shared_ptr<Cubemap> TextureImporter::LoadCubemap(std::filesystem::path path)
    {
        assert(path.extension() == ".hdr");

        int width, height, channels;
        auto pixels = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

        uint32_t size = width * height * 4 * sizeof(float);

        std::vector<uint8_t> data{ reinterpret_cast<uint8_t*>(pixels), reinterpret_cast<uint8_t*>(pixels) + size };
        std::vector<Mipmap> mipmaps{
            Mipmap{
                0,
                0,
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height),
                    1
                }
            }
        };

        stbi_image_free(pixels);

        auto cubemap = std::make_shared<Cubemap>(std::move(data), std::move(mipmaps));
        cubemap->GenerateMipmaps();

        return cubemap;
    }
};
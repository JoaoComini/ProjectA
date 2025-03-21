#include "TextureImporter.h"

#include "Resource/ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine
{
    void TextureImporter::ImportDefault(std::filesystem::path path)
    {
        auto texture = LoadDefault(path);

        ResourceManager::Get().CreateResource<Texture>(path.stem().string(), *texture);
    }

    std::shared_ptr<Texture> TextureImporter::LoadDefault(std::filesystem::path path)
    {
        int width, height, channels;

        auto string = path.string();

        auto pixels = stbi_load(string.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        uint32_t size = width * height * 4;

        std::vector<uint8_t> data{ pixels, pixels + size };
        std::vector mipmaps{
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

    std::shared_ptr<Texture> TextureImporter::LoadDefault(const std::vector<uint8_t>& bytes)
    {
        int width, height, channels;

        auto pixels = stbi_load_from_memory(bytes.data(), bytes.size(), & width, &height, &channels, STBI_rgb_alpha);
        uint32_t size = width * height * 4;

        std::vector<uint8_t> data{ pixels, pixels + size };
        std::vector mipmaps{
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

        ResourceManager::Get().CreateResource<Cubemap>(path.stem().string(), *cubemap);
    }

    std::shared_ptr<Cubemap> TextureImporter::LoadCubemap(std::filesystem::path path)
    {
        assert(path.extension() == ".hdr");

        int width, height, channels;
        auto pixels = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

        uint32_t size = width * height * 4 * sizeof(float);

        std::vector<uint8_t> data{ reinterpret_cast<uint8_t*>(pixels), reinterpret_cast<uint8_t*>(pixels) + size };
        std::vector mipmaps{
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
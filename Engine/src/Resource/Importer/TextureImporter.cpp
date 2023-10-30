#include "TextureImporter.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine
{
    TextureImporter::TextureImporter(Vulkan::Device& device)
        : device(device)
    {
    }

    std::shared_ptr<Texture> TextureImporter::Import(std::filesystem::path path)
    {
        int width, height, channels;

        auto string = path.string();
        stbi_uc* pixels = stbi_load(string.c_str(), &width, &height, &channels, 0);

        uint32_t size = width * height * channels;

        return std::make_shared<Texture>(device, width, height, std::vector<uint8_t>{ pixels, pixels + size }, TextureType::Albedo);
    }
};
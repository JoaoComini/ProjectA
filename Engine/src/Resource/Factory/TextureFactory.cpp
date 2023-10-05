#include "TextureFactory.hpp"

#include "Resource/ResourceManager.hpp"

#include <fstream>

namespace Engine
{
    TextureFactory::TextureFactory(const Vulkan::Device& device)
        : device(device) { }

    std::shared_ptr<Texture> TextureFactory::Create(std::filesystem::path destination, TextureSpec& spec)
    {
        std::ofstream file(destination, std::ios::out | std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<char*>(&spec.width), sizeof(spec.width));
        file.write(reinterpret_cast<char*>(&spec.height), sizeof(spec.height));
        file.write(reinterpret_cast<char*>(&spec.component), sizeof(spec.component));

        size_t size = spec.image.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(size_t));
        file.write(reinterpret_cast<char*>(spec.image.data()), spec.image.size());
        file.close();

        return std::make_shared<Texture>(device, spec.width, spec.height, spec.image);
    }

    std::shared_ptr<Texture> TextureFactory::Load(std::filesystem::path source)
    {
        TextureSpec spec{};

        std::ifstream file(source, std::ios::in | std::ios::binary);
        file.read(reinterpret_cast<char*>(&spec.width), sizeof(spec.width));
        file.read(reinterpret_cast<char*>(&spec.height), sizeof(spec.height));
        file.read(reinterpret_cast<char*>(&spec.component), sizeof(spec.component));

        size_t size{};
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));

        spec.image.resize(size);
        file.read(reinterpret_cast<char*>(spec.image.data()), size);
        file.close();

        return std::make_shared<Texture>(device, spec.width, spec.height, spec.image);
    }
}
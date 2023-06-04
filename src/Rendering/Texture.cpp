#include "Texture.hpp"

#include <stb_image.h>

#include <cmath>

#include "Vulkan/Buffer.hpp"


namespace Rendering
{

	Texture::Texture(const Vulkan::Device & device, std::string path): device(device)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		uint32_t size = width * height * 4;

		auto staging = Vulkan::BufferBuilder()
			.Size(size)
			.Persistent()
			.SequentialWrite()
			.BufferUsage(Vulkan::BufferUsageFlags::STAGING)
			.Build(device);

		staging->SetData(pixels, size);

		VkExtent3D extent = {
			.width = static_cast<uint32_t>(width),
			.height = static_cast<uint32_t>(height),
			.depth = 1
		};

		int mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		image = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_SAMPLED_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_FORMAT_R8G8B8A8_SRGB, extent, VK_SAMPLE_COUNT_1_BIT, mipLevels);
		imageView = std::make_unique<Vulkan::ImageView>(device, *image, mipLevels);

		stbi_image_free(pixels);

		device.SetImageLayout(*image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device.CopyBufferToImage(*staging, *image, width, height);
		device.GenerateMipMaps(*image);

		sampler = std::make_unique<Vulkan::Sampler>(device, static_cast<float>(mipLevels));
	}

	Vulkan::ImageView& Texture::GetImageView() const
	{
		return *imageView;
	}

	Vulkan::Sampler& Texture::GetSampler() const
	{
		return *sampler;
	}
}
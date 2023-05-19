#include "Texture.hpp"

#include "Vulkan/Buffer.hpp"

#include <stb_image.h>

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

		image = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_SAMPLED_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_FORMAT_R8G8B8A8_SRGB, width, height);
		imageView = std::make_unique<Vulkan::ImageView>(device, *image);

		stbi_image_free(pixels);

		device.SetImageLayout(*image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device.CopyBufferToImage(*staging, *image, width, height);
		device.SetImageLayout(*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	Vulkan::ImageView& Texture::GetImageView() const
	{
		return *imageView;
	}
}
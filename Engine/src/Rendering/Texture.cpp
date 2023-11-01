#include "Texture.hpp"
#include "Texture.hpp"

#include <cmath>
#include <vector>

#include "Vulkan/Buffer.hpp"

namespace Engine
{
	Texture::Texture(Vulkan::Device& device, uint32_t width, uint32_t height, std::vector<uint8_t> content)
	{
		uint32_t size = content.size();

		auto staging = Vulkan::BufferBuilder()
			.Size(size)
			.Persistent()
			.SequentialWrite()
			.BufferUsage(Vulkan::BufferUsageFlags::STAGING)
			.Build(device);

		staging->SetData(content.data(), size);

		VkExtent3D extent = {
			.width = static_cast<uint32_t>(width),
			.height = static_cast<uint32_t>(height),
			.depth = 1
		};

		int mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		image = std::make_unique<Vulkan::Image>(device, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_FORMAT_R8G8B8A8_UNORM, extent, VK_SAMPLE_COUNT_1_BIT, mipLevels);
		imageView = std::make_unique<Vulkan::ImageView>(device, *image, mipLevels);

		device.SetImageLayout(*image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		device.CopyBufferToImage(*staging, *image, width, height);
		device.GenerateMipMaps(*image);

		auto properties = device.GetPhysicalDeviceProperties();
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.maxLod = mipLevels;

		sampler = std::make_unique<Vulkan::Sampler>(device, samplerInfo);
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
#include "Texture.hpp"

#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include "Vulkan/Buffer.hpp"
#include "Vulkan/CommandBuffer.hpp"

namespace Engine
{
	Texture::Texture(std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps)
		: data(std::move(data)), mipmaps(std::move(mipmaps))
	{
	}

	Texture::Texture(Texture&& other) noexcept
	{
		data = std::move(other.data);
		mipmaps = std::move(other.mipmaps);
	}

	void Texture::GenerateMipmaps()
	{
		auto extent = GetExtent();

		auto nextWidth = std::max<uint32_t>(1, extent.width / 2);
		auto nextHeight = std::max<uint32_t>(1, extent.height / 2);

		auto texelSize = 4 * GetImageComponentSize();

		auto nextSize = nextWidth * nextHeight * texelSize;

		uint32_t levels = GetMipLevels(nextWidth, nextHeight);

		while (mipmaps.size() < levels)
		{
			// Make space for next mipmap
			auto oldSize = data.size();
			data.resize(oldSize + nextSize);

			auto& prevMipmap = mipmaps.back();
			// Update mipmaps
			Mipmap nextMipmap{};
			nextMipmap.level = prevMipmap.level + 1;
			nextMipmap.offset = oldSize;
			nextMipmap.extent = { nextWidth, nextHeight, 1 };

			// Fill next mipmap memory
			stbir_resize_uint8(
				data.data() + prevMipmap.offset, prevMipmap.extent.width, prevMipmap.extent.height, 0,
				data.data() + nextMipmap.offset, nextMipmap.extent.width, nextMipmap.extent.height, 0, texelSize
			);

			mipmaps.emplace_back(std::move(nextMipmap));

			// Next mipmap values
			nextWidth = std::max<uint32_t>(1, nextWidth / 2);
			nextHeight = std::max<uint32_t>(1, nextHeight / 2);
			nextSize = nextWidth * nextHeight * texelSize;

			if (nextWidth == 1 && nextHeight == 1)
			{
				break;
			}
		}
	}

	uint32_t Texture::GetImageComponentSize() const
	{
		return sizeof(uint8_t);
	}

	uint32_t Texture::GetMipLevels(int halfWidth, int halfHeight) const
	{
		return static_cast<uint32_t>(std::floor(std::log2(std::max(halfWidth, halfHeight)))) + 1;
	}

	void Texture::CreateVulkanResources(Vulkan::Device& device)
	{
		auto& builder = Vulkan::ImageBuilder()
			.Usage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.MipLevels(mipmaps.size());

		PrepareImageBuilder(builder);

		image = builder.Build(device);

		imageView = std::make_unique<Vulkan::ImageView>(device, *image, GetImageViewType());

		auto properties = device.GetPhysicalDeviceProperties();
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f; // make anisotropy configurable
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.maxLod = mipmaps.size();

		sampler = std::make_unique<Vulkan::Sampler>(device, samplerInfo);
	}

	void Texture::PrepareImageBuilder(Vulkan::ImageBuilder& builder)
	{
		builder.Extent(GetExtent());
	}

	VkImageViewType Texture::GetImageViewType() const
	{
		return VK_IMAGE_VIEW_TYPE_2D;
	}

	void Texture::UploadDataToGpu(Vulkan::Device& device)
	{
		uint32_t size = data.size();

		auto staging = Vulkan::BufferBuilder()
			.Size(size)
			.Persistent()
			.SequentialWrite()
			.BufferUsage(Vulkan::BufferUsageFlags::Staging)
			.Build(device);

		staging->SetData(data.data(), size);

		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = image->GetArrayLayers();
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = image->GetMipLevels();

		std::vector<VkBufferImageCopy> regions;
		PrepareBufferCopyRegions(regions);

		device.OneTimeSubmit([&](auto& commandBuffer) {
			commandBuffer.SetImageLayout(*image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
			commandBuffer.CopyBufferToImage(*staging, *image, regions);
			commandBuffer.SetImageLayout(*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		});

		device.ResetCommandPool();

		data.clear();
		data.shrink_to_fit();
	}

	void Texture::PrepareBufferCopyRegions(std::vector<VkBufferImageCopy>& regions)
	{
		uint32_t levels = image->GetMipLevels();

		regions.resize(levels);

		for (size_t level = 0; level < levels; level++)
		{
			auto& mipmap = mipmaps[level];
			auto& region = regions[level];

			region.bufferOffset = mipmap.offset;
			region.bufferRowLength = mipmap.extent.width;
			region.bufferImageHeight = mipmap.extent.height;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageSubresource.mipLevel = mipmap.level;
			region.imageExtent = mipmap.extent;
		}
	}

	Vulkan::ImageView& Texture::GetImageView() const
	{
		return *imageView;
	}

	Vulkan::Sampler& Texture::GetSampler() const
	{
		return *sampler;
	}

	VkExtent3D Texture::GetExtent() const
	{
		return mipmaps[0].extent;
	}

	const std::vector<uint8_t>& Texture::GetData() const
	{
		return data;
	}

	const std::vector<Mipmap>& Texture::GetMipmaps() const
	{
		return mipmaps;
	}
}
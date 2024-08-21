#include "Cubemap.hpp"

#include <stb_image.h>

#include "Vulkan/CommandBuffer.hpp"

#include <cmath>

namespace Engine
{
    Cubemap::Cubemap(std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps)
        : Texture{std::move(data), std::move(mipmaps)}
    {
    }

	Cubemap::Cubemap(Texture&& texture)
		: Texture{std::move(texture)}
	{

	}

	uint32_t Cubemap::GetImageComponentSize() const
	{
		return sizeof(float);
	}

	uint32_t Cubemap::GetMipLevels(int halfWidth, int halfHeight) const
    {
        return static_cast<uint32_t>(std::floor(std::log2(std::max(halfWidth / 4, halfHeight / 3)))) + 1;
    }

    VkImageViewType Cubemap::GetImageViewType() const
    {
        return VK_IMAGE_VIEW_TYPE_CUBE;
    }

    void Cubemap::PrepareImageBuilder(Vulkan::ImageBuilder& builder)
    {
		auto extent = GetFaceExtent();

		builder
			.ArrayLayers(6)
			.Flags(VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT)
			.Format(VK_FORMAT_R32G32B32A32_SFLOAT)
			.Extent(extent);
    }

    void Cubemap::PrepareBufferCopyRegions(std::vector<VkBufferImageCopy>& regions)
    {
        uint32_t layers = image->GetArrayLayers();
        uint32_t levels = image->GetMipLevels();

        regions.resize(levels * layers);

		std::vector<std::pair<uint32_t, uint32_t>> offsets
		{
			{1, 2}, // right
			{1, 0}, // left
			{0, 1}, // top
			{2, 1}, // bottom
			{1, 1}, // front
			{1, 3}, // back
		};

		for (size_t layer = 0; layer < layers; layer++)
		{
			auto& offset = offsets[layer];

			for (size_t level = 0; level < levels; level++)
			{
				auto& mipmap = mipmaps[level];
				auto& region = regions[layer * levels + level];

				auto extent = mipmap.extent;

				extent.width /= 4;
				extent.height /= 3;

				auto offsetX = mipmap.extent.width * extent.height * offset.first * 4 * sizeof(float);
				auto offsetY = extent.width * offset.second * 4 * sizeof(float);

				region.bufferOffset = mipmap.offset + offsetX + offsetY;
				region.bufferRowLength = mipmap.extent.width;
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.baseArrayLayer = layer;
				region.imageSubresource.layerCount = 1;
				region.imageSubresource.mipLevel = mipmap.level;
				region.imageExtent = extent;
			}
		}
    }

	VkExtent3D Cubemap::GetFaceExtent()
	{
		auto extent = GetExtent();

		extent.width /= 4;
		extent.height /= 3;

		return extent;
	}
}
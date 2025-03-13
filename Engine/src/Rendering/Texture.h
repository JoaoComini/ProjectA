#pragma once

#include "Vulkan/Device.h"
#include "Vulkan/Image.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Sampler.h"

#include "Resource/Resource.h"

namespace Engine
{
	struct Mipmap
	{
		uint32_t level{ 0 };

		uint32_t offset{ 0 };

		VkExtent3D extent{ 0, 0, 0 };
	};

	class Texture : public Resource
	{
	public:
		Texture(std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps);
		Texture(Texture&& other) noexcept;

		void GenerateMipmaps();
		void UploadToGpu(Vulkan::Device& device);

		Vulkan::ImageView& GetImageView() const;
		Vulkan::Sampler& GetSampler() const;

		VkExtent3D GetExtent() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Texture;
		}

		static std::string GetExtension()
		{
			return "pares";
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

		const std::vector<uint8_t>& GetData() const;
		const std::vector<Mipmap>& GetMipmaps() const;

	protected:
		virtual uint32_t GetMipLevels(int halfWidth, int halfHeight) const;
		virtual uint32_t GetImageComponentSize() const;
		virtual VkImageViewType GetImageViewType() const;

		virtual void PrepareImageBuilder(Vulkan::ImageBuilder& builder);
		virtual void PrepareBufferCopyRegions(std::vector<VkBufferImageCopy>& regions);

		std::unique_ptr<Vulkan::Image> image;
		std::vector<Mipmap> mipmaps;
		std::vector<uint8_t> data;

	private:
		void CreateVulkanResources(Vulkan::Device& device);

		std::unique_ptr<Vulkan::ImageView> imageView;
		std::unique_ptr<Vulkan::Sampler> sampler;
	};

}
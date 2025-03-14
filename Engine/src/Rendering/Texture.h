#pragma once

#include <cereal/archives/adapters.hpp>

#include "Vulkan/Device.h"
#include "Vulkan/Image.h"
#include "Vulkan/ImageView.h"
#include "Vulkan/Sampler.h"

#include "Resource/Resource.h"

template<typename Archive>
void Serialize(Archive& ar, VkExtent3D& extent)
{
	ar(extent.width, extent.height, extent.depth);
}

namespace Engine
{
	struct Mipmap
	{
		uint32_t level{ 0 };
		uint32_t offset{ 0 };
		VkExtent3D extent{ 0, 0, 0 };
	};

	template<typename Archive>
	void Serialize(Archive& ar, Mipmap& mipmap)
	{
		ar(mipmap.level, mipmap.offset, mipmap.extent);
	}

	class Texture : public Resource
	{
	public:
		Texture() = default;
		Texture(std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps);
		Texture(Texture&& other) noexcept;

		void GenerateMipmaps();
		void UploadToGpu(Vulkan::Device& device);

		[[nodiscard]] Vulkan::ImageView& GetImageView() const;
		[[nodiscard]] Vulkan::Sampler& GetSampler() const;

		[[nodiscard]] VkExtent3D GetExtent() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Texture;
		}

		static std::string GetExtension()
		{
			return "pares";
		}

		[[nodiscard]] ResourceType GetType() const override
		{
			return GetStaticType();
		}

		[[nodiscard]] const std::vector<uint8_t>& GetData() const;
		[[nodiscard]] const std::vector<Mipmap>& GetMipmaps() const;

		template<typename Archive>
		void Save(Archive& ar) const
		{
			ar(mipmaps, data);
		}

		template<typename Archive>
		void Load(Archive& ar)
		{
			ar(mipmaps, data);

			UploadToGpu(cereal::get_user_data<Vulkan::Device>(ar));
		}

	protected:
		[[nodiscard]] virtual uint32_t GetMipLevels(int halfWidth, int halfHeight) const;
		[[nodiscard]] virtual uint32_t GetImageComponentSize() const;
		[[nodiscard]] virtual VkImageViewType GetImageViewType() const;

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
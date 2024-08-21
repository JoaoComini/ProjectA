#pragma once

#include "Texture.hpp"

namespace Engine
{
	class Cubemap : public Texture
	{
	public:
		Cubemap(std::vector<uint8_t>&& data, std::vector<Mipmap>&& mipmaps);

		Cubemap(Texture&& texture);

		static ResourceType GetStaticType()
		{
			return ResourceType::Cubemap;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

	protected:
		uint32_t GetImageComponentSize() const override;
		uint32_t GetMipLevels(int halfWidth, int halfHeight) const override;
		VkImageViewType GetImageViewType() const override;

		void PrepareImageBuilder(Vulkan::ImageBuilder& builder) override;
		void PrepareBufferCopyRegions(std::vector<VkBufferImageCopy>& regions) override;
		
	private:
		VkExtent3D GetFaceExtent();
	};
}

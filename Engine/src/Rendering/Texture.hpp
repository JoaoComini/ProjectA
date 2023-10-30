#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Sampler.hpp"

#include "Resource/Resource.hpp"

namespace Engine
{

	enum TextureType
	{
		Albedo,
		Normal,
		MetallicRoughness,
		Unknown
	};

	class Texture : public Resource
	{
	public:
		Texture(Vulkan::Device& device, uint32_t width, uint32_t height, std::vector<uint8_t> data, TextureType textureType);
		~Texture() = default;

		Vulkan::ImageView& GetImageView() const;
		Vulkan::Sampler& GetSampler() const;

		static ResourceType GetStaticType()
		{
			return ResourceType::Texture;
		}

		virtual ResourceType GetType() const override
		{
			return GetStaticType();
		}

	private:
		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> imageView;
		std::unique_ptr<Vulkan::Sampler> sampler;

		TextureType textureType;
	};

}
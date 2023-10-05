#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Sampler.hpp"

#include "Resource/Resource.hpp"

namespace Engine
{

	class Texture : public Resource
	{
	public:
		Texture(const Vulkan::Device& device, uint32_t width, uint32_t height, std::vector<uint8_t> data);
		~Texture() = default;

		Vulkan::ImageView& GetImageView() const;
		Vulkan::Sampler& GetSampler() const;

	private:
		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> imageView;
		std::unique_ptr<Vulkan::Sampler> sampler;
	};

}
#pragma once

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Sampler.hpp"

namespace Engine
{

	class Texture
	{
	public:
		Texture(const Vulkan::Device& device, std::string path);
		Texture(const Vulkan::Device& device, uint32_t width, uint32_t height, std::vector<uint8_t> data);
		~Texture() = default;

		Vulkan::ImageView& GetImageView() const;
		Vulkan::Sampler& GetSampler() const;

	private:
		void Setup(const Vulkan::Device& device, uint32_t width, uint32_t height, std::vector<uint8_t> content);

		std::unique_ptr<Vulkan::Image> image;
		std::unique_ptr<Vulkan::ImageView> imageView;
		std::unique_ptr<Vulkan::Sampler> sampler;
	};

}
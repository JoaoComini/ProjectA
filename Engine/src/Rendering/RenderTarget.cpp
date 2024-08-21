#include "RenderTarget.hpp"

namespace Engine
{
	struct CompareExtent2D
	{
		bool operator()(const VkExtent2D& lhs, const VkExtent2D& rhs) const
		{
			return !(lhs.width == rhs.width && lhs.height == rhs.height) && (lhs.width < rhs.width && lhs.height < rhs.height);
		}
	};

	RenderTarget& RenderTarget::operator=(RenderTarget&& other) noexcept
	{
		colors = std::move(other.colors);
		depth = std::move(other.depth);
		extent = other.extent;

		return *this;
	}

	VkExtent2D RenderTarget::GetExtent() const
	{
		return extent;
	}
}
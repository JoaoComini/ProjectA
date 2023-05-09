#pragma once


namespace Vulkan
{
	template<typename THandle>
	class Resource
	{
	public:
		Resource(THandle handle = VK_NULL_HANDLE) : handle(handle) {}
		virtual ~Resource() = default;

		inline THandle GetHandle() const
		{
			return handle;
		}

	protected:
		THandle handle;
	};
}

#include "Vulkan/Device.hpp"

#include "Rendering/Texture.hpp"
#include "Rendering/Mesh.hpp"

#include <vector>

namespace Rendering
{

	class Model
	{
	public:
		Model(Vulkan::Device &device, std::string path);

		std::vector<std::unique_ptr<Mesh>> const& GetMeshes() const;

	private:
		std::vector<std::unique_ptr<Mesh>> meshes;
		std::vector<std::unique_ptr<Texture>> textures;

		std::vector<Material> materials;
	};

}

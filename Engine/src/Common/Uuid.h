#pragma once

namespace Engine
{
	class Uuid
	{
	public:
		Uuid();

		explicit Uuid(uint64_t value);

		operator uint64_t() const
		{
			return value; 
		}

		operator bool() const
		{
			return value != 0;
		}

		bool operator==(const Uuid& other) const
		{
			return value == other.value;
		}

		std::string ToString() const
		{
			return std::to_string(value);
		}

		template <class Archive>
		void Serialize(Archive& ar)
		{
			ar(value);
		}

	private:
		uint64_t value;
	};

};

template<>
struct std::hash<Engine::Uuid>
{
	size_t operator()(const Engine::Uuid& uuid) const noexcept
	{
		return uuid;
	}
};;

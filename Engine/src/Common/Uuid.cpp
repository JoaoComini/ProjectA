#include "Uuid.hpp"

namespace Engine
{
    static std::random_device randomDevice;
    static std::mt19937_64 engine(randomDevice());
    static std::uniform_int_distribution<uint64_t> uniformDistribution;

    Uuid::Uuid() : value(uniformDistribution(engine)) { }

    Uuid::Uuid(uint64_t value) : value(value) { }
};



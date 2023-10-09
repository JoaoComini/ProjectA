#include "Core/Application.hpp"
#include "Core/Main.hpp"


namespace Engine
{
    class ProjectA : public Application
    {
    public:
        ProjectA(ApplicationSpec& spec) : Application(spec)
        {
        }
    };

    std::unique_ptr<Application> CreateApplication()
    {
        ApplicationSpec spec
        {
            .name = "ProjectA"
        };

        return std::make_unique<ProjectA>(spec);
    }

}

#pragma once

namespace Engine
{
    class RenderGraphAllocator;
    class RenderGraphCommand;

    struct RenderGraphRecord;

    class RenderGraphResourceHandleBase
    {
    public:
        RenderGraphResourceHandleBase() = default;
        explicit RenderGraphResourceHandleBase(uint32_t id) : id(id) {}
        virtual ~RenderGraphResourceHandleBase() = default;

        uint32_t GetId() const { return id; }
    private:
        uint32_t id;
    };

    template <typename T>
    class RenderGraphResourceHandle : public RenderGraphResourceHandleBase
    {
    public:
        RenderGraphResourceHandle() = default;
        explicit RenderGraphResourceHandle(uint32_t id) : RenderGraphResourceHandleBase(id) {}
    };

    class RenderGraphResource final
    {
    public:
        enum class Type { Imported, Transient };

        template<typename T>
        RenderGraphResource(Type type, T&& resource, const typename T::Descriptor& descriptor)
            : type(type), impl(std::make_unique<Model<T>>(std::forward<T>(resource), descriptor)) { }

        void Allocate(void* allocator) const
        {
            impl->Allocate(allocator);
        }

        void Free(void* allocator) const
        {
            impl->Free(allocator);
        }

        void BeforeRead(void* command, void* info) const
        {
            impl->BeforeRead(command, info);
        }

        void BeforeWrite(void* command, void* info) const
        {
            impl->BeforeWrite(command, info);
        }

        Type GetType() const { return type; }

        void SetLastRecord(RenderGraphRecord* last) { this->last = last; }
        RenderGraphRecord* GetLastRecord() const { return last; }

    private:
        struct Concept
        {
            virtual ~Concept() = default;
            virtual void Allocate(void* allocator) = 0;
            virtual void Free(void* allocator) = 0;
            virtual void BeforeRead(void* command, void* info) = 0;
            virtual void BeforeWrite(void* command, void* info) = 0;
        };

        template<typename T>
        struct Model final : Concept
        {
            Model(T&& resource, const typename T::Descriptor& descriptor)
                : resource(std::forward<T>(resource)), descriptor(descriptor) { }

            void Allocate(void *allocator) override
            {
                auto typed = static_cast<typename T::Allocator*>(allocator);

                resource = typed->Allocate(descriptor);
            }

            void Free(void *allocator) override
            {
                auto typed = static_cast<typename T::Allocator*>(allocator);

                typed->Free(resource, descriptor);
            }

            void BeforeRead(void *command, void *info) override
            {
                auto typedCommand = static_cast<typename T::Command*>(command);
                auto typedInfo = static_cast<typename T::AccessInfo*>(info);

                typedCommand->BeforeRead(resource, descriptor, *typedInfo);
            }

            void BeforeWrite(void *command, void *info) override
            {
                auto typedCommand = static_cast<typename T::Command*>(command);
                auto typedInfo = static_cast<typename T::AccessInfo*>(info);

                typedCommand->BeforeWrite(resource, descriptor, *typedInfo);
            }

        private:
            T resource;
            typename T::Descriptor descriptor;
        };

        Type type;
        RenderGraphRecord* last{ nullptr };
        std::unique_ptr<Concept> impl{ nullptr };
    };

    class RenderGraphResourceAccess final
    {
    public:
        template<typename AccessInfo>
        RenderGraphResourceAccess(const RenderGraphResourceHandleBase handle, const AccessInfo& info)
            : handle(handle), impl(std::make_unique<Model<AccessInfo>>(info)) { }

        void BeforeRead(void* command, RenderGraphResource& resource) const
        {
            impl->BeforeRead(command, resource);
        }

        void BeforeWrite(void* command, RenderGraphResource& resource) const
        {
            impl->BeforeWrite(command, resource);
        }

        RenderGraphResourceHandleBase GetHandle() const { return handle; }

    private:
        struct Concept
        {
            virtual ~Concept() = default;
            virtual void BeforeRead(void* command, RenderGraphResource& resource) = 0;
            virtual void BeforeWrite(void* command, RenderGraphResource& resource) = 0;
        };

        template<typename AccessInfo>
        struct Model final : Concept
        {
            explicit Model(const AccessInfo& info) : info(info) { }

            void BeforeRead(void *command, RenderGraphResource &resource) override
            {
                resource.BeforeRead(command, &info);
            }

            void BeforeWrite(void *command, RenderGraphResource &resource) override
            {
                resource.BeforeWrite(command, &info);
            }

        private:
            AccessInfo info;
        };

        RenderGraphResourceHandleBase handle;
        std::unique_ptr<Concept> impl{ nullptr };
    };
};
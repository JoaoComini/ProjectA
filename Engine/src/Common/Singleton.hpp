#pragma once

class Container
{
public:
	static void Register(std::shared_ptr<void> instance)
	{
		singletons.emplace_front(instance);
	}

	static void TearDown()
	{
		singletons.clear();
	}

private:
	Container() = default;
	inline static std::list<std::shared_ptr<void>> singletons;
};


template <typename T>
class Singleton
{
public:
	template<typename... Args>
	static void Create(Args&&... args)
	{
		auto shared = std::make_shared<T>(std::forward<Args>(args)...);
		
		instance = shared;

		Container::Register(shared);
	}

	template<typename U, typename... Args, typename std::enable_if<std::is_base_of<T, U>::value>::type* = nullptr>
	static void Create(Args&&... args)
	{
		auto shared = std::make_shared<U>(std::forward<Args>(args)...);

		instance = shared;

		Container::Register(shared);
	}

	static T& Get()
	{
		return *instance.lock();
	}

protected:
	Singleton() = default;

	inline static std::weak_ptr<T> instance;
};
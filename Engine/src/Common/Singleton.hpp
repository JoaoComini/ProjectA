#pragma once


template <typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		return instance;
	}

	static void DeleteInstance()
	{
		delete instance;
	}

protected:
	Singleton() = default;

	inline static T* instance = nullptr;
};

#define ENUM_FLAG_OPERATORS(T)\
	inline T operator|(T a, T b)\
	{\
		return static_cast<T>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));\
	}\

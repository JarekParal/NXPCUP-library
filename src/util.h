#pragma once

namespace nxpcup{

template <typename T>
T clamp(T value, T min, T max)
{
	static_assert(std::is_arithmetic<T>::value);
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}

template <typename T>
T abs(const T &v) {
	return v < 0 ? -v : v;
}

template <typename T>
T sign(const T &v)
{
	if (v > 0)
		return 1;
	else if (v < 0)
		return -1;
	else
		return 0;
}

template <typename T>
T pow(T base, uint8_t exp = 2)
{
	T res = base;
	if(exp == 0)
		return 1;
	while(--exp != 0)
		res *= base;
	return res;
}

} // namespace nxpcup


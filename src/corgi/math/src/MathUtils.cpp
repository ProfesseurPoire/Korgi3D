#include <corgi/math/MathUtils.h>

#include <cmath>
#include <stdlib.h>

float corgi::math::to_float(int value)
{
	// if(value> 100'000'000 || value < -100'000'000)
	// {
	// 	log_warning("Losing precision when converting int value to float");
	// }
	return static_cast<float>(value);
}

bool corgi::math::in_range(float value, float left, float right)
{
	return ((value >= left)&& (value <= right));
}

float corgi::math::sign(float value)
{
	if (value >= 0.0f)
	{
		return 1.0f;
	}
	return -1.0f;
}

float corgi::math::clamp(float value, float min, float max)
{
    if (value < min)
        return min;

    if (value > max)
        return max;

    return value;
}

int corgi::math::round(float v)
{
	return (int)std::round(v);
}

double corgi::math::bernstein_polynom(double n, double i, double t)
{
	double val		= factorial(n);
	double underval = (factorial(i) * factorial(n - i));
	val = val / underval;
	val = val * powd(t, i);
	val = val * powd((1.0f - t), n - i);
	return val;
}

double corgi::math::riesenfeld_polynom(double i, double n, double t)
{
	double sum = 0.0f;
	for (int k = 0; k <= (n - i); k++)
	{
		double under = factorial(k) * factorial(n - k + 1);
		double up = powd(-1.0, k) * powd(t + n - i - k, n);
		sum += up / under;
	}
	return ((double)(n + 1) * sum);
}

float corgi::math::inverse_sqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;
	x2 = number * 0.5F;
	y = number;
	i = *(long*)& y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)& i;
	y = y * (threehalfs - (x2 * y * y));
	return y;
}

float corgi::math::randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float corgi::math::randf(float max)
{
	return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / max));
}

float corgi::math::randf(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ( min - max)));
}

float corgi::math::pow(float value, float n)
{
	return ::powf(value, n);
}

double corgi::math::powd(double value, double n)
{
	return ::pow(value,n);
}

float corgi::math::sqrtf(float value)
{
	return ::sqrtf(value);
}

float corgi::math::acosf(float radian)
{
	return ::acos(radian);
}

float corgi::math::asinf(float radian)
{
	return ::asin(radian);
}

float corgi::math::absf(float value)
{
	if (value < 0.0f)
	{
		return value * -1.0f;
	}
	return value;
}

float corgi::math::cosf(float radian)
{
	return ::cosf(radian);
}

float corgi::math::sinf(float radian)
{
	return ::sinf(radian);
}
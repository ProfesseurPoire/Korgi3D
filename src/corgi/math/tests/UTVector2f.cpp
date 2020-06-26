#include <corgi/math/Vec2.h>
#include <corgi/math/MathUtils.h>
#include <corgi/test/test.h>

#include <ostream>

using namespace corgi::math;
using namespace corgi;
using namespace std;
using namespace corgi::test;

static std::ostream& operator<<(std::ostream& o, const Vec2& v)
{
   return o << "{ x : " << v.x << " ; y = " << v.y << " }";
}

class Vec2Fixture : public corgi::test::Test 
{

};

//class Clock
//{
//public:
//
//	void start()
//	{
//		_start = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
//	}
//
//	double elapsed_time()
//	{
//		return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count() - _start;
//	}
//
//private:
//
//	double _start = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();;
//};

//void assignStuff(const std::array<float, 150000> & start, std::array<float, 150000> & destination, int s, int c)
//{
//	for (int i = 0; i < c; ++c)
//	{
//		destination[s + i] = start[s + i];
//	}
//}

//
//TEST_F(Vec2Fixture, speed)
//{
//	//std::vector<float> basev;
//	//std::array<float, 150000> base;
//	//std::array<float, 150000> output;
//
//	//for (int i = 0; i < base.size(); ++i)
//	//{
//	//	basev.push_back((std::rand() % 200) + 1);
//	//	base[i] = (std::rand() % 200) + 1;
//	//}
//
//	//Clock c;
//	//c.start();
//	//output = base;
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//
//	//for (int i = 0; i < base.size(); ++i)
//	//{
//	//	output[i] = base[i];
//	//}
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//	//memcpy(output.data(), base.data(), sizeof(float)*base.size());
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//	//std::copy_n(basev.begin(), basev.size(), output.begin());
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//	//memcpy(output.data(), basev.data(), sizeof(float)*base.size());
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//	//std::copy(basev.begin(), basev.end(), output.begin());
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//	//std::move(basev.begin(), basev.end(), output.begin());
//	//std::cout << c.elapsed_time() << std::endl;
//
//	//c.start();
//	//std::thread p1(assignStuff, base, output, 0, 75000);
//	//std::thread p2(assignStuff, base, output, 75000, 75000);
//
//	//p1.join();
//	//p2.join();
//	//std::cout << c.elapsed_time() << std::endl;
//
//}

TEST_F(Vec2Fixture, check_equals)
{
	assert_that(Vec2(1.0f, 0.0f), equals(Vec2(1.0f, 0.0f)));
}

TEST_F(Vec2Fixture, check_not_equals)
{
	assert_that(Vec2(1.0f, 0.0f), non_equals(Vec2(0.0f, 1.0f)));
}

TEST_F(Vec2Fixture, check_default_constructor)
{
	Vec2 d;

	assert_that(d.x, equals(0.0f));
	assert_that(d.y, equals(0.0f));

	Vec2 dd(0.0f, 1.0f);

	assert_that(dd.x, equals(0.0f));
	assert_that(dd.y, equals(1.0f));
}

TEST_F(Vec2Fixture, check_lerp)
{
	assert_that(Vec2::lerp(Vec2(0.0f, 0.0f),Vec2(0.0f, 1.0f), 0.5f),equals(Vec2(0.0f, 0.5f)));
}

TEST_F(Vec2Fixture, check_addition)
{
	assert_that(Vec2(2.0f, 4.0f) + Vec2(2.0f, 6.0f), equals(Vec2(4.0f, 10.0f)));
}

TEST_F(Vec2Fixture, check_subtraction)
{
	assert_that(Vec2(6.0f, 4.0f) - Vec2(4.0f, 0.0f), equals(Vec2(2.0f, 4.0f)));
}

TEST_F(Vec2Fixture, check_multiplication)
{
	assert_that(Vec2(2.0f, 5.0f)*Vec2(2.0f, 5.0f),equals(Vec2(4.0f, 25.0f)));
}

TEST_F(Vec2Fixture, check_orthogonal_vector)
{
	//assert_that(Vec2(0.0f, 1.0f).orthogonalVector(),equals(Vec2(1.0f, 0.0f)));
}

TEST_F(Vec2Fixture, check_ortho)
{
	//assert_that(Vec2(0.0f, 1.0f).dot(Vec2(1.0f, 0.0f)),equals(0.0f));
	//assert_that(Vec2::areOrtho(Vec2(0.0f, 1.0f),Vec2(1.0f, 0.0f)),equals(true));
}

TEST_F(Vec2Fixture, check_colinearity)
{
	//assert_that(Vec2(0.0f, 1.0f).areColinear(Vec2(0.0f, 1.0f)),equals(true));
}

TEST_F(Vec2Fixture, check_angle)
{
	//assert_that(Vec2(0.0f, 1.0f).angle(Vec2(1.0f, 0.0f)),equals(pi_f / 2.0f));
}

TEST_F(Vec2Fixture, CheckLength)
{
	Vec2 v = Vec2(2.0f, 0.0f);
	assert_that(v.length(), equals(2.0f));
}

TEST_F(Vec2Fixture, check_normalize)
{
	Vec2 v = Vec2(2.0f, 0.0f);
	v = v.normalized();
	//assert_that(v.normalize(), equals(true));
	assert_that(v, equals(Vec2(1.0f, 0.0f)));
}

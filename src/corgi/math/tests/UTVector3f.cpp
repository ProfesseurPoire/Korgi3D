#include <corgi/math/Vec3.h>
#include <corgi/test/test.h>
#include <ostream>

using namespace corgi::math;
using namespace corgi;
using namespace std;
using namespace corgi::test;

static std::ostream& operator<<(std::ostream& o, const Vec3& v)
{
   return o << "{ x : " << v.x << " ; y = " << v.y << " ; z = " << v.z << " }";
}



class Vector3Testing : public Test
{
public:

	Vec3 u;
	Vec3 right		= Vec3(1.0f, 0.0f, 0.0f);
	Vec3 up			= Vec3(0.0f, 1.0f, 0.0f);
	Vec3 one		= Vec3(1.0f, 1.0f, 1.0f);
	Vec3 backward	= Vec3(0.0f, 0.0f, 1.0f);
	Vec3 v			= Vec3(1.0f, 2.0f, 3.0f);
};

TEST_F(Vector3Testing, check_comparison)
{
	assert_that(v, equals(v));
	assert_that(v, non_equals(u));
}

TEST_F(Vector3Testing, check_cross_product)
{
	assert_that(right.cross(up), equals(backward));
}

TEST_F(Vector3Testing, check_lerp)
{
	//assert_that(Vec3::lerp(u, one, 0.5f), equals(Vec3(0.5f, 0.5f, 0.5f)));
}

TEST_F(Vector3Testing, Length)
{
	assert_that(Vec3(4.0f, 4.0f, 2.0f).length(), equals(6.0f));
}

TEST_F(Vector3Testing, check_sqrt_length)
{
	assert_that(Vec3(2.0f, 3.0f, 5.0f).sqrt_length(), equals(38.0f));
}

TEST_F(Vector3Testing, check_substraction)
{
	assert_that(one - one, equals(Vec3(0.0f, 0.0f, 0.0f)));
}

TEST_F(Vector3Testing, check_unary_substraction)
{
	assert_that(-one, equals(Vec3(-1.0f, -1.0f, -1.0f)));
}

TEST_F(Vector3Testing, Addition)
{
	assert_that(right + up, equals(Vec3(1.0f, 1.0f, 0.0f)));
}

TEST_F(Vector3Testing, Mutliplication)
{
	assert_that(v*v, equals(Vec3(1.0f, 4.0f, 9.0f)));
	assert_that(v*2.0f, equals(Vec3(2.0f, 4.0f, 6.0f)));
}

TEST_F(Vector3Testing, division_arithmetic_operator)
{
	assert_that(Vec3(10.0f, 8.0f, 6.0f) / 2.0f, equals(Vec3(5.0f, 4.0f, 3.0f)));
}

TEST_F(Vector3Testing, multiplication_assignment_operator)
{
	assert_that(Vec3(2.0f, 3.0f, 4.0f) *= Vec3(2.0f, 3.0f, 4.0f),
		equals(Vec3(4.0f, 9.0f, 16.0f)));

	assert_that(Vec3(2.0f, 3.0f, 4.0f) *= 2.0f,
		equals(Vec3(4.0f, 6.0f, 8.0f)));
}

TEST_F(Vector3Testing, assition_assignment_operator)
{
	assert_that(Vec3(2.0f, 3.0f, 5.0f) += Vec3(1.0f, 3.0f, 2.0f),
		equals(Vec3(3.0f, 6.0f, 7.0f)));
}

TEST_F(Vector3Testing, substraction_assignment_operator)
{
	assert_that(Vec3(2.0f, 3.0f, 5.0f) -= Vec3(2.0f, 3.0f, 5.0f),
		equals(Vec3(0.0f, 0.0f, 0.0f)));
}

TEST_F(Vector3Testing, division_assignment_operator)
{
	assert_that(Vec3(2.0f, 4.0f, 6.0f) /= 2.0f,
		equals(Vec3(1.0f, 2.0f, 3.0f)));
}

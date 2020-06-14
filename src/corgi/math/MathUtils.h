#pragma once

namespace corgi
{
	namespace math
	{
		const double pi = 3.141592653589793;
		const float  pi_f = 3.1415927f;

		/*!
		 * @brief	Returns a random value between 0.0f and 1.0f
		 */
		float randf();
		
		/*!
		 * @brief	Returns a random value between 0 and max
		 */
		float randf(float max);

		/*!
		 * @brief	Returns a random value between min and max
		 */
		float randf(float min, float max);
		

		float pow(float value, float n);
		double powd(double value, double n);
		float sqrtf(float value);
		float absf(float value);

		float cosf(float radian);
		float sinf(float radian);

		bool in_range(float value, float min, float max);

		// Return 1.0f if value >=0.0f, -1.0f otherwise
		float sign(float value);

		float acosf(float radian);
		float asinf(float radian);

		int round(float value);

		float clamp(float value, float min, float max);

		// Mainly used to compute vector's normals
		// Quake3 trick
		float inverse_sqrt(float number);

		/*!
		 *	@brief 	Convert an integer value to float value
		 *			Raise a warning if we lose precision
		 */
		float to_float(int value);





		/*!
			@brief Compute the coeficients values of the AB line
			Computes the values of the line equation ax+by+c = 0 using
			a and b
			@param a First point defining the line
			@param b Second point used to define the line
			@param Returns a Vector3 storing the coefficients,
			where x = a, y = b and z = c;
		*/
		//template<class T>
		//constexpr static detail::Vector3<T> line_coef(detail::Vector3<T> a, detail::Vector3<T> b)
		//{
		//    return detail::Vector3<T>((b.y - a.y), -(b.x - a.x), -(a.x * b.y) + (b.x * a.y));
		//}

		//! @brief  Notation : n!
		template<class T>
		T factorial(T n)
		{
			if (n > static_cast<T>(1))
			{
				return n * factorial(n - static_cast<T>(1));
			}
			return static_cast<T>(1);
		}


		// TODO Testing this stuff would be nice too
		// TODO	Potentially remove that from here and move it to where it'd be actually used,
		//		like Curve code		
		double bernstein_polynom(double n, double i, double t);
		double riesenfeld_polynom(double i, double n, double t);

		/*!
			@brief  Convert the input value from interval [in_max;in_out] to [out_min;out_max]

			This function will compute a t real between 0.0 and 1.0 that correspond to
			the relative position of the v parameter inside the min interval.
			Then, this t will be used to compute the value of the t parameter
			inside the output interval

			A warning though, this function assume the parameters are correct

			## Example

			```cpp
			double t = convert_interval(1.0, 0.0, 2.0, 0.0, 4.0)
			```

			Would return 2

			Parameter @ref v is equals to 1. (2.0-1.0)/(2.0-0.0) equals 0.5.
			Then, the length of output_max is equal to 4, so, 0.0+ 0.5*4 - 2

		*/
		template<class T>
		constexpr T convert_interval(T v, T in_min, T in_max, T out_min, T out_max)
		{
			v = (v - in_min) / (in_max - in_min);
			return (out_min + (out_max - out_min) * v);
		}


		//
		//template<class T>
		//constexpr static bool segment_intersect(Vector2f a, 
		//                                        Vector2f b, 
		//                                        Vector2f c,
		//                                        Vector2f d,
		//                                        Vector2f & intersection)
		//{
		//    // First, we make sure that ab and cd aren't colinear
		//    //detail::Vector2<T> b = a2 - a1;
		//    //detail::Vector2<T> d = b2 - b1;
		//    //
		//    //// this is the determinant
		//    //T determinant = (b.x * d.y) - (b.y * d.x);
		//    //
		//    //// if the dot product equals 0, it means a and b are parallel
		//    //// In that case, no collision can ever happen
		//    //if (determinant == 0)
		//    //{
		//    //    return false;
		//    //}
		//    //
		//    //// We get the vector's equation
		//    //detail::Vector2<T> c = b1 - a1;
		//    //
		//    //T t = (c.x * d.y - c.y * d.x) / determinant;
		//    //
		//    //if (t < 0 || t > 1)
		//    //{
		//    //    return false; // The segments doesn't intersect
		//    //}
		//    //
		//    //T u = (c.x * b.y - c.y * b.x) / determinant;
		//    //
		//    //if (u < 0 || u > 1)
		//    //{
		//    //    return false; // The segments doesn't intersect
		//    //}
		//    //
		//    //intersection = a1 + b*t;
		//    return true;
		//}

		// This is the SAT algorithm
		////bool tmath::Intersect(std::vector<vec3>& shape1, std::vector<vec3>& axis1, std::vector<vec3>& shape2, std::vector<vec3>& axis2) {
		////
		////    // Ces valeurs concernent le MTV, (Minimum translation vector)
		////    // Elles permettent de potentiellement pousser une forme au mieux pour éviter que
		////    // les formes ne se rentrent dedans 
		////
		////    vec3 smallestaxis;
		////    float smallestmagnitude = 99999999999.0f;
		////
		////    return false;
		////
		////    // La première étape consiste à récupérer l'intégralité des axes à tester
		////    // Une normale d'un arrête de la forme correspond à un axe à traiter
		////
		////    std::vector<vec3> axis(axis1);
		////    axis.insert(axis.end(), axis2.begin(), axis2.end());
		////
		////    std::vector<Vec2f> projections1;
		////    std::vector<Vec2f> projections2;
		////
		////    for (size_t i = 0; i < axis1.size(); i++) 
		////    {
		////        projections1.push_back(project_points(shape1, axis[i]));    // just use 
		////        projections2.push_back(project_points(shape1, axis[i]));
		////
		////        if (!Overlap(projections1[i].x, projections1[i].y, projections2[i].x, projections2[i].y)) {
		////            return false;
		////        }
		////        else
		////        {
		////            float overlapvalue = Overlap(projections1[i].x, projections1[i].y, projections2[i].x, projections2[i].y);
		////
		////            if (overlapvalue < smallestmagnitude) {
		////                smallestmagnitude = overlapvalue;
		////                smallestaxis = axis[i];
		////            }
		////        }
		////    }
		////    return true;
		////}

		/*!
			@brief  Returns true if the value overlaps
			Used by the SAT algorithm after projecting points on an axis
		*/
		////bool tmath::Overlap(float valmin1, float valmax1, float valmin2, float valmax2) {
		////    if (valmin2 >= valmin1 && valmin2 <= valmax1)
		////        return true;
		////    if (valmax2 >= valmin1 && valmax2 <= valmax1)
		////        return true;
		////    if (valmin1 >= valmin2 && valmin1 <= valmax2)
		////        return true;
		////    if (valmax1 >= valmin2 && valmax1 <= valmax2)
		////        return true;
		////    return false;
		////}

		// Used by the sat
		////Vec2f tmath::project_points(std::vector<vec3>& points, vec3 axis)
		////{
		////    float min = vec3::dot(points[0], axis);
		////    float max = min;
		////    float val = 0.0f;
		////
		////    for (size_t i = 0; i < points.size(); i++)
		////    {
		////        val = vec3::dot(points[i], axis);
		////        min = std::min(val, min);
		////        max = std::max(val, max);
		////    }
		////    return Vec2f(min, max);
		////}
	}
}
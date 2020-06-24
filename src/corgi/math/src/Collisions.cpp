#include <corgi/math/Collisions.h>

namespace corgi::math
{
	Vec3 get_triangle_normal(const Vec3& a, const Vec3& b, const Vec3& c)
	{
		float b_ax = b.x - a.x;
		float b_ay = b.y - a.y;
		float b_az = b.z - a.z;

		float c_ax = c.x - a.x;
		float c_ay = c.y - a.y;
		float c_az = c.z - a.z;

		return Vec3(
			(b_ay * c_az) - (b_az * c_ay),
			(b_az * c_ax) - (b_ax * c_az),
			(b_ax * c_ay) - (b_ay * c_ax)
		).normalized();


		//return (b - a).cross(c - a).normalized();
	}

	//TODO : I don't think I can significantly speed that up anymore
	//		Maybe one way to make the raycast faster would be
	//		to use threads where multiple raycast are computed at the same time?
	bool intersect_with_collider(
		int offset,
		int vertex_size,
		const Matrix& model_matrix,
		const Matrix& inverse,
		const std::vector<unsigned int>&indexes,
		const float* vertices,
		const std::vector<Vec3>& normals,
		const Ray& ray,
		Vec3& intersection_point, 
		Vec3& intersection_normal)
	{
		bool does_intersect					= false;
		bool is_first_intersection_point	= false;

		// We convert the ray into the model's space by using the inverse matrix
		// Why? Because the ray is only 2 Vec3.The mesh could have
		// hundreds of normals and vertices to transform in world space
		const Vec3 start		= inverse * ray.start;
		const Vec3 direction	= inverse * ray.direction - inverse * Vec3::zero;

		// We loop through every triangle
		for (size_t i = 0; i < indexes.size(); i += 3)
		{
			// We first check if the plane and the ray are collinear by
			// comparing the plane's normal with the ray's direction
			const Vec3& normal = normals[i / 3] ;

			// I don't remember why collinearity is checked that way here
			float denom = direction.x * normal.x + direction.y * normal.y + direction.z * normal.z;

			// If denom equals 0, both are collinear and so we can skip
			// the current triangle
			if (denom == 0)
				continue;

			// we use the ax+by+cz+d = 0 plane equation. We use 1 point
			// of the plane to compute the equation

			Vec3 posA(&vertices[indexes[i] * vertex_size + offset]);
			float D = -(posA.x * normal.x + posA.y * normal.y + posA.z * normal.z);

			// Reusing denom variable, was called t before
			denom = (-start.x * normal.x - start.y * normal.y - start.z * normal.z - D)/denom;

			// TODO : This is a big hack I'm really not sure it actually works lol
			// Well apparently it does work, this has been added to allow me to send negative 
			// length value, although I could just flip the direction when I notice that length is negative too
			if(ray.length>0.0f)
			{
				// We skip if the point is not inside the ray
				if (denom <= 0)
					continue;

				if (denom >= ray.length)
					continue;
			}
			else
			{
				if (denom >= 0)
					continue;

				if (denom <= ray.length)
					continue;
			}

			// P being the point that intersect between the plane
			// and the ray
			Vec3 P( start + (direction * denom));

			// We now check if the intersection point is inside the triangle

			Vec3 posB(&vertices[indexes[i + 1] * vertex_size + offset]);
			Vec3 posC(&vertices[indexes[i + 2] * vertex_size + offset]);
			
			if (Vec3::point_in_triangle(posA, posB, posC, P))
			{
				if (!is_first_intersection_point)
				{
					is_first_intersection_point = true;
					intersection_point  = P;
					intersection_normal = normal;
				}
				else
				{
					if (denom < (intersection_point - start).length())
					{
						intersection_point  = P;
						intersection_normal = normal;
					}
				}
				does_intersect = true;
			}
		}

		if(does_intersect)
		{
			intersection_normal = model_matrix * intersection_normal - model_matrix * Vec3::zero;
			intersection_point = model_matrix * intersection_point;
		}
		return does_intersect;
	}
}
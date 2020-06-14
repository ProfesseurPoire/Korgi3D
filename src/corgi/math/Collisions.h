#pragma once

#include <corgi/math/Vec3.h>
#include <corgi/math/Ray.h>
#include <corgi/math/Matrix.h>
#include <vector>
#include <algorithm>

namespace corgi::math
{
	// TODO : Maybe really have a Triangle class with static function for this stuff
	// I could at least put that and "point_in_triangle" there
	Vec3 get_triangle_normal(const Vec3& a, const Vec3& b, const Vec3& c);

	// TODO : put this in Ray
	// TODO : All Colliders must have a set of triangles points for raycasting
	//  Inspecte tout les triangles d'un mesh pour trouver le point d'intersection le plus proche (s'il existe)
	//  Renvoie true si le rayon rentre en collision avec le mesh. On r�cup�re le point d'intersection
	//  � partir de dernier argument pass� en r�f�rence


	// Used by the sat
	static Vec3 project_points(std::vector<Vec3>& points, const Vec3& axis)
	{
	    float mi = points[0].dot(axis);
	    float ma = mi;
	    float val = 0.0f;
		
	    for (size_t i = 1; i < points.size(); i++)
	    {
			val = points[i].dot(axis);
			if (val < mi)
				mi = val;

			if (val > ma)
				ma = val;
	    }
	    return Vec3(mi, ma, 0.0f);
	}

	bool intersect_with_collider(
		int offset,
		int vertex_size,
		const Matrix& model_matrix,
		const Matrix& inverse_matrix,
		const std::vector<unsigned int>& indexes,
		const float* vertices,
		const std::vector<Vec3>& normals,
		const Ray& ray,
		Vec3& intersection_point, Vec3& intersection_normal );

	

	// We also assume the vertices are GL_TRIANGLES
	static bool intersect_with_mesh(
		const std::vector<unsigned int>& indexes,
		const float* vertices,				// Mesh's vertices
		int offset,							// offset for the position attribute inside the vertice list
		int vertex_size,					// Size of 1 vertex, not in bytes but in float
		int vertex_count,
		Matrix model_matrix,
		Ray ray,
		Vec3& intersectionPoint,
		Vec3& intersectionNormal
	)
	{
		bool doesIntersect = false;
		bool isClosestPointSet = false;

		for (size_t i = 0; i < indexes.size() ; i += 3)
		{
			Vec3 posA = model_matrix * Vec3(&vertices[indexes[i]   * vertex_size + offset]);
			Vec3 posB = model_matrix * Vec3(&vertices[indexes[i+1] * vertex_size + offset]);
			Vec3 posC = model_matrix * Vec3(&vertices[indexes[i+2] * vertex_size + offset]);

			// On a besoin de la normale pour trouver l'�quation du plan du triangle
			// Rappel, �quation du plan : ax+by+cz+d=0
			Vec3 normal = get_triangle_normal(posA, posB, posC);

			float A = normal.x;
			float B = normal.y;
			float C = normal.z;

			// � partir de la normale, on connait les composantes a,b et c de l'�quation ax+by+cz+d=0 du plan
			// On doit d�sormais trouver la composant D; Pour se faire, on s�lectionne un point de notre plan pour 
			// remplir x,y et c et trouver d

			float D = -(posA.x * A + posA.y * B + posA.z * C);

			// On calcule maintenant les composants de l'�quation param�trique de la droite 
			// � partir de son vecteur directeur

			float denom = ray.direction.x * A + ray.direction.y * B + ray.direction.z * C;

			// non  Colinarit�

			if (denom != 0)
			{
				float t = -ray.start.x * A - ray.start.y * B - ray.start.z * C - D;
				t = t / denom;

				Vec3 P
				(
					ray.start.x + t * ray.direction.x,
					ray.start.y + t * ray.direction.y,
					ray.start.z + t * ray.direction.z
				);

				// We skip if the point is not inside the ray
				if ( t <= 0 )
					continue;

				if (t >= ray.length)
					continue;

				if (Vec3::point_in_triangle(posA, posB, posC, P))
				{
					if (!isClosestPointSet)
					{
						isClosestPointSet = true;
						intersectionPoint = P;
						intersectionNormal = normal;
					}
					else
					{
						if ((P - ray.start).sqrt_length() < (intersectionPoint - ray.start).sqrt_length())
						{
							intersectionPoint = P;
							intersectionNormal = normal;
						}
					}
					doesIntersect = true;
				}
			}
		}
		return doesIntersect;
	}


	// TODO : Maybe reuse the Vertex class or something

	/*!
	 *
	 * @brief	Returns the normals of every triangle inside the given mesh
	 * 
	 *			This function assume that the every 3 vertex we have a triangle
	 *
	 * @param model_matrix		Model matrix
	 * @param vertices			Array containing the vertex information
	 * @param vertex_size		How many float per vertex
	 * @param position_offset	Where the position information/attribute is store in a vertex
	 * @param vertex_count		How many vertex there is inside the array
	 */
	static void get_axes(
		const std::vector<unsigned int>& indexes,
		std::vector<Vec3>& normals,
		const Matrix& model_matrix,
		const float*	vertices,			
		int		vertex_size,		
		int		position_offset,	
		int		vertex_count		
	)
	{
		// That's the maximal number of normals that could be used
		// at least I avoid memory allocations, and push back would explode the 
		// vector size anyways since it's *2 each time I think

		bool found = false;

		for(size_t i=0; i< indexes.size(); i+=3)
		{
			Vec3 normal = get_triangle_normal(
				model_matrix * Vec3(&vertices[indexes[i] * vertex_size + position_offset]),
				model_matrix * Vec3(&vertices[indexes[i+1] * vertex_size + position_offset]),
				model_matrix * Vec3(&vertices[indexes[i+2] * vertex_size + position_offset]));

			// We only add the normal if it doesn't already exist 
			found = false;
			for(auto& norm : normals)
			{
				if(norm.equal(normal, 0.0001f) || norm.equal(-normal, 0.0001f))
					found = true;
			}

			if (!found)
				normals.push_back(normal);
		}
	}

	static void extract_position_attribute(
		std::vector<Vec3>& positions,
		Matrix		model_matrix,
		const float*		vertices,
		int			vertex_size,
		int			position_offset,
		int			vertex_count
)
	{
		positions.reserve(vertex_count);

		for (int i = 0; i < vertex_count; ++i)
			positions.push_back(model_matrix * Vec3(&vertices[i * vertex_size + position_offset]));
	}

	static bool Overlap(float valmin1, float valmax1, float valmin2, float valmax2)
	{
		if (valmin2 >= valmin1 && valmin2 <= valmax1)
			return true;

		if (valmax2 >= valmin1 && valmax2 <= valmax1)
			return true;

		if (valmin1 >= valmin2 && valmin1 <= valmax2)
			return true;

		if (valmax1 >= valmin2 && valmax1 <= valmax2)
			return true;

		return false;
	}

	static float GetOverlap(float valmin1, float valmax1, float valmin2, float valmax2)
	{
		if (valmin2 >= valmin1 && valmin2 <= valmax1)
		{
			float max = valmax2;
			if (valmax1 > valmax2)
				max = valmax1;
			return max - valmin2;
		}

		if (valmax2 >= valmin1 && valmax2 <= valmax1)
		{
			float min = valmin1;
			if (valmin2 > valmin1)
				min = valmin2;
			return valmax2 - min;
		}
		return 0.0f;
	}

	static bool intersect_3D(
		Matrix model_matrix_a,
		const std::vector<unsigned int>& index1,
		const float* vertices_a,
		int		vertex_size_a,
		int		position_offset_a,
		int		vertex_count_a,

		Matrix model_matrix_b,
		const float* vertices_b,
		const std::vector<unsigned int>& index2,
		int vertex_size_b,
		int position_offset_b,
		int vertex_count_b)
	{
	
		std::vector<Vec3> axes;
		axes.reserve(vertex_count_a + vertex_count_b);

		// I just have to compute the normals here otherwise I can't use the model matrix
		get_axes(index1, axes, model_matrix_a, vertices_a, vertex_size_a, position_offset_a, vertex_count_a);
		get_axes(index2, axes, model_matrix_b, vertices_b, vertex_size_b, position_offset_b, vertex_count_b);

		// TODO :	This is not a really performant operation, but at least it's easier to wrap
		//			my mind around that. Maybe later on, once this works, optimize it
		std::vector<Vec3> v_a;
		std::vector<Vec3> v_b;

		extract_position_attribute(v_a, model_matrix_a, vertices_a, vertex_size_a, position_offset_a, vertex_count_a);
		extract_position_attribute(v_b, model_matrix_b, vertices_b, vertex_size_b, position_offset_b, vertex_count_b);

		// Ces valeurs concernent le MTV, (Minimum translation vector)
		// Elles permettent de potentiellement pousser une forme au mieux pour �viter que
		// les formes ne se rentrent dedans 

		Vec3 smallestaxis;
		float smallestmagnitude = 99999999999.0f;

		// Shape contient d�j� les axes � tester

		//std::vector<Vec3> axes = new Vector3[shape1.axes_.Length + shape2.axes_.Length + (shape1.axes_.Length * shape2.axes_.Length)];
		//shape1.axes_.CopyTo(axes, 0);
		//shape2.axes_.CopyTo(axes, shape1.axes_.Length);

		std::vector<Vec3> shape1projections;
		shape1projections.reserve(axes.size());
		std::vector<Vec3> shape2projections;
		shape2projections.reserve(axes.size());

		for(auto& axe : axes)
		{
			shape1projections.push_back(project_points(v_a, axe));
			shape2projections.push_back(project_points(v_b, axe));

			if (!Overlap(shape1projections.back().x, shape1projections.back().y, shape2projections.back().x, shape2projections.back().y))
				return false;

			const float overlapvalue = GetOverlap(
				shape1projections.back().x, 
				shape1projections.back().y,
				shape2projections.back().x,
				shape2projections.back().y);

			if (overlapvalue < smallestmagnitude)
			{
				smallestmagnitude = overlapvalue;
				smallestaxis = axe;
			}
		}
		return true;
	}
}
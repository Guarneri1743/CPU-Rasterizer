#include "PrimitiveFactory.hpp"
#include "Material.hpp"
#include "Sampling.hpp"

namespace CpuRasterizer {
	Vertex cube_vertices[36] = {
		Vertex(tinymath::vec4f(-0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, -1.0f), tinymath::vec2f(0.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, -1.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, -1.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, -1.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(-0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, -1.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, -1.0f), tinymath::vec2f(0.0f,  0.0f)),

		Vertex(tinymath::vec4f(-0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, 1.0f), tinymath::vec2f(0.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, 1.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, 1.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, 1.0f), tinymath::vec2f(1.0f, 1.0f)),
		Vertex(tinymath::vec4f(-0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, 1.0f), tinymath::vec2f(0.0f, 1.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f,  0.0f, 1.0f), tinymath::vec2f(0.0f,  0.0f)),

		Vertex(tinymath::vec4f(-0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(-1.0f,  0.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(-0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(-1.0f,  0.0f,  0.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(-1.0f,  0.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(-1.0f,  0.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(-1.0f,  0.0f,  0.0f), tinymath::vec2f(0.0f,  0.0f)),
		Vertex(tinymath::vec4f(-0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(-1.0f,  0.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),

		Vertex(tinymath::vec4f(0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(1.0f,  0.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(1.0f,  0.0f,  0.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(1.0f,  0.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(1.0f,  0.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(1.0f,  0.0f,  0.0f), tinymath::vec2f(0.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(1.0f,  0.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),

		Vertex(tinymath::vec4f(-0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f, -1.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f, -1.0f,  0.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f, -1.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f, -1.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f, -1.0f,  0.0f), tinymath::vec2f(0.0f,  0.0f)),
		Vertex(tinymath::vec4f(-0.5f, -0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f, -1.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),

		Vertex(tinymath::vec4f(-0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f, 1.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f, 1.0f,  0.0f), tinymath::vec2f(1.0f,  1.0f)),
		Vertex(tinymath::vec4f(0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f, 1.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(0.5f, 0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f, 1.0f,  0.0f), tinymath::vec2f(1.0f,  0.0f)),
		Vertex(tinymath::vec4f(-0.5f,  0.5f,  0.5f, 1.0f), tinymath::vec3f(0.0f, 1.0f,  0.0f), tinymath::vec2f(0.0f,  0.0f)),
		Vertex(tinymath::vec4f(-0.5f,  0.5f, -0.5f, 1.0f), tinymath::vec3f(0.0f, 1.0f,  0.0f), tinymath::vec2f(0.0f,  1.0f)),
	};

	uint32_t cube_indices[36] = {
		0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35,
	};

	Vertex plane_vertices[8] = {
		Vertex(tinymath::vec4f(1, 0, 1, 1), tinymath::kVec3fUp,  tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1, 0, -1, 1), tinymath::kVec3fUp,  tinymath::vec2f(0.0f, 1.0f)),
		Vertex(tinymath::vec4f(-1, 0, -1, 1), tinymath::kVec3fUp, tinymath::vec2f(1.0f, 1.0f)),
		Vertex(tinymath::vec4f(-1, 0, -1, 1), tinymath::kVec3fUp, tinymath::vec2f(1.0f, 1.0f)),
		Vertex(tinymath::vec4f(-1, 0, 1, 1), tinymath::kVec3fUp, tinymath::vec2f(1.0f, 0.0f)),
		Vertex(tinymath::vec4f(1, 0, 1, 1), tinymath::kVec3fUp,  tinymath::vec2f(0.0f, 0.0f))
	};

	uint32_t plane_indices[6] = {
		0, 1, 2, 3, 4, 5
	};

	Vertex sky_box_vertices[] = {
		Vertex(tinymath::vec4f(-1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),

		Vertex(tinymath::vec4f(-1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),

		Vertex(tinymath::vec4f(1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),

		Vertex(tinymath::vec4f(-1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),

		Vertex(tinymath::vec4f(-1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f,  1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),

		Vertex(tinymath::vec4f(-1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f, -1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(-1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f)),
		Vertex(tinymath::vec4f(1.0f, -1.0f,  1.0f, 1.0f), tinymath::kVec3fUp, tinymath::vec2f(0.0f, 0.0f))
	};

	inline tinymath::vec3f cal_tangent(const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		auto edge1 = v2.position - v1.position;
		auto edge2 = v3.position - v1.position;
		auto duv1 = v2.uv - v1.uv;
		auto duv2 = v3.uv - v1.uv;
		float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
		tinymath::vec3f tangent;
		tangent.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
		tangent.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
		tangent.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);
		return tinymath::normalize(tangent);
	}

	constexpr float kDefaultSpherePrecision = 32.0f;
	constexpr float kDefaultPhiStep = TWO_PI / kDefaultSpherePrecision;
	constexpr float kDefaultThetaStep = PI / kDefaultSpherePrecision;

	std::shared_ptr<Model> PrimitiveFactory::sphere(std::shared_ptr<Material> material)
	{
		std::vector<Vertex> vert;
		std::vector<size_t> ind;

		for (float phi = 0.0f; phi < TWO_PI; phi += kDefaultPhiStep)
		{
			for (float theta = -HALF_PI; theta < HALF_PI; theta += kDefaultThetaStep)
			{
				float phi0 = phi;
				float phi1 = phi + kDefaultPhiStep;

				float theta0 = theta;
				float theta1 = theta + kDefaultThetaStep;

				auto pos0 = radian_to_spherical_coord(theta0, phi0);
				auto uv0 = spherical_coord_to_uv(pos0);

				auto pos1 = radian_to_spherical_coord(theta1, phi0);
				auto uv1 = spherical_coord_to_uv(pos1);

				auto pos2 = radian_to_spherical_coord(theta1, phi1);
				auto uv2 = spherical_coord_to_uv(pos2);

				auto pos3 = radian_to_spherical_coord(theta0, phi1);
				auto uv3 = spherical_coord_to_uv(pos3);

				auto v0 = Vertex(pos0, pos0, uv0);
				v0.color = tinymath::kColorBlack;
				auto v1 = Vertex(pos1, pos1, uv1);
				v1.color = tinymath::kColorRed;
				auto v2 = Vertex(pos2, pos2, uv2);
				v2.color = tinymath::kColorGreen;
				auto v3 = Vertex(pos3, pos3, uv3);
				v3.color = tinymath::kColorBlue;

				ind.push_back(vert.size());
				vert.push_back(v0);
				ind.push_back(vert.size());
				vert.push_back(v1);	
				ind.push_back(vert.size());
				vert.push_back(v2);
				ind.push_back(vert.size());
				vert.push_back(v0);
				ind.push_back(vert.size());
				vert.push_back(v2);
				ind.push_back(vert.size());
				vert.push_back(v3);
			}
		}

		return Model::load_raw(vert, ind, material);
	}

	std::shared_ptr<Model> PrimitiveFactory::plane(std::shared_ptr<Material> material)
	{
		auto tangent1 = cal_tangent(plane_vertices[0], plane_vertices[1], plane_vertices[2]);
		auto tangent2 = cal_tangent(plane_vertices[5], plane_vertices[3], plane_vertices[4]);
		plane_vertices[0].tangent = tangent1;
		plane_vertices[1].tangent = tangent1;
		plane_vertices[2].tangent = tangent1;
		plane_vertices[3].tangent = tangent2;
		plane_vertices[4].tangent = tangent2;
		plane_vertices[5].tangent = tangent2;
		const std::vector<Vertex> vert(plane_vertices, plane_vertices + 6);
		const std::vector<size_t> ind(plane_indices, plane_indices + 6);
		return Model::load_raw(vert, ind, material);
	}

	std::shared_ptr<Model> PrimitiveFactory::cube(std::shared_ptr<Material> material)
	{
		const std::vector<Vertex> vert(cube_vertices, cube_vertices + 36);
		const std::vector<size_t> ind(cube_indices, cube_indices + 36);
		return Model::load_raw(vert, ind, material);
	}

	std::shared_ptr<Model> PrimitiveFactory::skybox(std::shared_ptr<Material> material)
	{
		const std::vector<Vertex> vert(sky_box_vertices, sky_box_vertices + 36);
		const std::vector<size_t> ind(cube_indices, cube_indices + 36);
		return Model::load_raw(vert, ind, material);
	}
}
#ifndef _PRIMITIVE_FACTORY_
#define _PRIMITIVE_FACTORY_
#include <Guarneri.hpp>

namespace Guarneri {
	class PrimitiveFactory {
	public:
		static Vector3 cal_tangent(const Vertex& v1, const Vertex& v2, const Vertex& v3);
		static std::unique_ptr<Model> plane(std::unique_ptr<Material>& material);
		static std::unique_ptr<Model> cube(std::unique_ptr<Material>& material);
		static std::unique_ptr<Model> skybox(std::unique_ptr<Material>& material);
	};


	Vertex cube_vertices[36] = {
		Vertex(Vector4(-0.5f, -0.5f, -0.5f, 1.0f), Vector3(0.0f,  0.0f, -1.0f), Vector2(0.0f,  0.0f)),
		Vertex(Vector4(0.5f, -0.5f, -0.5f, 1.0f), Vector3(0.0f,  0.0f, -1.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(0.5f,  0.5f, -0.5f, 1.0f), Vector3(0.0f,  0.0f, -1.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(0.5f,  0.5f, -0.5f, 1.0f), Vector3(0.0f,  0.0f, -1.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(-0.5f,  0.5f, -0.5f, 1.0f), Vector3(0.0f,  0.0f, -1.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(-0.5f, -0.5f, -0.5f, 1.0f), Vector3(0.0f,  0.0f, -1.0f), Vector2(0.0f,  0.0f)),

		Vertex(Vector4(-0.5f, -0.5f,  0.5f, 1.0f), Vector3(0.0f,  0.0f, 1.0f), Vector2(0.0f,  0.0f)),
		Vertex(Vector4(0.5f, -0.5f,  0.5f, 1.0f), Vector3(0.0f,  0.0f, 1.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(0.5f,  0.5f,  0.5f, 1.0f), Vector3(0.0f,  0.0f, 1.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(0.5f,  0.5f,  0.5f, 1.0f), Vector3(0.0f,  0.0f, 1.0f), Vector2(1.0f, 1.0f)),
		Vertex(Vector4(-0.5f,  0.5f,  0.5f, 1.0f), Vector3(0.0f,  0.0f, 1.0f), Vector2(0.0f, 1.0f)),
		Vertex(Vector4(-0.5f, -0.5f,  0.5f, 1.0f), Vector3(0.0f,  0.0f, 1.0f), Vector2(0.0f,  0.0f)),

		Vertex(Vector4(-0.5f,  0.5f,  0.5f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(-0.5f,  0.5f, -0.5f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(-0.5f, -0.5f, -0.5f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(-0.5f, -0.5f, -0.5f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(-0.5f, -0.5f,  0.5f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector2(0.0f,  0.0f)),
		Vertex(Vector4(-0.5f,  0.5f,  0.5f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector2(1.0f,  0.0f)),

		Vertex(Vector4(0.5f,  0.5f,  0.5f, 1.0f), Vector3(1.0f,  0.0f,  0.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(0.5f,  0.5f, -0.5f, 1.0f), Vector3(1.0f,  0.0f,  0.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(0.5f, -0.5f, -0.5f, 1.0f), Vector3(1.0f,  0.0f,  0.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(0.5f, -0.5f, -0.5f, 1.0f), Vector3(1.0f,  0.0f,  0.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(0.5f, -0.5f,  0.5f, 1.0f), Vector3(1.0f,  0.0f,  0.0f), Vector2(0.0f,  0.0f)),
		Vertex(Vector4(0.5f,  0.5f,  0.5f, 1.0f), Vector3(1.0f,  0.0f,  0.0f), Vector2(1.0f,  0.0f)),

		Vertex(Vector4(-0.5f, -0.5f, -0.5f, 1.0f), Vector3(0.0f, -1.0f,  0.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(0.5f, -0.5f, -0.5f, 1.0f), Vector3(0.0f, -1.0f,  0.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(0.5f, -0.5f,  0.5f, 1.0f), Vector3(0.0f, -1.0f,  0.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(0.5f, -0.5f,  0.5f, 1.0f), Vector3(0.0f, -1.0f,  0.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(-0.5f, -0.5f,  0.5f, 1.0f), Vector3(0.0f, -1.0f,  0.0f), Vector2(0.0f,  0.0f)),
		Vertex(Vector4(-0.5f, -0.5f, -0.5f, 1.0f), Vector3(0.0f, -1.0f,  0.0f), Vector2(0.0f,  1.0f)),

		Vertex(Vector4(-0.5f,  0.5f, -0.5f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(0.0f,  1.0f)),
		Vertex(Vector4(0.5f,  0.5f, -0.5f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(1.0f,  1.0f)),
		Vertex(Vector4(0.5f,  0.5f,  0.5f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(0.5f, 0.5f,  0.5f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(1.0f,  0.0f)),
		Vertex(Vector4(-0.5f,  0.5f,  0.5f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(0.0f,  0.0f)),
		Vertex(Vector4(-0.5f,  0.5f, -0.5f, 1.0f), Vector3(0.0f, 1.0f,  0.0f), Vector2(0.0f,  1.0f)),
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
		Vertex(Vector4(1, 0, 1, 1), Vector3::UP,  Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1, 0, -1, 1), Vector3::UP,  Vector2(0.0f, 1.0f)),
		Vertex(Vector4(-1, 0, -1, 1), Vector3::UP, Vector2(1.0f, 1.0f)),
		Vertex(Vector4(-1, 0, -1, 1), Vector3::UP, Vector2(1.0f, 1.0f)),
		Vertex(Vector4(-1, 0, 1, 1), Vector3::UP, Vector2(1.0f, 0.0f)),
		Vertex(Vector4(1, 0, 1, 1), Vector3::UP,  Vector2(0.0f, 0.0f))
	};

	uint32_t plane_indices[6] = {
		0, 1, 2, 3, 4, 5
	};

	Vertex sky_box_vertices[] = {
		Vertex(Vector4(-1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),

		Vertex(Vector4(-1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),

		Vertex(Vector4(1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),

		Vertex(Vector4(-1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),

		Vertex(Vector4(-1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f,  1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),

		Vertex(Vector4(-1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f, -1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(-1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f)),
		Vertex(Vector4(1.0f, -1.0f,  1.0f, 1.0f), Vector3::UP, Vector2(0.0f, 0.0f))
	};

	Vector3 sphere_parametric_equation(const float& radius, const float& alpha, const float& phi)
	{
		Vector3 p;
		p.x = radius * std::sin(alpha * PI / 180.0f) * std::cos(phi * PI / 180.0f);
		p.y = radius * std::sin(alpha * PI / 180.0f) * std::sin(phi * PI / 180.0f);
		p.z = radius * std::cos(alpha * PI / 180.0f);
		return p;
	}

	Vector3 PrimitiveFactory::cal_tangent(const Vertex& v1, const Vertex& v2, const Vertex& v3)
	{
		auto edge1 = v2.position - v1.position;
		auto edge2 = v3.position - v1.position;
		auto duv1 = v2.uv - v1.uv;
		auto duv2 = v3.uv - v1.uv;
		float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
		Vector3 tangent;
		tangent.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
		tangent.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
		tangent.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);
		return tangent.normalized();
	}

	std::unique_ptr<Model> PrimitiveFactory::plane(std::unique_ptr<Material>& material)
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
		const std::vector<uint32_t> ind(plane_indices, plane_indices + 6);
		return Model::create(vert, ind, material);
	}

	std::unique_ptr<Model> PrimitiveFactory::cube(std::unique_ptr<Material>& material)
	{
		const std::vector<Vertex> vert(cube_vertices, cube_vertices + 36);
		const std::vector<uint32_t> ind(cube_indices, cube_indices + 36);
		return Model::create(vert, ind, material);
	}

	std::unique_ptr<Model> PrimitiveFactory::skybox(std::unique_ptr<Material>& material)
	{
		const std::vector<Vertex> vert(sky_box_vertices, sky_box_vertices + 36);
		const std::vector<uint32_t> ind(cube_indices, cube_indices + 36);
		return Model::create(vert, ind, material);
	}
}
#endif
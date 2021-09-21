#include "Light.hpp"

namespace Guarneri
{
	Light::Light()
	{
		ambient = Color::BLACK;
		diffuse = Color::WHITE;
		specular = Color::WHITE;
		intensity = 1.0f;
	}

	DirectionalLight::DirectionalLight() : Light()
	{
		this->yaw = 0.0f;
		this->pitch = -45.0f;
		//this->p = Matrix4x4::perspective(45.0f, 800.0f/600.0f, 0.5f, 500.0f);
		this->p = Matrix4x4::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.01f, 200.0f);
		this->position = Vector3(10.0f, 10.0f, 10.0f);
		update_rotation();
	}

	Matrix4x4 DirectionalLight::light_space() const
	{
		auto v = Matrix4x4::lookat(position, position + forward, Vector3::UP);
		auto ret = p * v;
		return ret;
	}

	Matrix4x4 DirectionalLight::view_matrix() const
	{
		return Matrix4x4::lookat(position, position + forward, Vector3::UP);
	}

	Matrix4x4 DirectionalLight::projection_matrix() const
	{
		return p;
	}

	void DirectionalLight::rotate(const float& yaw_offset, const float& pitch_offset)
	{
		this->yaw -= yaw_offset;
		this->pitch -= pitch_offset;
		this->pitch = std::clamp(this->pitch, -89.0f, 89.0f);
		update_rotation();
	}

	void DirectionalLight::update_rotation()
	{
		forward.x = cos(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
		forward.y = sin(DEGREE2RAD(this->pitch));
		forward.z = sin(DEGREE2RAD(this->yaw)) * cos(DEGREE2RAD(this->pitch));
		forward = Vector3::normalize(forward);
		Vector3::calculate_right_up(forward, right, up);
	}

	rapidjson::Value DirectionalLight::serialize(rapidjson::Document& doc, const DirectionalLight& light)
	{
		rapidjson::Value serialized_light;
		serialized_light.SetObject();
		rapidjson::Value ambient = Color::serialize(doc, light.ambient);
		rapidjson::Value diffuse = Color::serialize(doc, light.diffuse);
		rapidjson::Value specular = Color::serialize(doc, light.specular);
		rapidjson::Value intensity; intensity.SetFloat(light.intensity);
		rapidjson::Value yaw; yaw.SetFloat(light.yaw);
		rapidjson::Value pitch; pitch.SetFloat(light.pitch);
		rapidjson::Value position = Vector3::serialize(doc, light.position);
		serialized_light.AddMember("ambient", ambient, doc.GetAllocator());
		serialized_light.AddMember("diffuse", diffuse, doc.GetAllocator());
		serialized_light.AddMember("specular", specular, doc.GetAllocator());
		serialized_light.AddMember("intensity", intensity, doc.GetAllocator());
		serialized_light.AddMember("yaw", yaw, doc.GetAllocator());
		serialized_light.AddMember("pitch", pitch, doc.GetAllocator());
		serialized_light.AddMember("position", position, doc.GetAllocator());
		return serialized_light;
	}

	DirectionalLight DirectionalLight::deserialize(const rapidjson::Value& v)
	{
		DirectionalLight light; 
		light.ambient = Color::deserialize(v["ambient"].GetObject());
		light.diffuse = Color::deserialize(v["diffuse"].GetObject());
		light.specular = Color::deserialize(v["specular"].GetObject());
		light.intensity = v["intensity"].GetFloat();
		light.yaw = v["yaw"].GetFloat();
		light.pitch = v["pitch"].GetFloat();
		light.position = Vector3::deserialize(v["position"].GetObject());
		return light;
	}

	PointLight::PointLight() : Light()
	{
		constant = 1.0f;
		linear = 0.1f;
		quadratic = 0.03f;
		position = Vector3();
	}

	rapidjson::Value PointLight::serialize(rapidjson::Document& doc, const PointLight& light)
	{
		rapidjson::Value serialized_light;
		serialized_light.SetObject();
		rapidjson::Value ambient = Color::serialize(doc, light.ambient);
		rapidjson::Value diffuse = Color::serialize(doc, light.diffuse);
		rapidjson::Value specular = Color::serialize(doc, light.specular);
		rapidjson::Value intensity; intensity.SetFloat(light.intensity);
		rapidjson::Value constant; constant.SetFloat(light.constant);
		rapidjson::Value linear; linear.SetFloat(light.linear);
		rapidjson::Value quadratic; quadratic.SetFloat(light.quadratic);
		rapidjson::Value position = Vector3::serialize(doc, light.position);
		serialized_light.AddMember("ambient", ambient, doc.GetAllocator());
		serialized_light.AddMember("diffuse", diffuse, doc.GetAllocator());
		serialized_light.AddMember("specular", specular, doc.GetAllocator());
		serialized_light.AddMember("intensity", intensity, doc.GetAllocator());
		serialized_light.AddMember("constant", constant, doc.GetAllocator());
		serialized_light.AddMember("linear", linear, doc.GetAllocator());
		serialized_light.AddMember("quadratic", quadratic, doc.GetAllocator());
		serialized_light.AddMember("position", position, doc.GetAllocator());
		return serialized_light;
	}

	PointLight PointLight::deserialize(const rapidjson::Value& v)
	{
		PointLight light;
		light.ambient = Color::deserialize(v["ambient"].GetObject());
		light.diffuse = Color::deserialize(v["diffuse"].GetObject());
		light.specular = Color::deserialize(v["specular"].GetObject());
		light.intensity = v["intensity"].GetFloat();
		light.constant = v["constant"].GetFloat();
		light.linear = v["linear"].GetFloat();
		light.quadratic = v["quadratic"].GetFloat();
		light.position = Vector3::deserialize(v["position"].GetObject());
		return light;
	}
}
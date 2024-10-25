#include "Scene.h"
#include "Utils.h"
#include "Material.h"

#include <ranges>
#include <algorithm>

#include "SDL_egl.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene() :
		m_Materials({ new Material_SolidColor({1,0,0}) })
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		HitRecord closestHitRecord{ };

		for(auto const& sphere : m_SphereGeometries)
		{
			HitRecord temp{ };
			if (GeometryUtils::HitTest_Sphere(sphere, ray, temp))
			{
				if (temp.t < closestHitRecord.t)
				{
					closestHitRecord = temp;
				}
			}
		}

		for (auto const& plane : m_PlaneGeometries)
		{
			HitRecord temp{ };
			if (GeometryUtils::HitTest_Plane(plane, ray, temp))
			{
				if (temp.t < closestHitRecord.t)
				{
					closestHitRecord = temp;
				}
			}
		}

		for (auto const& mesh : m_TriangleMeshGeometries)
		{
			HitRecord temp{ };
			if (GeometryUtils::HitTest_TriangleMesh(mesh, ray, temp))
			{
				if (temp.t < closestHitRecord.t)
				{
					closestHitRecord = temp;
				}
			}
		}

		closestHit = closestHitRecord;
	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		for (auto const& sphere : m_SphereGeometries)
		{
			if (GeometryUtils::HitTest_Sphere(sphere, ray))
			{
				return true;
			}
		}

		for (auto const& plane : m_PlaneGeometries)
		{
			if (GeometryUtils::HitTest_Plane(plane, ray))
			{
				return true;
			}
		}

		for (auto const& mesh : m_TriangleMeshGeometries)
		{
			if (GeometryUtils::HitTest_TriangleMesh(mesh, ray))
			{
				return true;
			}
		}

		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddAreaLight(Vector3 const& origin, float intensity, ColorRGB const& color, LightShape shape, float radius, std::vector<Vector3> const& vertices)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Area;

		l.radius = radius;
		l.vertices = vertices;

		l.shape = shape;

		switch (l.shape)
		{
		case LightShape::None:
			assert(l.vertices.size() == 0);
			break;
		case LightShape::Triangular:
			assert(l.vertices.size() == 3);
			break;
		}
		l.vertices.shrink_to_fit();

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W1
	void Scene_W1::Initialize()
	{
		//default: Material id0 >> SolidColor Material (RED)
		unsigned char constexpr matId_Solid_Red{ 0 };
		unsigned char const matId_Solid_Blue{ AddMaterial(new Material_SolidColor{ colors::Blue }) };

		unsigned char const matId_Solid_Yellow{ AddMaterial(new Material_SolidColor{ colors::Yellow }) };
		unsigned char const matId_Solid_Green { AddMaterial(new Material_SolidColor{ colors::Green }) };
		unsigned char const matId_Solid_Magenta{ AddMaterial(new Material_SolidColor{ colors::Magenta }) };

		//Spheres
		AddSphere({ -25.f, 0.f, 100.f }, 50.f, matId_Solid_Red);
		AddSphere({ 25.f, 0.f, 100.f }, 50.f, matId_Solid_Blue);

		//Planes
		AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
	}
#pragma endregion

#pragma region SCENE W2
	void Scene_W2::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		//default: Material id0 >> SolidColor Material (RED)
		unsigned char constexpr matId_Solid_Red{ 0 };
		unsigned char const matId_Solid_Blue{ AddMaterial(new Material_SolidColor{ colors::Blue }) };

		unsigned char const matId_Solid_Yellow{ AddMaterial(new Material_SolidColor{ colors::Yellow }) };
		unsigned char const matId_Solid_Green{ AddMaterial(new Material_SolidColor{ colors::Green }) };
		unsigned char const matId_Solid_Magenta{ AddMaterial(new Material_SolidColor{ colors::Magenta }) };

		//Planes
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

		//Light
		AddPointLight({ 0.f, 5.f, -5.f }, 70.f, colors::White);
	}
#pragma endregion

#pragma region SCENE W3
	void Scene_W3::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		auto const matCT_GrayRoughMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, 1.f }) };
		auto const matCT_GrayMediumMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, .6f }) };
		auto const matCT_GraySmoothMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, .1f }) };
		auto const matCT_GrayRoughPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, 1.f }) };
		auto const matCT_GrayMediumPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, .6f }) };
		auto const matCT_GraySmoothPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, .1f }) };

		auto const matLambertGrayBlue{ AddMaterial(new Material_Lambert{{.49f, .57f, .57f}, 1.f}) };

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matLambertGrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matLambertGrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matLambertGrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matLambertGrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matLambertGrayBlue);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		//Light
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f }); //Backlight
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .80f, .45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f }); 
	}

	void Scene_W3_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		unsigned char const matId_Red{ AddMaterial(new Material_Lambert{ colors::Red, 1.f }) };
		unsigned char const matId_Blue{ AddMaterial(new Material_LambertPhong{ colors::Blue, 1.f, 1.f, 60.f }) };
		unsigned char const matId_Yellow{ AddMaterial(new Material_Lambert{ colors::Yellow, 1.f }) };

		//Spheres
		AddSphere({ -.75f, 1.f, 0.f }, 1.f, matId_Red);
		AddSphere({ .75f, 1.f, 0.f }, 1.f, matId_Blue);

		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Yellow);

		AddPointLight({ 0.f, 5.f, 5.f }, 25.f, colors::White);
		AddPointLight({ 0.f, 2.5f, -5.f }, 25.f, colors::White);
	}
#pragma endregion

#pragma region SCENE W4
	void Scene_TriangleTest::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		auto const matLambert_GrayBlue{ AddMaterial(new Material_Lambert{ {.49f, .57f, .57f }, 1.f}) };
		auto const matLambert_White{ AddMaterial(new Material_Lambert{ colors::White, 1.f}) };

		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);

		Triangle const baseTriangle{ {-.75f, 1.5f, 0.f}, {.75f, 0.f, 0.f }, {-.75f, 0.f, 0.f} };
		auto m{ AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White) };
		m->AppendTriangle(baseTriangle, true);
		m->Translate({ 0.f, .5f, 0.f });
		m->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .80f, .45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f });
	}

	void Scene_W4_TestScene::Initialize()
	{
		m_Camera.origin = { 0.f, 1.f, -5.f };
		m_Camera.fovAngle = 45.f;

		auto const matLambert_GrayBlue{ AddMaterial(new Material_Lambert{ {.49f, .57f, .57f }, 1.f}) };
		auto const matLambert_White{ AddMaterial(new Material_Lambert{ colors::White, 1.f}) };

		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);

		auto pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		//Utils::ParseOBJ("resources/simple_cube.obj",
		Utils::ParseOBJ("resources/simple_object.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ .7f, .7f, .7f });
		pMesh->Translate({ 0.f, 1.f, 0.f });

		pMesh->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .80f, .45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f });
	}

	void Scene_W4_TestScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		//m_TriangleMeshGeometries.back().RotateY(PI_DIV_2 * pTimer->GetTotal());
		//m_TriangleMeshGeometries.back().UpdateTransforms();
	}

	void Scene_W4_ReferenceScene::Initialize()
	{
		m_SceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		auto const matCT_GrayRoughMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, 1.f }) };
		auto const matCT_GrayMediumMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, .6f }) };
		auto const matCT_GraySmoothMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, .1f }) };
		auto const matCT_GrayRoughPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, 1.f }) };
		auto const matCT_GrayMediumPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, .6f }) };
		auto const matCT_GraySmoothPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, .1f }) };

		auto const matLambert_GrayBlue{ AddMaterial(new Material_Lambert{ {.49f, .57f, .57f }, 1.f}) };
		auto const matLambert_White{ AddMaterial(new Material_Lambert{ colors::White, 1.f}) };

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		Triangle const baseTriangle{ {-.75f, 1.5f, 0.f}, {.75f, 0.f, 0.f }, {-.75f, 0.f, 0.f} };
		auto m{ AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White) };
		m->AppendTriangle(baseTriangle, true);
		m->Translate({ -1.75f, 4.5f, 0.f });
		m->UpdateTransforms();

		m = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
		m->AppendTriangle(baseTriangle, true);
		m->Translate({ 0.f, 4.5f, 0.f });
		m->UpdateTransforms();

		m = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
		m->AppendTriangle(baseTriangle, true);
		m->Translate({ 1.75f, 4.5f, 0.f });
		m->UpdateTransforms();

		//Lights
		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .80f, .45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f });
	}

	void Scene_W4_ReferenceScene::Update(Timer* pTimer)
	{
		Scene::Update(pTimer);

		auto const yawAngle{ (cos(pTimer->GetTotal()) + 1.f) / 2.f * PI_2 };
		for(auto& m : m_TriangleMeshGeometries)
		{
			m.RotateY(yawAngle);
			m.UpdateTransforms();
		}
	}

	void Scene_W4_BunnyScene::Initialize()
	{
		m_SceneName = "Reference Scene";
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		auto const matLambert_GrayBlue{ AddMaterial(new Material_Lambert{ {.49f, .57f, .57f }, 1.f}) };
		auto const matLambert_White{ AddMaterial(new Material_Lambert{ colors::White, 1.f}) };

		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);

		auto pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
		Utils::ParseOBJ("resources/lowpoly_bunny.obj",
			pMesh->positions,
			pMesh->normals,
			pMesh->indices);

		pMesh->Scale({ 2.f, 2.f, 2.f });
		pMesh->RotateY(TO_RADIANS * 180.f);

		pMesh->UpdateTransforms();

		AddPointLight({ 0.f, 5.f, 5.f }, 50.f, { 1.f, .61f, .45f });
		AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .80f, .45f });
		AddPointLight({ 2.5f, 2.5f, -5.f }, 50.f, { .34f, .47f, .68f });
	}
#pragma endregion

	void Scene_Softshadows::Initialize()
	{
		m_Camera.origin = { 0.f, 3.f, -9.f };
		m_Camera.fovAngle = 45.f;

		auto const matCT_GrayRoughMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, 1.f }) };
		auto const matCT_GrayMediumMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, .6f }) };
		auto const matCT_GraySmoothMetal{ AddMaterial(new Material_CookTorrence{ {.972f, .960f, .915f}, 1.f, .1f }) };
		auto const matCT_GrayRoughPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, 1.f }) };
		auto const matCT_GrayMediumPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, .6f }) };
		auto const matCT_GraySmoothPlastic{ AddMaterial(new Material_CookTorrence{ {.75f, .75f, .75f}, 0.f, .1f }) };

		auto const matLambert_GrayBlue{ AddMaterial(new Material_Lambert{ {.49f, .57f, .57f }, 1.f}) };

		//Planes
		AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);
		AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);

		//Spheres
		AddSphere({ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
		AddSphere({ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
		AddSphere({ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);
		AddSphere({ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
		AddSphere({ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
		AddSphere({ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

		AddAreaLight({ 0.f, 8.f, -5.f }, 100.f , { 1.f, 1.f, 1.f }, LightShape::Triangular, 0.f, { {0.f, 8.f, -5.f}, { 1.f, 9.f, -5.f}, {2.f, 8.f, -5.f} });
	}

}

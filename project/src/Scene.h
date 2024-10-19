#pragma once
#include <string>
#include <vector>

#include "Maths.h"
#include "DataTypes.h"
#include "Camera.h"

namespace dae
{
	//Forward Declarations
	class Timer;
	class Material;
	struct Plane;
	struct Sphere;
	struct Light;

	//Scene Base Class
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		Scene(const Scene&) = delete;
		Scene(Scene&&) noexcept = delete;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) noexcept = delete;

		virtual void Initialize() = 0;
		virtual void Update(dae::Timer* pTimer)
		{
			m_Camera.Update(pTimer);
		}

		Camera& GetCamera() { return m_Camera; }
		void GetClosestHit(const Ray& ray, HitRecord& closestHit) const;
		[[nodiscard]] bool DoesHit(const Ray& ray) const;

		std::vector<Plane> const& GetPlaneGeometries() const { return m_PlaneGeometries; }
		std::vector<Sphere>const& GetSphereGeometries() const { return m_SphereGeometries; }
		std::vector<Light> const& GetLights() const { return m_Lights; }
		std::vector<Material*> const& GetMaterials() const { return m_Materials; }

	protected:
		std::string m_SceneName;

		std::vector<Plane> m_PlaneGeometries{};
		std::vector<Sphere> m_SphereGeometries{};
		std::vector<TriangleMesh> m_TriangleMeshGeometries{};
		std::vector<Light> m_Lights{};
		std::vector<Material*> m_Materials{};

		Camera m_Camera{};

		Sphere* AddSphere(Vector3 const& origin, float radius, unsigned char materialIndex = 0);
		Plane* AddPlane(Vector3 const& origin, Vector3 const& normal, unsigned char materialIndex = 0);
		TriangleMesh* AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex = 0);

		Light* AddPointLight(Vector3 const& origin, float intensity, ColorRGB const& color);
		Light* AddDirectionalLight(Vector3 const& direction, float intensity, ColorRGB const& color);
		unsigned char AddMaterial(Material* pMaterial);
	};

	class Scene_W1 final : public Scene
	{
	public:
		Scene_W1() = default;
		~Scene_W1() override = default;

		Scene_W1(const Scene_W1&) = delete;
		Scene_W1(Scene_W1&&) noexcept = delete;
		Scene_W1& operator=(const Scene_W1&) = delete;
		Scene_W1& operator=(Scene_W1&&) noexcept = delete;

		void Initialize() override;
	};

	class Scene_W2 final : public Scene
	{
	public:
		Scene_W2() = default;
		~Scene_W2() override = default;

		Scene_W2(const Scene_W2&) = delete;
		Scene_W2(Scene_W2&&) noexcept = delete;
		Scene_W2& operator=(const Scene_W2&) = delete;
		Scene_W2& operator=(Scene_W2&&) noexcept = delete;

		void Initialize() override;
	};

	class Scene_W3 final : public Scene
	{
	public:
		Scene_W3() = default;
		~Scene_W3() override = default;

		Scene_W3(const Scene_W3&) = delete;
		Scene_W3(Scene_W3&&) noexcept = delete;
		Scene_W3& operator=(const Scene_W3&) = delete;
		Scene_W3& operator=(Scene_W3&&) noexcept = delete;

		void Initialize() override;
	};

	class Scene_W3_TestScene final : public Scene
	{
	public:
		Scene_W3_TestScene() = default;
		~Scene_W3_TestScene() override = default;

		Scene_W3_TestScene(const Scene_W3_TestScene&) = delete;
		Scene_W3_TestScene(Scene_W3_TestScene&&) noexcept = delete;
		Scene_W3_TestScene& operator=(const Scene_W3_TestScene&) = delete;
		Scene_W3_TestScene& operator=(Scene_W3_TestScene&&) noexcept = delete;

		void Initialize() override;
	};

	class Scene_TriangleTest final : public Scene
	{
	public:
		Scene_TriangleTest() = default;
		~Scene_TriangleTest() override = default;

		Scene_TriangleTest(const Scene_TriangleTest&) = delete;
		Scene_TriangleTest(Scene_TriangleTest&&) noexcept = delete;
		Scene_TriangleTest& operator=(const Scene_TriangleTest&) = delete;
		Scene_TriangleTest& operator=(Scene_TriangleTest&&) noexcept = delete;

		void Initialize() override;
		};

	class Scene_W4_TestScene final : public Scene
	{
	public:
		Scene_W4_TestScene() = default;
		~Scene_W4_TestScene() override = default;

		Scene_W4_TestScene(const Scene_W4_TestScene&) = delete;
		Scene_W4_TestScene(Scene_W4_TestScene&&) noexcept = delete;
		Scene_W4_TestScene& operator=(const Scene_W4_TestScene&) = delete;
		Scene_W4_TestScene& operator=(Scene_W4_TestScene&&) noexcept = delete;

		void Initialize() override;
		void Update(Timer* pTimer) override;
	};

	class Scene_W4_ReferenceScene final : public Scene
	{
	public:
		Scene_W4_ReferenceScene() = default;
		~Scene_W4_ReferenceScene() override = default;

		Scene_W4_ReferenceScene(const Scene_W4_ReferenceScene&) = delete;
		Scene_W4_ReferenceScene(Scene_W4_ReferenceScene&&) noexcept = delete;
		Scene_W4_ReferenceScene& operator=(const Scene_W4_ReferenceScene&) = delete;
		Scene_W4_ReferenceScene& operator=(Scene_W4_ReferenceScene&&) noexcept = delete;

		void Initialize() override;
		void Update(Timer* pTimer) override;
	};

	class Scene_W4_BunnyScene final : public Scene
	{
	public:
		Scene_W4_BunnyScene() = default;
		~Scene_W4_BunnyScene() override = default;

		Scene_W4_BunnyScene(const Scene_W4_BunnyScene&) = delete;
		Scene_W4_BunnyScene(Scene_W4_BunnyScene&&) noexcept = delete;
		Scene_W4_BunnyScene& operator=(const Scene_W4_BunnyScene&) = delete;
		Scene_W4_BunnyScene& operator=(Scene_W4_BunnyScene&&) noexcept = delete;

		void Initialize() override;
	};

}

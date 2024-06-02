#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "UBO.h"
#include <memory>

struct Light {
	glm::vec3 direction;
	float intensity;
	glm::vec3 color;	
	float padding;
	Light(const glm::vec3& dir, const glm::vec3& col, float inten)
		: direction(dir), color(col), intensity(inten), padding(0.0f) {}
};

struct Sat
{
	Sat() = default;

	void Create(float* data, int width, int height);
	float GetSum(int x, int y, int w, int h);
	float PickFromIntegral(int x, int y);

	int m_Width;
	int m_Height;
	std::vector<float> m_IntegralImage;
	float* m_HDRI;
};

struct Region
{
	Region() = default;

	void Create(int x, int y, int w, int h, Sat* parent);
	void SplitVertical(Region& r1);
	void SplitVertical(Region& r1, Region& r2);
	void SplitHorizontal(Region& r1);
	void SplitHorizontal(Region& r1, Region& r2);
	glm::vec2 GetCentroid();
	glm::vec3 GetColor();

	int m_X, m_Y, m_Width, m_Height;
	float m_Sum;
	Sat* m_Sat;
};

class MedianCut
{
public:
	MedianCut() = default;
	~MedianCut();

	void LoadHDRI(const std::string& filepath);
	void Process(int n);
	void WriteToPPM();
	int GetNumLights();
	std::vector<Light> GetLights();
private:
	void Split(Region& r, int depth);
	glm::vec3 EquirectengularToCartesian(glm::vec2 uv);
	void ApplyCosInclination();

	float* m_HDRI = nullptr;
	int m_Width, m_Height;
	std::vector<Region> m_Regions;
	std::vector<Light> m_Lights;
	Sat m_Sat;
	GLuint m_SSBO;
};

#include "MedianCut.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include <iostream>

#define PI 3.14159265358979323846

MedianCut::~MedianCut()
{
	stbi_image_free(m_HDRI);
}

void MedianCut::LoadHDRI(const std::string& filepath)
{
    int width, height, nrComponents;
    float* data = stbi_loadf(filepath.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        m_Width = width;
		m_Height = height;
		m_HDRI = data;
        ApplyCosInclination();
        m_Sat.Create(data, width, height);
    }
    else
    {
        std::cout << "Failed to load HDRI image" << std::endl;
    }
}

void MedianCut::Process(int n)
{
    m_Regions.clear();
    m_Lights.clear();

    Region root;
    root.Create(0, 0, m_Width, m_Height, &m_Sat);
    
    Split(root, n);

    for (Region& r : m_Regions)
    {
        // Construct the light here.
        glm::vec3 color = r.GetColor();
        glm::vec2 uv = r.GetCentroid();
        glm::vec3 direction = EquirectengularToCartesian(uv);
        float numLights = m_Regions.size();
        float intensity = r.m_Sum / (r.m_Width * r.m_Height);
        
        m_Lights.push_back(Light(direction, color, intensity));
    }
}

void MedianCut::WriteToPPM()
{
    std::vector<unsigned char> conv;
    conv.resize(m_Width * m_Height * 3);

    for (size_t i = 0; i < m_Width * m_Height * 3; ++i)
        conv[i] = static_cast<unsigned char>(m_HDRI[i] * 255);
   
    // Draw regions 
    for (Region& r : m_Regions)
    {
        glm::vec2 centroid = r.GetCentroid();
        glm::vec3 color = r.GetColor();
        color = glm::normalize(color);


        int x = centroid.x;
        int y = centroid.y;
        // draw a cross
        for (int i = -3; i <= 3; i++)
        {
			conv[(y + i) * m_Width * 3 + x * 3] = 255 * color.r;
			conv[(y + i) * m_Width * 3 + x * 3 + 1] = 255 * color.g;
			conv[(y + i) * m_Width * 3 + x * 3 + 2] = 255 * color.b;

			conv[y * m_Width * 3 + (x + i) * 3] = 0;
			conv[y * m_Width * 3 + (x + i) * 3 + 1] = 0;
			conv[y * m_Width * 3 + (x + i) * 3 + 2] = 255;
		}

        for (int x = r.m_X + 1; x < r.m_X + r.m_Width - 1; x++)
        { 
            conv[r.m_Y * m_Width * 3 + x * 3] = 0;
			conv[r.m_Y * m_Width * 3 + x * 3 + 1] = 0;
			conv[r.m_Y * m_Width * 3 + x * 3 + 2] = 255;

			conv[(r.m_Y + r.m_Height - 1) * m_Width * 3 + x * 3] = 0;
			conv[(r.m_Y + r.m_Height - 1) * m_Width * 3 + x * 3 + 1] = 0;
			conv[(r.m_Y + r.m_Height - 1) * m_Width * 3 + x * 3 + 2] = 255;

        }
        for (int y = r.m_Y + 1; y < r.m_Y + r.m_Height - 1; y++)
        {
            conv[y * m_Width * 3 + r.m_X * 3 + 2] = 255;
        }
    }
    stbi_write_bmp("test.bmp", m_Width, m_Height, 3, &conv[0]);
}

void MedianCut::Split(Region& r, int depth)
{
    if (r.m_Width <= 1 || r.m_Height <= 1 || depth == 0)
	{
		m_Regions.push_back(r);
		return;
	}

    Region r1, r2;
    if (r.m_Width > r.m_Height)
	{
		r.SplitVertical(r1, r2);
	}
	else
	{
		r.SplitHorizontal(r1, r2);
	}
    Split(r1, depth - 1);
	Split(r2, depth - 1);
}

glm::vec3 MedianCut::EquirectengularToCartesian(glm::vec2 uv)
{
    // Convert from [0, 1] range to [-1, 1] range for latitude and longitude
    float nx = uv.x / m_Width;
    float ny = uv.y / m_Height;
    float longitude = nx * 2.0 * 3.14159265359 - 3.14159265359;
    float latitude = ny * 3.14159265359 - 1.57079632679;

    // Spherical to Cartesian conversion
    float x = cos(latitude) * cos(longitude);
    float y = sin(latitude);
    float z = cos(latitude) * sin(longitude);

    return glm::vec3(x, y, z);
}

int MedianCut::GetNumLights()
{
    return m_Lights.size();
}

std::vector<Light> MedianCut::GetLights()
{
    return m_Lights;
}

void MedianCut::ApplyCosInclination()
{
    for (int y = 0; y < m_Height; y++)
    {
        for (int x = 0; x < m_Width; x++)
        {
			float phi = (2 * PI * x / m_Width) - PI;
			float theta = (PI * y / m_Height) - (PI / 2);

			float cosTheta = cos(theta);
			m_HDRI[(y * m_Width + x) * 3] *= cosTheta;
			m_HDRI[(y * m_Width + x) * 3 + 1] *= cosTheta;
			m_HDRI[(y * m_Width + x) * 3 + 2] *= cosTheta;
		}
    }
}

void Region::Create(int x, int y, int w, int h, Sat* parent)
{
    m_Sat = parent;
    m_X = x;
    m_Y = y;
    m_Width = w;
    m_Height = h;
    m_Sum = m_Sat->GetSum(x, y, w, h);
}

void Region::SplitVertical(Region& r1)
{
    for (int i = 0; i <= m_Width; i++)
    {
        r1.Create(m_X, m_Y, i, m_Height, m_Sat);
        if (r1.m_Sum > m_Sum / 2)
		{
			break;
		}
    }
}

void Region::SplitVertical(Region& r1, Region& r2)
{
    SplitVertical(r1);
    r2.Create(m_X + (r1.m_Width - 1), m_Y, m_Width - (r1.m_Width), m_Height, m_Sat);
}

void Region::SplitHorizontal(Region& r1)
{
    for (int i = 0; i <= m_Height; i++)
	{
		r1.Create(m_X, m_Y, m_Width, i, m_Sat);
		if (r1.m_Sum > m_Sum / 2)
		{
			break;
		}
	}
}

void Region::SplitHorizontal(Region& r1, Region& r2)
{
    SplitHorizontal(r1);
	r2.Create(m_X, m_Y + (r1.m_Height - 1), m_Width, m_Height - (r1.m_Height), m_Sat);
}

glm::vec2 Region::GetCentroid()
{
    Region r1, r2, r3, r4;
    SplitVertical(r1, r2);
    SplitHorizontal(r3, r4);
    float x = r1.m_X + r1.m_Width;
    float y = r3.m_Y + r3.m_Height;
    return glm::vec2(x, y);
}

glm::vec3 Region::GetColor()
{
    // Color is the sum of all pixels in the region, not their average
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    for (int j = m_Y; j < m_Y + m_Height; j++)
    {
        for (int i = m_X; i < m_X + m_Width; i++)
        {
			int index = j * m_Sat->m_Width + i;
			r += m_Sat->m_HDRI[index * 3 + 0];
			g += m_Sat->m_HDRI[index * 3 + 1];
			b += m_Sat->m_HDRI[index * 3 + 2];
		}
	}
    return glm::vec3(r, g, b);
}

void Sat::Create(float* data, int width, int height)
{
    m_Width = width;
    m_Height = height;
    m_HDRI = data;
    m_IntegralImage.resize(width * height, 0);
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int index = j * width + i;
            float r = data[index * 3 + 0];
            float g = data[index * 3 + 1];
            float b = data[index * 3 + 2];
            float lum = 0.2125f * r + 0.7154f * g + 0.0721f * b;
            m_IntegralImage[index] 
                = lum + PickFromIntegral(i - 1, j) + PickFromIntegral(i, j - 1) - PickFromIntegral(i - 1, j - 1);
        }
    }
}

float Sat::GetSum(int x, int y, int w, int h)
{
    float pos = PickFromIntegral(x + w - 1, y + h - 1) + PickFromIntegral(x, y);
    float neg = PickFromIntegral(x, y + h - 1) + PickFromIntegral(x + w, y - 1);
    return pos - neg;
}

float Sat::PickFromIntegral(int x, int y)
{
    if (x < 0 || y < 0) return 0;
    if (x >= m_Width) x = m_Width - 1;
    if (y >= m_Height) y = m_Height - 1;
    return m_IntegralImage[y * m_Width + x];
}

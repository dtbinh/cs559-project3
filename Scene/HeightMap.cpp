/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map 
/************************************************************************/
#include "HeightMap.h"
#include "Camera.h"
#include "../Core/Common.h"
#include "../Core/ImageManager.h"
#include "../Utility/Matrix2d.h"

#include <SFML/Graphics.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <cmath>

using std::vector;


HeightMap::HeightMap(const unsigned int width
					, const unsigned int height
					, const float groundScale
					, const float heightScale)
	: heights(height, width)
	, groundScale(groundScale)
	, heightScale(heightScale)
	, imageName("")
{
	randomizeGaussian();
}

void HeightMap::render(Camera *camera)
{
	glDisable(GL_LIGHTING); // TODO: calculate and store normals

	const sf::Image& image = ImageManager::get().getImage(imageName);
	image.Bind();
	glColor4f(1,1,1,1);

	const float dt = 1.f / static_cast<float>(heights.rows() - 1);
	const float ds = 1.f / static_cast<float>(heights.cols() - 1);

	// TODO: vertex arrays or vertex buffer objects
	for(unsigned int row = 0; row < (heights.rows() - 1); ++row)
	{
		float t = row * dt;

		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int col = 0; col < (heights.cols() - 1); ++col)
		{
			float s = col * ds; 

			// row, col
			glTexCoord2f(s, t);
			glVertex3d(groundScale * col
					 , heightScale * heights(row,col)
					 , groundScale * row);
			// row+1,col
			glTexCoord2f(s, t);
			glVertex3d(groundScale * col
					 , heightScale * heights(row+1,col)
					 , groundScale * (row + 1));
			// row, col+1
			glTexCoord2f(s, t);
			glVertex3d(groundScale * (col + 1)
					 , heightScale * heights(row,col+1)
					 , groundScale * row);
			// row+1, col+1
			glTexCoord2f(s, t);
			glVertex3d(groundScale * (col + 1)
					 , heightScale * heights(row+1,col+1)
					 , groundScale * (row + 1));
		}
		glEnd();
	}

	glEnable(GL_LIGHTING); // TODO: calculate and store normals
}

void HeightMap::randomizeGaussian()
{
	for(unsigned int row = 0; row < heights.rows(); ++row)
	for(unsigned int col = 0; col < heights.cols(); ++col)
	{
		heights(row,col) = glm::gaussRand(1.0, 0.4);
	}
}

void HeightMap::loadFromImage( const std::string& filename )
{
	imageName = filename;

	sf::Image& image = ImageManager::get().getImage(filename);
	image.SetSmooth(false); // fixes bad pixel border

	const unsigned int width  = image.GetWidth();
	const unsigned int height = image.GetHeight();
	heights = HeightMatrix(height, width);

	for(unsigned int y = 0; y < height; ++y)
	for(unsigned int x = 0; x < width;  ++x)
	{
		const sf::Color pixel  = image.GetPixel(x,y);
		// Use luminance value to determine height in [0,1]
		const double lum = 0.299 * (pixel.r / 255.0)
						 + 0.587 * (pixel.g / 255.0)
						 + 0.114 * (pixel.b / 255.0);
		heights(y,x) = lum;
	}
}

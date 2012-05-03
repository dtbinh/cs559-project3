#pragma once
/************************************************************************/
/* HeightMap 
/* ---------
/* A basic height map mesh
/************************************************************************/
#include "../Utility/Mesh.h"
#include "../Utility/Logger.h"


class HeightMap : public Mesh
{
private:
	std::string  imageName;
	float heightScale;
	float groundScale;

public:
	/**
	 * Creates a new heightmap with the specified parameters
	 * \param width  - number of vertices wide
	 * \param height - number of vertices long
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const unsigned int width  = 100
			, const unsigned int height = 100
			, const float groundScale   = 1.f
			, const float heightScale   = 1.f);

	/**
	 * Creates a new heightmap by loading height data from an image 
	 * \param imageFilename - the name of the image file that has height data 
	 * \param groundScale - distance between vertices in the plane
	 * \param heightScale - distance between vertices vertically
	**/
	HeightMap(const std::string& imageFilename
			, const float groundScale   = 0.5f
			, const float heightScale   = 2.f);

	/**
	 * Get the height value for the given column and row 
	 * using bilinear interpolation to get a value between 
	 * neighboring vertices.
	 * \param col - the column index to lookup
	 * \param row - the row index to lookup
	 * \return - the height value at the specified indices,
	 *           or the smallest possible float 
	**/
	float heightAt(const float col, const float row);

	float getHeightScale() const;
	float getGroundScale() const;

private:
	void randomizeGaussian();
};


inline float HeightMap::getHeightScale() const {return heightScale;}
inline float HeightMap::getGroundScale() const {return groundScale;}

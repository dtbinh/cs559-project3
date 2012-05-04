/************************************************************************/
/* Objects
/* ------
/* A class that contains all objects in the scene
/************************************************************************/

#include "Objects.h"
#include <glm\glm.hpp>
#include "../Utility/RenderUtils.h"
#include <SFML\Graphics.hpp>
#include "../Particles/Particles.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Utility/Curve.h"
#include "../Utility/MathUtils.h"
#include <iostream>


static float lastTime = 0.f;

Fish::Fish(glm::vec3 pos, sf::Color color, HeightMap& heightmap, Fluid& fluid)
	: SceneObject(pos)
	 ,color(color)
	 ,theta(0)
	 ,direction(sf::Randomizer::Random(0,1) * 2 -1)
	 ,fluid(fluid)
	 ,heightmap(heightmap)
{
	quadric = gluNewQuadric();
}

Fish::~Fish()
{
	gluDeleteQuadric(quadric);
}


void Fish::update(const sf::Clock &clock)
{
	//TODO: make this a little more flashy (all 3 axes instead of one, rotate instead of flip, fix minor visual glitch)
	if(lastTime < .0001)
	{
		lastTime += clock.GetElapsedTime();
		return;
	}
	else
	{
		lastTime = 0;
	}
	float randz = sf::Randomizer::Random(0.f, .05f);
	float randx = sf::Randomizer::Random(-.02f, .02f);

	if( heightmap.heightAt(this->getPos().x + randx, this->getPos().z + randz)  > this->getPos().y - .1f)		//should fix for when fish out of fluid
	{		//need to turn
		transform[0][0] = glm::cos(theta);
		transform[2][0] = glm::sin(theta);
		transform[0][2] = -glm::sin(theta);
		transform[2][2] = glm::cos(theta);
		transform[1][1] = 1;
		theta += .1f * direction;
		transform[3][0] = transform[3][0] + (randx * glm::cos(theta)) + (randz * glm::sin(theta));
		transform[3][2] = transform[3][2] + (randz * glm::cos(theta)) + (randx * glm::sin(theta));
	}
	else
	{
		direction = sf::Randomizer::Random(0,1) * 2 - 1;
		transform[3][0] = transform[3][0] + (randx * glm::cos(theta)) + (randz * glm::sin(theta));
		transform[3][2] = transform[3][2] + (randz * glm::cos(theta)) + (randx * glm::sin(theta));
	}

}

void Fish::draw(const Camera& camera)
{
	float size = .2f;
	
	float radiusx = size;
	float radiusy = size/4.f;
	float height = size;

	glDisable(GL_TEXTURE_2D);

	glColor3ub(color.r, color.g, color.b);

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform));
		glRotatef(90.f, 1.f, 0.f, 0.f);
		glPushMatrix();
			glRotatef(180, 1, 0, 0);
			glBegin(GL_QUADS);
				glVertex3d(-radiusy, 0, -radiusx);
				glVertex3d(-radiusy, 0, radiusx);
				glVertex3d(radiusy, 0, radiusx);
				glVertex3d(radiusy, 0, -radiusx);
			glEnd();
		glPopMatrix();

		glBegin(GL_TRIANGLE_FAN);
			glVertex3d(0, height, 0);
			glVertex3d(-radiusy, 0, -radiusx);
			glVertex3d(-radiusy, 0, radiusx);
			glVertex3d(radiusy, 0, radiusx);
			glVertex3d(radiusy, 0, -radiusx);
			glVertex3d(-radiusy, 0, -radiusx);
		glEnd();

		float xscale = .1f;
		float yscale = .2f;
		float zscale = .35f;

		glPushMatrix();
			glTranslatef(0.f, height + zscale, 0.f);			
			glScalef(xscale, zscale, yscale);
			gluSphere(quadric, 1.f, 10, 10);
		glPopMatrix();

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
}

Fountain::Fountain(glm::vec3 pos, float size, ParticleEmitter& emitter)
	: SceneObject(pos)
	 ,texture(GetImage("fountain.png"))
	 ,emitter(emitter)
	 ,size(size)
	 ,count(0)
{
	const unsigned int sz = static_cast<unsigned int>(size);
	fluid = new Fluid(
		2*(sz*2) + 1,   // number of vertices wide
		2*(sz*4) + 1,   // number of vertices high
		0.25f,  // distance between vertices
		0.03f, // time step for evaluation
		4.f,  // wave velocity
		0.4f   // fluid viscosity
		,pos.x - (size/2.f)
		,pos.y + .25f
		,pos.z - (size)
	);
}

Fountain::~Fountain()
{
	delete fluid;
}

void Fountain::update(const sf::Clock &clock)
{
	std::for_each(emitter.getParticles().begin(), emitter.getParticles().end(),
			[&](Particle& particle)
	{
		glm::vec3 translated = glm::vec3((particle.position.x - fluid->pos.x) / fluid->getDist()
										, particle.position.y
										,(particle.position.z - fluid->pos.z) / fluid->getDist());
			
		if( translated.x < 0 || translated.z < 0
		 || translated.x > fluid->getWidth() 
		 || translated.z > fluid->getHeight() )
		{
			particle.active = false;
		}
		if(particle.position.y <= fluid->pos.y)
		{
			fluid->displace( translated.x
						   , translated.z
						   , 1.f / (emitter.getMaxParticles() * 100)
						   , particle.velocity.y * 2.5f);
			particle.active = false;
		}
	});
	fluid->evaluate();
}

void Fountain::draw(const Camera& camera)
{
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform));
		texture.Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float l =  1.f * size;
		float w =  0.5f * size;
		const float height = .5f;

		// Inside
		glBegin(GL_QUADS);
			glNormal3d( 0,0,1);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size,    0.f);glVertex3d(w,-height,-l);
				glTexCoord2f(size,    1.f);glVertex3d(w,height,-l);
				glTexCoord2f(0.f,   1.f);glVertex3d(-w,height,-l);
			glNormal3d( 0, 0, -1);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height, -l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(w,-height, l);
				glTexCoord2f(size * 2,    1.f);glVertex3d(w,height, l);
				glTexCoord2f(0.f,   1.f);glVertex3d(w,height, -l);
			glNormal3d( 1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height,l);
				glTexCoord2f(size, 0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size, 1.f);glVertex3d(-w,height,l);
				glTexCoord2f(0.f,   1.f);glVertex3d(w,height,l);
			glNormal3d(-1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2, 0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2, 1.f);glVertex3d(-w,height,-l);
				glTexCoord2f(0.f,   1.f);glVertex3d(-w,height,l);
		glEnd();
		

		l += 1;
		w += 1;

		//outside
		glBegin(GL_QUADS);
			glNormal3d(-1,0,0);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(0.f, 0.f);glVertex3d(w,-height,l);
				glTexCoord2f(0.f, 1.f);glVertex3d(w,height,l);
				glTexCoord2f(size * 2,   1.f);glVertex3d(-w,height,l);

			glNormal3d( 1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(0.f,  1.f);glVertex3d(-w,height,-l);
				glTexCoord2f(size, 1.f);glVertex3d(w,height,-l);
				glTexCoord2f(size,   0.f);glVertex3d(w,-height,-l);

			glNormal3d( 0, 0, -1);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,-height, -l);
				glTexCoord2f(0.f,    1.f);glVertex3d(w,height, -l);
				glTexCoord2f(size * 2,    1.f);glVertex3d(w,height, l);
				glTexCoord2f(size * 2,   0.f);glVertex3d(w,-height,l);


			glNormal3d( 0,0,-1);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2,    1.f);glVertex3d(-w,height,l);
				glTexCoord2f(0.f,   1.f);glVertex3d(-w,height,-l);

			//bottom
			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,-height,-l);
				glTexCoord2f(size * 2,    0.f);glVertex3d(-w,-height,l);
				glTexCoord2f(size * 2,    size);glVertex3d(w,-height,l);
				glTexCoord2f(0.f,   size);glVertex3d(w,-height,-l);
		glEnd();

		//top
		glBegin(GL_QUADS);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,height,-l);
				glTexCoord2f(0.f,    1.f);glVertex3d(-w,height,-l+ 1);
				glTexCoord2f(size,   1.f);glVertex3d(w,height,-l+ 1);
				glTexCoord2f(size,   0.f);glVertex3d(w,height,-l);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,height,l-1);
				glTexCoord2f(0.f,    1.f);glVertex3d(-w,height,l);
				glTexCoord2f(size,   1.f);glVertex3d(w,height,l);
				glTexCoord2f(size,   0.f);glVertex3d(w,height,l-1);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w+1,height,-l+1);
				glTexCoord2f(0.f,    1.f);glVertex3d(-w,height,-l+1);
				glTexCoord2f(size*2,   1.f);glVertex3d(-w,height,l-1);
				glTexCoord2f(size*2,   0.f);glVertex3d(-w+1,height,l-1);

			glNormal3d( 0,1,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w-1,height,l-1);
				glTexCoord2f(0.f,    1.f);glVertex3d(w,height,l-1);
				glTexCoord2f(size*2,   1.f);glVertex3d(w,height,-l+1);
				glTexCoord2f(size*2,   0.f);glVertex3d(w-1,height,-l+1);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();

	fluid->render();
}

Bush::Bush(glm::vec3 pos, float size) : 
	 SceneObject(pos)
	,side(GetImage("bush-side.png"))
	,top(GetImage("bush-top.png"))
	,size(size)
{
};

Bush::~Bush()
{

}

void Bush::draw(const Camera& camera)
{
	glPushMatrix();
			// Move the particle into position and scale it  
		const glm::mat4 newTransform(
			glm::scale( glm::translate( glm::mat4(1.0), glm::vec3(transform[3][0], transform[3][1], transform[3][2]) )
					, glm::vec3(1.f, 1.f, 1.f) )
		);
		glMultMatrixf(glm::value_ptr(newTransform));

		// Undo the camera translation and get the inverse rotation
		const glm::mat4 inverseCameraRotation(
			glm::inverse( glm::translate( camera.view(), camera.position() ) )
		);

		const float halfScale = -0.5f;
		const glm::vec3  originCentered(halfScale, halfScale, 0.f);
		// Move the origin to the center of the particle
		const glm::mat4 mat(	glm::translate( inverseCameraRotation, originCentered ) );

		// Apply the final matrix for the billboarded particle
		glMultMatrixf(glm::value_ptr(mat));

		glColor3f(1.0, 1.0, 1.0);

		side.Bind();
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

		float l =  size;
		float w =  size;

		glBegin(GL_QUADS);
			glNormal3d(-1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(w,l,0);
				glTexCoord2f(0.f, 1.f);glVertex3d(w,-l,0);
				glTexCoord2f(1.f, 1.f);glVertex3d(-w,-l,0);
				glTexCoord2f(1.f,   0.f);glVertex3d(-w,l,0);
			glNormal3d(1,0,0);
				glTexCoord2f(0.f,    0.f);glVertex3d(-w,l,0);
				glTexCoord2f(0.f, 1.f);glVertex3d(-w,-l,0);
				glTexCoord2f(1.f, 1.f);glVertex3d(w,-l,0);
				glTexCoord2f(1.f,   0.f);glVertex3d(w,l,0);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}


Blimp::Blimp(glm::vec3 pos, float size):
	SceneObject(pos)
	,size(size)
	,count(0)
	,btext(GetImage("blimp2.png"))
{
	quadric = gluNewQuadric();
	curve = new Curve(bspline);
	curve->clearPoints();


	const float step = TWO_PI / 5.f;
	const float radius = 30.f;
	for(float i = 0.f; i < TWO_PI; i += step)
	{
		const glm::vec3     pos(cosf(i), 0.f, sinf(i));
		const CtrlPoint point(pos * radius);
		curve->addControlPoint(point);
	}

	curve->regenerateSegments();
}


Blimp::~Blimp()
{
	gluDeleteQuadric(quadric);
}

void Blimp::update(const sf::Clock &clock)
{
	//if(clock.GetElapsedTime() - count > CLOCKS_PER_SEC/30) count = clock.GetElapsedTime();
	count += clock.GetElapsedTime() * 1000;
	if (count >= 5) count = 0;
	
	curve->selectedSegment = static_cast<int>(std::floor(count));
	const glm::vec3& p(curve->getPosition(count));
	const glm::vec3& d(curve->getDirection(count));
	const glm::vec3& tangent(glm::normalize(d));

	glm::vec3 normal = curve->getSegment(curve->selectedSegment)->getStartPoint().pos();

	const glm::vec3 binormal(glm::normalize(glm::cross(normal, tangent)));
	normal = glm::normalize(glm::cross(tangent, binormal));

	// Apply the orientation + translation matrix
	const glm::vec3& z(tangent), y(normal), x(binormal);
	newTransform = glm::mat4x4(
		x.x, x.y, x.z, 0.f,
		y.x, y.y, y.z, 0.f,
		z.x, z.y, z.z, 0.f,
		p.x, p.y, p.z, 1.f
	);
}
	

void Blimp::draw(const Camera& camera)
{
	glEnable(GL_TEXTURE_2D);


	glPushMatrix();
		btext.Bind();	
		glMultMatrixf(glm::value_ptr(transform));
		glPushMatrix();
			glMultMatrixf(glm::value_ptr(newTransform));
			glRotatef(90, 0, 1, 0);
			//glRotatef(90, 0, 0, 1);
			gluQuadricDrawStyle( quadric, GLU_FILL);
			gluQuadricNormals( quadric, GLU_SMOOTH);
			gluQuadricOrientation( quadric, GLU_OUTSIDE);
			gluQuadricTexture( quadric, GL_TRUE);
			glColor3f(1.0, 1.0, 1.0);
			glScalef(size, size*2, size);
			gluSphere(quadric, 1.f, 20, 20);
			glTranslatef(0, 0, size/16);
			gluQuadricTexture( quadric, GL_FALSE);
			glDisable(GL_BLEND);
			glColor4ub(128, 128, 128, 255);
			gluQuadricNormals( quadric, GLU_SMOOTH);
			gluCylinder(quadric, size/16, size/16, size/16, 20, 20);
			glTranslatef(0, 0, size/16);
			gluQuadricNormals( quadric, GLU_SMOOTH);
			gluDisk(quadric, 0, size/16, 20, 20);
		glPopMatrix();
	glPopMatrix();
	glEnable(GL_BLEND);
}

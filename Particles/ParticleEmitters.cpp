/************************************************************************/
/* ParticleEmitters
/* ----------------
/* Subclasses of ParticleEmitter 
/************************************************************************/
#include "ParticleEmitters.h"
#include "ParticleEmitter.h"
#include "ParticleAffectors.h"
#include "Particle.h"
#include "../Core/ImageManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

using namespace glm;


/************************************************************************/
/* ExplosionEmitter
/* Emit particles once from "position" with velocities 
/* that cause them to move outward in a spherical shape.
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
ExplosionEmitter::ExplosionEmitter(const vec3& position
								 , const unsigned int maxParticles
								 , const float lifetime)
	: ParticleEmitter(maxParticles, lifetime)
{
	add(new ScaleDownAffector(this, 0.f, 10.f));
	add(new FadeOutAffector(this, 0.f, 10.f));
	add(new ForceAffector(this, vec3(0,-25,0)));

	setBlendMode(ALPHA);
	setPosition(position);
	setOneTimeEmission(true);
	setOneTimeNumParticles(maxParticles);
	setTexture(&GetImage("particle-dot.png"));
}

void ExplosionEmitter::initParticle(Particle& p)
{
	Particle pp;

	pp.position     = position;
	pp.prevPosition = position;

	pp.velocity = sphericalRand(60.f);
	pp.accel    = -0.1f * pp.velocity;

	pp.color = vec4(linearRand(0.3f, 1.f)
				  , linearRand(0.3f, 1.f)
				  , linearRand(0.3f, 1.f)
				  , 1.f);

	pp.lifespan = 1.f;
	pp.scale = linearRand(0.1f, 0.4f);

	pp.active = true;

	p = pp;
}


/************************************************************************/
/* FountainEmitter 
/* Emit particles continuously from "position" with velocities 
/* mostly upwards velocities and slightly to either side. 
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
FountainEmitter::FountainEmitter( const glm::vec3& position
								, const unsigned int maxParticles
								, const float lifetime)
	: ParticleEmitter(maxParticles, lifetime)
{
	add(new ScaleDownAffector(this, 0.f, 10.f));
	add(new FadeOutAffector(this, 0.f, 1.f));
	add(new ForceAffector(this, vec3(0,-20.f,0)));

	setBlendMode(ALPHA);
	setPosition(position);
	setOneTimeEmission(false);
	setTexture(&GetImage("particle-droplet.png"));

	setEmissionRate(10000.f);
}

void FountainEmitter::initParticle( Particle& p )
{
	Particle pp;

	pp.position     = position;
	pp.prevPosition = position;

	pp.velocity = vec3(linearRand(-7.f, 7.f)
					 , linearRand(40.f, 80.f)
					 , linearRand(-7.f, 7.f));
	pp.accel = vec3(0,0,0);

	pp.color = vec4(0, linearRand(0.6f, 0.9f), 1, 1);

	pp.lifespan = 0.5f;
	pp.scale = linearRand(0.3f, 0.5f);

	pp.active = true;

	p = pp;
}


/************************************************************************/
/* WaterEmitter 
/* James's edits
/* Affectors: FadeOut, ScaleDown, Force (gravity) 
/************************************************************************/
WaterEmitter::WaterEmitter( const glm::vec3& position
								, const unsigned int maxParticles
								, const float lifetime)
	: ParticleEmitter(maxParticles, lifetime)
{
	add(new ScaleDownAffector(this, 0.f, 10.f));
	add(new FadeOutAffector(this, 0.f, 20.f));
	add(new ForceAffector(this, vec3(0,-1.f,0)));

	setBlendMode(ALPHA);
	setPosition(position);
	setOneTimeEmission(false);
	setTexture(&ImageManager::get().getImage("particle-droplet.png"));

	setEmissionRate(10000.f);
}

void WaterEmitter::initParticle( Particle& p )
{
	Particle pp;

	pp.position     = position;
	pp.prevPosition = position;

	pp.velocity = vec3(linearRand(-2.5f, 2.5f)
					 , linearRand(2.5f, 2.5f)
					 , linearRand(-2.5f, 2.5f));
	pp.accel = vec3(0,0,0);

	pp.color = vec4(0, 0.8f, 1, 1);

	pp.lifespan = 1.5f;
	pp.scale = linearRand(0.2f, 0.4f);

	pp.active = true;

	p = pp;
}


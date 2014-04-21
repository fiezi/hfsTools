#ifndef _ASTEROID
#define _ASTEROID




#include "actor.h"
#include "renderer.h"


class particleStream : public Actor{

	public:

        particleStream();
        virtual ~particleStream();

		void setup();
		void setup(float mySize,int particleCount, float particleSpeed, int myDrawType);
		void update();
        void spawnParticle(Vector3f spawnLocation,int p=0);
        void remove();

        Vector3f            initialOffset;
        float               particleFieldSize;
        float               speed;
        float               minSpeed;
        int                 numParticles;
        int                 particleDrawType;
        bool                bConstantUpdate;
        bool                bRespawnTheDead;
        vector<Actor*>      myParticles;

};

#endif

#include "particleStream.h"


particleStream::particleStream(){

    bConstantUpdate=true;
    bRespawnTheDead=true;
    minSpeed=0.0;
    textureID="grid_solid";
    initialOffset=Vector3f(0,0,40);
    scale=Vector3f(0.4,0.4,0.4);

}

particleStream::~particleStream(){


}

//--------------------------------------------------------------
void particleStream::setup(){

    Actor::setup();

    particleFieldSize=20;
    numParticles=100;
    speed=0.5;
    particleDrawType=DRAW_PLANE;

    srand(15);
    for (int i=0;i<numParticles;i++)
        spawnParticle(Vector3f(particleFieldSize * float(rand()%200-100.0)/100.0,particleFieldSize *float(rand()%200-100.0)/100.0,0)+initialOffset);
}

void particleStream::setup(float mySize,int particleCount, float particleSpeed, int myDrawType){

    Actor::setup();

    particleFieldSize=mySize;
    numParticles=particleCount;
    speed=particleSpeed;
    particleDrawType=myDrawType;

    srand(15);
    for (int i=0;i<numParticles;i++)
        spawnParticle(Vector3f(particleFieldSize * float(rand()%200-100.0)/100.0,particleFieldSize *float(rand()%200-100.0)/100.0,0)+initialOffset,i);
}


void particleStream::spawnParticle(Vector3f spawnLocation, int p){

    Actor* particle= new Actor;
    particle->setLocation(spawnLocation+Vector3f(0,0,0.1)* p);
    particle->setRotation(Vector3f(0,180,0));
    particle->postLoad();
    particle->drawType=particleDrawType;
    particle->bTextured=true;
    particle->textureID=textureID;
    particle->sceneShaderID="texture";
    particle->setScale(scale);
    particle->birth=float(rand()%100)/100.0 *speed + minSpeed;
    particle->name=name;

    renderer->actorList.push_back(particle);
    myParticles.push_back(particle);
    renderer->layerList[0]->actorList.push_back(particle);

}

//--------------------------------------------------------------
void particleStream::update(){

    if (!bConstantUpdate)
        return;

    if (myParticles.size()< numParticles && bRespawnTheDead){
        for (int i=0;i<numParticles-myParticles.size();i++){
            spawnParticle(Vector3f(particleFieldSize *float(rand()%200-100.0)/100.0,particleFieldSize *float(rand()%200-100.0)/100.0,0)+initialOffset,i);
        }
    }

    for (int i=0;i<myParticles.size();i++){

        Actor* ast= myParticles[i];
        if (!ast){
            break;
        }
        Vector3f moveSpeed= Vector3f(0,0,-ast->birth*renderer->deltaTime*0.01);
        ast->setLocation(ast->location+moveSpeed);
        if (ast->location.z<0){
            ast->remove();
            //remove from myParticles vector
            myParticles.erase(myParticles.begin()+i);
            //reset count, so we don't step over
            i--;
        }
    }
}

void particleStream::remove(){

    for (int i=0;i<myParticles.size();i++)
        myParticles[i]->remove();
    myParticles.clear();

    bConstantUpdate=false;
}


#include "testApp.h"

#ifdef TARGET_WIN32

	#include <windows.h>
	#include <stdio.h>
	#include <conio.h>
	#include <tchar.h>

	TCHAR szName[]=TEXT("Global\\KinectSharedMemory");
#else

	#include <sys/mman.h>

#endif

#include "actor.h"
#include "sliderButton.h"
#include "textInputButton.h"
#include "assignButton.h"
#include "msbLight.h"

#define BUF_SIZE 640*480*4*sizeof(KINECTSIZE)

testApp::testApp(){}

testApp::~testApp(){

    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    TiXmlElement * root = new TiXmlElement( "MsbKinect" );
    doc.LinkEndChild( root );

    TiXmlElement* myElement=save(root);
    root->LinkEndChild(myElement);
    doc.SaveFile("userConfig.xml");

}

//--------------------------------------------------------------
void testApp::setup(){

    //ofDisableArbTex();

    //ofSetFrameRate(30);

    currentState=STARS;

    bFullscreen=false;
    cutOffDepth=4096;
    bSetCutoffToZero=false;
    thresh= 48;
	starfieldSize=20;
	numAsteroids=1000;

    msbSetup();
	kinect.bImage=true;


    //interface setup
    interfaceSetup();

    //OF_STUFF

    loadSettings();
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();

	kinect.cutOffFar=cutOffDepth;

	numAsteroids=0;
	myVideo.loadMovie("omg-cat.gif");
	myVideo.setLoopState(OF_LOOP_NONE);
    //myVideo.play();


}

void testApp::msbSetup(){

    //MSB setup
    renderer=Renderer::getInstance();
    input=Input::getInstance();

    renderer->loadPreferences();

    renderer->windowX=SCREENRESX;
    renderer->windowY=SCREENRESY;

    renderer->screenX=SCREENRESX;
    renderer->screenY=SCREENRESY;

    renderer->setup();

    renderer->camActor=new Actor;
    renderer->camActor->setLocation(Vector3f(0,0,0));

    renderer->camActor->postLoad();


    //MSB light
    MsbLight* myActor = new MsbLight;
    myActor->setLocation(Vector3f(0,0.0,-2));
    myActor->postLoad();
    myActor->color=Vector4f(0.5,0.5,1,1)*0.25;
    myActor->lightDistance=64;
    myActor->particleScale=1;
    renderer->actorList.push_back(myActor);
    renderer->lightList.push_back((MsbLight*)myActor);
    renderer->layerList[0]->actorList.push_back(myActor);

    myStars= new particleStream;
    myStars->name="stars";
    myStars->setup(20.0,100,1.5,DRAW_PLANE);

    myAsteroid= new particleStream;
    myAsteroid->name="asteroids";
    myAsteroid->bRespawnTheDead=false;
    myAsteroid->minSpeed=1.0;


    registerProperties();

}

void testApp::interfaceSetup(){

    TextInputButton* txt = new TextInputButton;
    txt->setLocation(Vector3f(10,300,0));
    txt->name="cutOffDepth";
    txt->buttonProperty="CUTOFFDEPTH";

    txt->bDrawName=true;
    txt->tooltip="";
    txt->scale.x=130;
    txt->scale.y=20;
    txt->textureID="icon_flat";
    txt->parent=this;
    txt->color=Vector4f(0.5,0.5,0.5,1.0);
    txt->setup();
    renderer->buttonList.push_back(txt);

    kinectDisplay = new BasicButton;
    kinectDisplay->setLocation(Vector3f(50,400,0));
    kinectDisplay->scale.x=320;
    kinectDisplay->scale.y=240;
    kinectDisplay->bTextured=true;
    kinectDisplay->textureID="a_sign";
    kinectDisplay->sceneShaderID="zeichen";
    kinectDisplay->color=Vector4f(1.0,1.0,1.0,1.0);
    kinectDisplay->setup();
    renderer->buttonList.push_back(kinectDisplay);
}

void testApp::registerProperties(){

   createMemberID("CUTOFFDEPTH",&cutOffDepth,this);
   createMemberID("BSETCUTOFFTOZERO",&bSetCutoffToZero,this);
   createMemberID("THRESH",&thresh,this);

}


//--------------------------------------------------------------
void testApp::update(){

	ofBackground(100, 100, 100);

    renderer->camActor->setRotation(Vector3f(0,0,ofGetElapsedTimef()*1.0));
    renderer->camActor->update(renderer->deltaTime);

    //renderer->camActor->setLocation(Vector3f(sin(ofGetElapsedTimef()*0.01),0,0));

	kinect.update();
    kinectDisplay->ofTexturePtr=&kinect.sensors[0].depthTex.getTextureReference();

    renderer->update();
    myStars->update();
    myAsteroid->update();

    checkKinect();

    if (currentState==STARS){
    //switch to next stage!
        if (numAsteroids>100){
            myStars->remove();
            myStars->textureID="bloodVessel";
            myStars->minSpeed=0.1;
            myStars->initialOffset=Vector3f(0,0,10);
            myStars->setScale(Vector3f(1,1,1));
            myStars->setup(0.1,100,0.0,DRAW_PLANE);
            myStars->bConstantUpdate=true;
            numAsteroids=0;
            currentState=VIDEO;
            myVideo.play();

            myAsteroid->remove();
            myAsteroid->bConstantUpdate=true;
            myAsteroid->minSpeed=0.25;
            myAsteroid->setScale(Vector3f(0.25,0.25,0.25));
        }

        if ((ofGetElapsedTimeMillis()/100)%100==0)
            sendMeteors();
    }


    if (currentState==VIDEO){
        myVideo.update();
        if (myVideo.getIsMovieDone()){
            currentState=BLOOD;
            cout << "yo! we finished!" << endl;
        }
    }

    if (currentState==BLOOD){
        if ((ofGetElapsedTimeMillis()/100)%100==0)
            myAsteroid->setup(0.2,100,0.25,DRAW_CUBE);

    }


/*
    cvImage.threshold(thresh,true);
    contourFinder.findContours(cvImage, 256, 80000, 10, false);
*/
}


//find closest point to kinect, use as mouse for picking
void testApp::checkKinect(){

    splodeX=0;
    splodeY=0;


    unsigned char* myPixels;//= new unsigned char[640*480];

    myPixels=kinect.sensors[0].depthPixels;

    unsigned char lowest=255;
    int coord=0;

    for (int i=0;i<640*480;i++){
        if (myPixels[i]<lowest){
            lowest=myPixels[i];
            coord=i;
        }
    }
    if (lowest >100 || lowest==0)
        return;

    int x,y;


    y=coord/640;
    x=coord%640;

    renderer->pick(x*3,int(float(y)*2.25));
    if(input->worldTarget && input->worldTarget->name=="asteroids"){
        //input->worldTarget->remove();
        input->worldTarget->drawType=DRAW_TEA;
        input->worldTarget->setScale(Vector3f(1,1,1));
        cout << "I hit an asteroid!!!!!!!!!!" << endl;
        numAsteroids++;
    }

    splodeX=x*3;
    splodeY=y*2.25;

}

//--------------------------------------------------------------
void testApp::draw(){

    glEnable(GL_LIGHTING);
    renderer->draw();
    glDisable(GL_LIGHTING);

    ofSetColor(255, 255, 255);

    ofCircle(splodeX,splodeY,20.0);
    char buffer [30];
    sprintf(buffer,"amount of Asteroids turned into teapots: %i",numAsteroids);
    ofDrawBitmapString(buffer,1500,50);

    sprintf(buffer,"time: %i",ofGetElapsedTimeMillis()/1000);
    ofDrawBitmapString(buffer,1500,100);

    if (currentState==VIDEO)
        myVideo.draw(0,0,1920,1440);

}

void testApp::sendMeteors(){

    myAsteroid->setup(1.0,100,0.75,DRAW_CUBE);
}

//--------------------------------------------------------------
void testApp::exit(){


}

//--------------------------------------------------------------
void testApp::keyPressed (int key){

    input->normalKeyDown(key,mouseX,mouseY);
    input->specialKeyDown(key,mouseX,mouseY);

    if (key=='f')
        bFullscreen=!bFullscreen;

}

void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);
    input->specialKeyUp(key,mouseX,mouseY);

    if (key=='m')
        cvMaskBase.setFromPixels(cvImage.getPixels(),640,480);

    if (key=='x')
        myStars->bRespawnTheDead=false;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

    input->moveMouse(x,y);

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    input->dragMouse(x,y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    input->pressedMouse(button,0,x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

    input->pressedMouse(button,1,x,y);

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}

void testApp::trigger(Actor* other){

    if (other->name=="cutOffDepth")
        kinect.cutOffFar=cutOffDepth;


    if (other->name=="MakeMask"){
        cvMaskBase.setFromPixels(cvImage.getPixels(),640,480);
    }

}


void testApp::loadSettings(){

    //load settings
    TiXmlDocument doc( "userConfig.xml" );
    if (!doc.LoadFile()) return;
    TiXmlHandle hDoc(&doc);
    TiXmlElement * element;
    TiXmlHandle hRoot(0);
    element=hDoc.FirstChildElement().Element();
    if (!element) return;

    hRoot=TiXmlHandle(element);
    element=hRoot.FirstChild( "Actor" ).Element();
    int listPos=0;
    string myType;

      for( ; element!=NULL; element=element->NextSiblingElement("Actor"))
        {
        myType=element->GetText();
        cout << "Loading property type: " << myType << endl;
        load(element);
        listPos++;
        }
}

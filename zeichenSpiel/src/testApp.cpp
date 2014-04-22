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

    ofSetFrameRate(30);

    bFullscreen=false;
    cutOffDepth=4096;

	kinect.bImage=true;
    xOffset=960;
    yOffset=240;

	circleX=0;
	circleY=333;
	circleR=90;
	ofSetCircleResolution(100);

	bFullSystem = false;
	bLectureGroup = true;
	systemVoiceSize=90;

	//left = new float[256];

	//bufferCounter = 0;
	//drawCounter = 0;

    //SOUND INPUT
    amp = new float[1024];
    bufferCounter = 0;
    ofSoundStreamListDevices();

    mySound.setDeviceID(0);
    mySound.setup(this,2,1, 22050, 1024, 4);

    msbSetup();

    //interface setup
    interfaceSetup();

    //OF_STUFF

    loadSettings();
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();

	kinect.cutOffFar=cutOffDepth;


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
    renderer->camActor->setLocation(Vector3f(0,0,-5));
    renderer->camActor->postLoad();

    registerProperties();

}

void testApp::interfaceSetup(){

    setupManageInterface();
    setupKinectInterface();
}

void testApp::setupManageInterface(){

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

    txt = new TextInputButton;
    txt->setLocation(Vector3f(10,320,0));
    txt->name="circleX";
    txt->buttonProperty="CIRCLEX";

    txt->bDrawName=true;
    txt->tooltip="";
    txt->scale.x=130;
    txt->scale.y=20;
    txt->textureID="icon_flat";
    txt->parent=this;
    txt->color=Vector4f(0.5,0.5,0.5,1.0);
    txt->setup();
    renderer->buttonList.push_back(txt);

    txt = new TextInputButton;
    txt->setLocation(Vector3f(10,340,0));
    txt->name="circleY";
    txt->buttonProperty="CIRCLEY";
    txt->bDrawName=true;
    txt->tooltip="";
    txt->scale.x=130;
    txt->scale.y=20;
    txt->textureID="icon_flat";
    txt->parent=this;
    txt->color=Vector4f(0.5,0.5,0.5,1.0);
    txt->setup();
    renderer->buttonList.push_back(txt);

    txt = new TextInputButton;
    txt->setLocation(Vector3f(10,360,0));
    txt->name="circleR";
    txt->buttonProperty="CIRCLER";
    txt->bDrawName=true;
    txt->tooltip="";
    txt->scale.x=130;
    txt->scale.y=20;
    txt->textureID="icon_flat";
    txt->parent=this;
    txt->color=Vector4f(0.5,0.5,0.5,1.0);
    txt->setup();
    renderer->buttonList.push_back(txt);
}

void testApp::setupKinectInterface(){

    kinectDisplay = new BasicButton;
    kinectDisplay->setLocation(Vector3f(xOffset+160,yOffset+240,0));
    kinectDisplay->setRotation(Vector3f(0,0,180));
    kinectDisplay->scale.x=320;
    kinectDisplay->scale.y=240;
    kinectDisplay->bTextured=true;
    kinectDisplay->textureID="alone_sign";
    kinectDisplay->sceneShaderID="zeichen";
    kinectDisplay->color=Vector4f(1.0,1.0,1.0,1.0);
    kinectDisplay->setup();
    renderer->buttonList.push_back(kinectDisplay);

    kinectDisplayTwo = new BasicButton;
    kinectDisplayTwo->setLocation(Vector3f(xOffset+1,yOffset+518,0));
    kinectDisplayTwo->setRotation(Vector3f(0,0,60));
    kinectDisplayTwo->scale.x=320;
    kinectDisplayTwo->scale.y=240;
    kinectDisplayTwo->bTextured=true;
    kinectDisplayTwo->textureID="mean_sign";
    kinectDisplayTwo->sceneShaderID="zeichen";
    kinectDisplayTwo->color=Vector4f(1.0,1.0,1.0,1.0);
    kinectDisplayTwo->setup();
    renderer->buttonList.push_back(kinectDisplayTwo);

    kinectDisplayThree = new BasicButton;
    kinectDisplayThree->setLocation(Vector3f(xOffset-160,yOffset+240,0));
    kinectDisplayThree->setRotation(Vector3f(0,0,-60));
    kinectDisplayThree->scale.x=320;
    kinectDisplayThree->scale.y=240;
    kinectDisplayThree->bTextured=true;
    kinectDisplayThree->textureID="happy_sign";
    kinectDisplayThree->sceneShaderID="zeichen";
    kinectDisplayThree->color=Vector4f(1.0,1.0,1.0,1.0);
    kinectDisplayThree->setup();
    renderer->buttonList.push_back(kinectDisplayThree);
}

void testApp::registerProperties(){

   createMemberID("CUTOFFDEPTH",&cutOffDepth,this);
   createMemberID("CIRCLEX",&circleX,this);
   createMemberID("CIRCLEY",&circleY,this);
   createMemberID("CIRCLER",&circleR,this);

}

//--------------------------------------------------------------
void testApp::update(){

	kinect.update();

    renderer->update();

    if (bLectureGroup){
        kinectDisplay->sceneShaderID="texture";
        kinectDisplayTwo->sceneShaderID="texture";
        kinectDisplayThree->sceneShaderID="texture";
    }else{
        kinectDisplay->sceneShaderID="zeichen";
        kinectDisplayTwo->sceneShaderID="zeichen";
        kinectDisplayThree->sceneShaderID="zeichen";
    }


    kinectDisplay->ofTexturePtr=&kinect.sensors[0].depthTex.getTextureReference();
    kinectDisplayTwo->ofTexturePtr=&kinect.sensors[0].depthTex.getTextureReference();
    kinectDisplayThree->ofTexturePtr=&kinect.sensors[0].depthTex.getTextureReference();

    if (bFullSystem){
        systemVoiceSize-=renderer->deltaTime * 0.1;
        if (systemVoiceSize<circleR)
            systemVoiceSize=circleR;
    }else{
        systemVoiceSize+=renderer->deltaTime * 0.1;
        if (systemVoiceSize>370)
            systemVoiceSize=370;
    }

/*
    cvImage.threshold(thresh,true);
    contourFinder.findContours(cvImage, 256, 80000, 10, false);
*/
}

//--------------------------------------------------------------
void testApp::draw(){


	ofBackground(0, 0, 0);
    ofSetColor(255, 255, 255);

    //kinect.draw(400,50,320,240,0);
    kinect.drawDepth(50, 50, 320, 240,0);

    ofPushMatrix();
        ofTranslate(xOffset,yOffset);
        drawComputer();
    ofPopMatrix();

    glEnable(GL_LIGHTING);
    renderer->draw();
    glDisable(GL_LIGHTING);

/*
    if (bLectureGroup){
        ofPushMatrix();
            ofTranslate(xOffset,yOffset);
            ofPushMatrix();
                ofTranslate(-160, 0);
                ofRotate(180);
                ofTranslate(-320, -240);
                kinect.drawDepth(0, 0, 320, 240,0);
                //cvFinal.draw(960-160, 0, 320, 240);
            ofPopMatrix();
            ofPushMatrix();
                ofTranslate(-160,240);
                ofRotate(60);
                kinect.drawDepth(0, 0, 320, 240,0);
                //cvFinal.draw(0, 0, 320, 240);
            ofPopMatrix();

            ofPushMatrix();
                ofTranslate(160,240);
                ofRotate(-60);
                ofTranslate(-320,0);
                kinect.drawDepth(0, 0, 320, 240,0);
                //cvFinal.draw(0,0,320, 240);
            ofPopMatrix();
        ofPopMatrix();
    }
*/

    ofPushMatrix();
        ofTranslate(xOffset,yOffset);
        drawSystemVoice();
    ofPopMatrix();






}


void testApp::drawComputer(){


        ofSetColor(0, 0, 0);
        ofCircle(circleX,circleY,370);
        ofSetColor(255, 255, 255);

        ofNoFill();
        ofCircle(circleX,circleY,370);
        ofFill();

        ofSetColor(255, 255, 255);
        ofPushMatrix();
            ofTranslate(0,circleY);
            ofLine(0,0,0,370);
            ofRotate(120);
            ofLine(0,0,0,370);
            ofRotate(120);
            ofLine(0,0,0,370);
        ofPopMatrix();
}

void testApp::drawSystemVoice(){

        ofSetColor(0, 0, 15);
        ofCircle(circleX,circleY,systemVoiceSize);

        ofSetColor(systemVoice/ 2.0+200, systemVoice/8.0+120, 0);
        ofCircle(circleX,circleY,systemVoiceSize-5);

        ofSetColor(255, 255, 255);
        ofNoFill();
        ofCircle(circleX,circleY,systemVoiceSize);
        ofFill();

}



void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){
	// samples are "interleaved"
	for (int i = 0; i < bufferSize; i++){
		amp[i] = input[i];
	}
	bufferCounter++;


	systemVoice=0.0f;
	for (int i=0;i<1024;i++)
        systemVoice+=abs(amp[i]);
}

//--------------------------------------------------------------
void testApp::exit(){

    mySound.close();
    kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key){

    input->normalKeyDown(key,mouseX,mouseY);
    input->specialKeyDown(key,mouseX,mouseY);

}

void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);
    input->specialKeyUp(key,mouseX,mouseY);

    if (key=='f')
        bFullSystem=!bFullSystem;
    if (key=='l')
        bLectureGroup=!bLectureGroup;

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

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
    bSetCutoffToZero=false;
    thresh= 48;

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
	kinect.update();

    renderer->update();
    kinectDisplay->ofTexturePtr=&kinect.sensors[0].depthTex.getTextureReference();

/*
    cvImage.threshold(thresh,true);
    contourFinder.findContours(cvImage, 256, 80000, 10, false);
*/
}

//--------------------------------------------------------------
void testApp::draw(){

    glEnable(GL_LIGHTING);
    renderer->draw();
    glDisable(GL_LIGHTING);

    ofSetColor(255, 255, 255);

    //kinect.draw(400,50,320,240,0);
    kinect.drawDepth(50, 50, 320, 240,0);

    ofPushMatrix();
        ofTranslate(0,240);
        ofPushMatrix();
            ofTranslate(960-160, 0);
            ofRotate(180);
            ofTranslate(-320, -240);
            kinect.drawDepth(0, 0, 320, 240,0);
            //cvFinal.draw(960-160, 0, 320, 240);
        ofPopMatrix();
        ofPushMatrix();
            ofTranslate(960-160,240);
            ofRotate(90);
            kinect.drawDepth(0, 0, 320, 240,0);
            //cvFinal.draw(0, 0, 320, 240);
        ofPopMatrix();

        ofPushMatrix();
            ofTranslate(960+160,240);
            ofRotate(-90);
            ofTranslate(-320,0);
            kinect.drawDepth(0, 0, 320, 240,0);
            //cvFinal.draw(0,0,320, 240);
        ofPopMatrix();

    ofPopMatrix();

        //cvImage.draw(420,50,400,300);

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

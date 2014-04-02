#include "testApp.h"


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

	camWidth 		= 640;	// try to grab at this size.
	camHeight 		= 480;

	selectedCam     =1;

	vidGrabberOne.setVerbose(true);
	vidGrabberOne.setDeviceID(0);
	vidGrabberOne.initGrabber(camWidth,camHeight);

	vidGrabberTwo.setVerbose(true);
	vidGrabberTwo.setDeviceID(0);
	vidGrabberTwo.initGrabber(camWidth,camHeight);

	vidGrabberThree.setVerbose(true);
	vidGrabberThree.setDeviceID(0);
	vidGrabberThree.initGrabber(camWidth,camHeight);

	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	videoTextureOne.allocate(camWidth,camHeight, GL_RGB);
	videoTextureTwo.allocate(camWidth,camHeight, GL_RGB);
	videoTextureThree.allocate(camWidth,camHeight, GL_RGB);

	apolloMovie.loadMovie("apollo11.mp4");
	apolloMovie.setLoopState(OF_LOOP_NONE);

    msbSetup();

    loadSettings();

    interfaceSetup();


    myFont.loadFont("verdana.ttf",150);

}


void testApp::msbSetup(){

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

void testApp::registerProperties(){


}


//--------------------------------------------------------------
void testApp::update(){

    renderer->update();
    apolloMovie.update();
	ofBackground(100,100,100);

	vidGrabberOne.update();
	vidGrabberTwo.update();
	vidGrabberThree.update();


	if (vidGrabberThree.isFrameNew()){
		int totalPixels = camWidth*camHeight*3;
		unsigned char * pixels = vidGrabberThree.getPixels();
		for (int i = 0; i < totalPixels; i++){
			videoInverted[i] = 255 - pixels[i];
		}
//		videoTexture.loadData(videoInverted, camWidth,camHeight, GL_RGB);
	}

}

//--------------------------------------------------------------
void testApp::draw(){

	ofNoFill();
	ofSetHexColor(0xffffff);

	if (selectedCam==1){
        ofSetHexColor(0xff0000);
        ofRect(10,10,340,220);
	}
	if (selectedCam==2){
        ofSetHexColor(0xff0000);
        ofRect(370,10,340,220);
	}
	if (selectedCam==3){
        ofSetHexColor(0xff0000);
        ofRect(10,250,340,220);
	}
	ofSetHexColor(0xffffff);
	vidGrabberOne.draw(20,30,320,180);
	apolloMovie.draw(380,30,320,180);

        ofFill();
        ofSetHexColor(0x555555);
        ofRect(20,270,320,180);
        ofNoFill();

        ofFill();
        ofSetHexColor(0x000000);
        ofRect(380,270,320,180);
        ofNoFill();

    ofSetHexColor(0xffffff);

	if (selectedCam==1){
        vidGrabberOne.draw(1366,0,1280,720);
	}
	if (selectedCam==2){
        apolloMovie.draw(1366,0,1280,720);
	}
	if (selectedCam==3){
        drawText();
	}
	if (selectedCam==4){
        ofFill();
        ofSetHexColor(0x000000);
        ofRect(1366,0,1280,720);
        ofNoFill();
	}


    renderer->draw();
}

void testApp::drawText(){

    ofPushMatrix();
    ofTranslate(1366,200);
    myFont.drawString(myBut->tooltip,50,50);
    ofPopMatrix();


}


void testApp::trigger(Actor* other){


    if (other->name=="thresholdSlider"){


    }

    if (other->name=="stefano"){

        cout << "hippie!!!" << endl;

    }
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	// in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...

	if (key=='1'){
        selectedCam=1;
        apolloMovie.stop();
        apolloMovie.firstFrame();
        return;
	}
	if (key=='2'){
        selectedCam=2;
        apolloMovie.play();
        return;
	}
	if (key=='3'){
        selectedCam=3;
        apolloMovie.stop();
        apolloMovie.firstFrame();
        return;
        //myBut->clickedLeft();

	}
	if (key=='4'){
        selectedCam=4;
        apolloMovie.stop();
        apolloMovie.firstFrame();
        return;
	}


    input->normalKeyDown(key,mouseX,mouseY);
    if (input->bTextInput)
        return;


	if (key == 's' || key == 'S'){
		vidGrabberOne.videoSettings();
		vidGrabberTwo.videoSettings();
		vidGrabberThree.videoSettings();
	}


    if (key==13){
        myBut->clickedLeft();
    }

}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);

    if (input->bTextInput)
        return;

    if (key==' ')
        apolloMovie.play();

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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
void testApp::windowResized(int w, int h){

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

void testApp::interfaceSetup(){

    //Adding MSB content
    BasicButton *but;

    //anderes
    but= new TextInputButton();
    but->name="TextFeld";
    but->buttonProperty="NULL";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(20,270,0));
    but->scale.x=64;
    but->scale.y=18;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);
    myBut=but;

}



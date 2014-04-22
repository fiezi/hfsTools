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

	drohneOneText="";
	drohneTwoText="";
	xOffset=400;
	yOffset=100;

    fontSizeOne=72;
    fontSizeTwo=72;

	textSpeedOne=10;
	textSpeedTwo=5;

    bShowTextOne=false;
    bShowImageOne=false;
    bShowMovieOne=false;

    bShowTextTwo=false;
    bShowImageTwo=false;
    bShowMovieTwo=false;

    textOneChar=0;
    textTwoChar=0;

    drohneOneImage.loadImage("stage.tga");
    msbSetup();

    loadSettings();

    interfaceSetup();


    drohneOneFont.loadFont("DroidSans.ttf",fontSizeOne,true,true);
    drohneTwoFont.loadFont("DroidSans.ttf",fontSizeTwo,true,true);

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

    createMemberID("TEXTSPEEDONE",&textSpeedOne,this);
    createMemberID("TEXTSPEEDTWO",&textSpeedTwo,this);
    createMemberID("FONTSIZEONE",&fontSizeOne,this);
    createMemberID("FONTSIZETWO",&fontSizeTwo,this);
}

void testApp::interfaceSetup(){

    TextInputButton *but;

    //TYPING SPEED

    but= new TextInputButton();
    but->name="txtSpeed";
    but->buttonProperty="TEXTSPEEDONE";
    but->bDrawName=true;
    but->tooltip="Drohne 1 Text Speed";
    but->setLocation(Vector3f(20,250,0));
    but->scale.x=128;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="txtSpeed";
    but->buttonProperty="TEXTSPEEDTWO";
    but->bDrawName=true;
    but->tooltip="Drohne 2 Text Speed";
    but->setLocation(Vector3f(20,550,0));
    but->scale.x=128;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);


    //FONT SIZE

    but= new TextInputButton();
    but->name="FontSizeOne";
    but->buttonProperty="FONTSIZEONE";
    but->bDrawName=true;
    but->tooltip="Drohne Font Size";
    but->setLocation(Vector3f(20,280,0));
    but->scale.x=128;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="FontSizeTwo";
    but->buttonProperty="FONTSIZETWO";
    but->bDrawName=true;
    but->tooltip="Drohne Font Size";
    but->setLocation(Vector3f(20,580,0));
    but->scale.x=128;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);


    textFieldSetup();

}

void testApp::textFieldSetup(){

    //Adding MSB content
    TextInputButton *but;

    but= new TextInputButton();
    but->name="Drohne1";
    but->buttonProperty="NULL";
    but->bDrawName=true;
    but->tooltip="Was fuer ein toller\nTag!";
    but->setLocation(Vector3f(20,100,0));
    but->scale.x=128;
    but->scale.y=32;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.0,0.5,0.0,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);
    drohneOne=but;

    but= new TextInputButton();
    but->name="Drohne2";
    but->buttonProperty="NULL";
    but->bDrawName=true;
    but->tooltip="ja, find ich auch!";
    but->setLocation(Vector3f(20,400,0));
    but->scale.x=128;
    but->scale.y=32;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.0,0.0,0.50,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);
    drohneTwo=but;

    AssignButton* bb= new AssignButton;
    bb->name="show1";
    bb->buttonProperty="NULL";
    bb->bDrawName=true;
    bb->tooltip="";
    bb->setLocation(Vector3f(20,140,0));
    bb->scale.x=64;
    bb->scale.y=64;
    bb->textureID="icon_flat";
    bb->parent=this;
    bb->color=Vector4f(0.5,0.0,0.0,1.0);
    bb->setup();
    renderer->buttonList.push_back(bb);

    bb= new AssignButton;
    bb->name="show2";
    bb->buttonProperty="NULL";
    bb->bDrawName=true;
    bb->tooltip="";
    bb->setLocation(Vector3f(20,440,0));
    bb->scale.x=64;
    bb->scale.y=64;
    bb->textureID="icon_flat";
    bb->parent=this;
    bb->color=Vector4f(0.5,0.0,0.0,1.0);
    bb->setup();
    renderer->buttonList.push_back(bb);
}



//--------------------------------------------------------------
void testApp::update(){

    //Add linebreaks to the input text
    //tooltips are updated from inputText, so the linebreaks auto-transfer
    //also reset drawing animation when editing text
    if(input->focusButton==drohneOne){
        textOneChar=0;
        input->inputText=linebreak(input->inputText, &drohneOneFont);
    }
    if(input->focusButton==drohneTwo){
        textTwoChar=0;
        input->inputText=linebreak(input->inputText, &drohneTwoFont);
    }



    // TEXT ANIMATION
    //do not update DrohneText, if we are editing it right now!
    if (bShowTextOne && input->focusButton!=drohneOne){
        textOneChar+=renderer->deltaTime*textSpeedOne*0.01;
        textOneChar=min(textOneChar,float(drohneOne->tooltip.size()));
        drohneOneText=drohneOne->tooltip.substr(0,int(textOneChar));
    }
    if (bShowTextTwo && input->focusButton!=drohneTwo){
        textTwoChar+=renderer->deltaTime*textSpeedTwo*0.01;
        textTwoChar=min(textTwoChar,float(drohneTwo->tooltip.size()));
        drohneTwoText=drohneTwo->tooltip.substr(0,int(textTwoChar));
    }

    renderer->update();

}

//--------------------------------------------------------------
void testApp::draw(){

	ofBackground(100,100,100);

    renderer->draw();

    //why we have to do this, i have no idea!
    //if we don't do it, font drawing doesn't work!
    drohneOneImage.draw(0,0,-100,-100);

    ofSetColor(255,255,255);

    ofPushMatrix();
        ofTranslate(xOffset,yOffset);
        ofScale(0.5,0.5);
        ofSetColor(20,20,30);
        ofRect(0,0,1920,1080);

        ofSetColor(255,255,255);
        ofRect(955,0,10,1080);


        // only show pictures after text os finished displaying...
        if (bShowTextOne)
            drohneOneFont.drawString(drohneOneText,5,drohneOneFont.stringHeight("A")+10);

        if (bShowImageOne && textOneChar==drohneOneText.size())
            drohneOneImage.draw(5,drohneOneFont.stringHeight(drohneOneText)+10);



        // only show pictures after text os finished displaying...
        if (bShowTextTwo)
            drohneTwoFont.drawString(drohneTwoText,970,drohneTwoFont.stringHeight("A")+10);

        if (bShowImageTwo&& textTwoChar==drohneTwoText.size())
            drohneTwoImage.draw(975,drohneTwoFont.stringHeight(drohneTwoText)+10);

    ofPopMatrix();

}


string testApp::linebreak(string text, ofTrueTypeFont* myFont){

    //always go from the back of the text
    unsigned foundBreak = text.find_last_of("\n");
    string lastLine= text.substr(foundBreak+1);
    //are we over capacity? then let's insert a linebreak at the last space we find
    if (myFont->stringWidth(lastLine)>850){
        //so let's do it!
        //Find a whitespace
         unsigned foundSpace= lastLine.find_last_of(' ');
         text.replace(foundBreak+foundSpace+1,1,"\n");
    }


    return text;

}


void testApp::trigger(Actor* other){

    if (other->name=="show1"){
        bShowTextOne=!bShowTextOne;
        textOneChar=0;
    }

    if (other->name=="show2"){
        bShowTextTwo=!bShowTextTwo;
        textTwoChar=0;
    }

    if (other->name=="Drohne1"){
        bShowTextOne=true;
        textOneChar=0;
        bShowImageOne=parseForImage(&drohneOneImage,drohneOne->tooltip);
    }

    if (other->name=="Drohne2"){
        bShowTextTwo=true;
        textTwoChar=0;
        bShowImageTwo=parseForImage(&drohneTwoImage,drohneTwo->tooltip);
    }

    if (other->name=="FontSizeOne"){
        drohneOneFont.loadFont("DroidSans.ttf",fontSizeOne,true,true);
    }

    if (other->name=="FontSizeTwo"){
        drohneTwoFont.loadFont("DroidSans.ttf",fontSizeTwo,true,true);
    }
}

bool testApp::parseForImage(ofImage* myImage, string text){

    unsigned fileStart=text.find('<');
    if (fileStart==string::npos){
        return false;
    }
    unsigned fileEnd=text.find('>');
    string imgFile=text.substr(fileStart+1,fileEnd-fileStart-1);
    cout << " found image filename: " << imgFile << endl;
    return myImage->loadImage(imgFile);

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	// in fullscreen mode, on a pc at least, the
	// first time video settings the come up
	// they come up *under* the fullscreen window
	// use alt-tab to navigate to the settings
	// window. we are working on a fix for this...

    input->normalKeyDown(key,mouseX,mouseY);
    if (input->bTextInput)
        return;

}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);

    if (input->bTextInput)
        return;
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

      for( ; element!=NULL; element=element->NextSiblingElement("Actor")){
        myType=element->GetText();
        cout << "Loading property type: " << myType << endl;
        load(element);
        listPos++;
        }
}


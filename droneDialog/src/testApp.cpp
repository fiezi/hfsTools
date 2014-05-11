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

    myData->readTextFromFile("dialog.xml");

}


//--------------------------------------------------------------
void testApp::setup(){

	drohneOneText="";
	drohneTwoText="";
	xOffset=400;
	yOffset=50;

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

    linebreakWidth=480;

    drohneOneImage.loadImage("stage.tga");
    msbSetup();

    loadSettings();

    interfaceSetup();

    //always set beat to zero at Beginning!
    beat=0;

    drohneOneFont.loadFont("DroidSans.ttf",fontSizeOne,true,true);
    drohneTwoFont.loadFont("DroidSans.ttf",fontSizeTwo,true,true);

    myData= new DataHandlerDialog;
    myData->setup("dialoge\\dialog.xml");

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

    createMemberID("LINEBREAKWIDTH",&linebreakWidth,this);
    createMemberID("TEXTSPEEDONE",&textSpeedOne,this);
    createMemberID("TEXTSPEEDTWO",&textSpeedTwo,this);
    createMemberID("FONTSIZEONE",&fontSizeOne,this);
    createMemberID("FONTSIZETWO",&fontSizeTwo,this);
    createMemberID("BEAT",&beat,this);
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


    //BEAT INFO


    but= new TextInputButton();
    but->name="Next Beat";
    but->buttonProperty="BEAT";
    but->bDrawName=true;
    but->tooltip="naechster Beat";
    but->setLocation(Vector3f(20,610,0));
    but->scale.x=128;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="line break width";
    but->buttonProperty="LINEBREAKWIDTH";
    but->bDrawName=true;
    but->tooltip="wann umschlagen?";
    but->setLocation(Vector3f(20,640,0));
    but->scale.x=128;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);


    sli= new SliderButton();
    sli->name="Scroll to beat";
    sli->buttonProperty="NULL";
    sli->bDrawName=false;
    sli->tooltip="Scroll Beat";
    sli->setLocation(Vector3f(300,200,0));
    sli->bTriggerWhileDragging=true;
    //sli->
    sli->scale.x=20;
    sli->scale.y=400;
    sli->textureID="icon_flat";
    sli->parent=this;
    sli->color=Vector4f(0.4,0.4,0.4,1.0);
    sli->setup();
    renderer->buttonList.push_back(sli);

    textFieldSetup();
}

void testApp::textFieldSetup(){

    //Adding MSB content
    TextInputButton *but;

    but= new TextInputButton();
    but->name="Drohne1";
    but->buttonProperty="NULL";
    but->bDrawName=true;
    but->tooltip="Drone One Text\ngoes here...";
    but->setLocation(Vector3f(20,100,0));
    but->scale.x=128;
    but->scale.y=32;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.75,0.5,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);
    drohneOne=but;

    but= new TextInputButton();
    but->name="Drohne2";
    but->buttonProperty="NULL";
    but->bDrawName=true;
    but->tooltip="whereas Drone Two\nText is here.";
    but->setLocation(Vector3f(20,400,0));
    but->scale.x=128;
    but->scale.y=32;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.50,0.50,0.750,1.0);
    but->bShowCursor=true;
    but->setup();
    renderer->buttonList.push_back(but);
    drohneTwo=but;

    AssignButton* bb= new AssignButton;
    bb->name="insert1";
    bb->buttonProperty="NULL";
    bb->bDrawName=true;
    bb->tooltip="";
    bb->setLocation(Vector3f(20,140,0));
    bb->scale.x=60;
    bb->scale.y=60;
    bb->textureID="icon_flat";
    bb->parent=this;
    bb->color=Vector4f(0.5,0.40,0.40,1.0);
    bb->setup();
    renderer->buttonList.push_back(bb);

    bb= new AssignButton;
    bb->name="replace1";
    bb->buttonProperty="NULL";
    bb->bDrawName=true;
    bb->tooltip="";
    bb->setLocation(Vector3f(88,140,0));
    bb->scale.x=60;
    bb->scale.y=60;
    bb->textureID="icon_flat";
    bb->parent=this;
    bb->color=Vector4f(0.5,0.40,0.40,1.0);
    bb->setup();
    renderer->buttonList.push_back(bb);

    bb= new AssignButton;
    bb->name="insert2";
    bb->buttonProperty="NULL";
    bb->bDrawName=true;
    bb->tooltip="";
    bb->setLocation(Vector3f(20,440,0));
    bb->scale.x=60;
    bb->scale.y=60;
    bb->textureID="icon_flat";
    bb->parent=this;
    bb->color=Vector4f(0.5,0.40,0.40,1.0);
    bb->setup();
    renderer->buttonList.push_back(bb);

    bb= new AssignButton;
    bb->name="replace2";
    bb->buttonProperty="NULL";
    bb->bDrawName=true;
    bb->tooltip="";
    bb->setLocation(Vector3f(88,440,0));
    bb->scale.x=60;
    bb->scale.y=60;
    bb->textureID="icon_flat";
    bb->parent=this;
    bb->color=Vector4f(0.5,0.40,0.40,1.0);
    bb->setup();
    renderer->buttonList.push_back(bb);
}


void testApp::setTextFromBeat(int b, bool bPlayVoice){

    if (b>=myData->dialog.size() || b<0 ){
        cout << "reached end of dialogue: " << myData->dialog.size() << endl;
        drohneOne->tooltip="";
        drohneTwo->tooltip="";
        return;
    }

    if (myData->dialog[b].whoIs==1){
        drohneOne->tooltip=linebreak(myData->dialog[b].dialogText, &drohneOneFont);
        bShowTextOne=true;
        bShowTextTwo=false;
        bShowImageOne=parseForImage(&drohneOneImage,drohneOne->tooltip);
        bShowImageTwo=false;
        textOneChar=0;
        if (bPlayVoice && myData->dialog[b].dialogVoice.isLoaded())
            myData->dialog[b].dialogVoice.play();
    }
    if (myData->dialog[b].whoIs==2){
        drohneTwo->tooltip=linebreak(myData->dialog[b].dialogText, &drohneTwoFont);
        bShowTextTwo=true;
        bShowTextOne=false;
        bShowImageTwo=parseForImage(&drohneTwoImage,drohneTwo->tooltip);
        bShowImageOne=false;
        textTwoChar=0;
        if (bPlayVoice && myData->dialog[b].dialogVoice.isLoaded())
            myData->dialog[b].dialogVoice.play();
    }

}
//--------------------------------------------------------------
void testApp::update(){

    //update slider
    sli->sliderValue=max(0.0f, min((float)beat/(float)myData->dialog.size(),1.0f));

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

	ofBackground(100,100,105);

    renderer->draw();

    //why we have to do this, i have no idea!
    //if we don't do it, font drawing doesn't work!
    drohneOneImage.draw(0,0,-100,-100);

    ofSetColor(255,255,255);

    drawPreview();
    drawFinal();
}

void testApp::drawFinal(){

    ofPushMatrix();
        ofTranslate(1440,0);

        //window dressing
        //ofScale(0.3,0.3);
        ofSetColor(40,40,40);
        ofRect(0,0,1280,800);

        ofSetColor(255,255,255);
        ofRect(635,0,10,800);

        //proper text animation
        // only show pictures after text os finished displaying...
        if (bShowTextOne)
            drohneOneFont.drawString(drohneOneText,5,drohneOneFont.stringHeight("A")+10);

        if (bShowImageOne && textOneChar==drohneOne->tooltip.size())
            drohneOneImage.draw(5,drohneOneFont.stringHeight(drohneOneText)+10);



        // only show pictures after text os finished displaying...
        if (bShowTextTwo)
            drohneTwoFont.drawString(drohneTwoText,650,drohneTwoFont.stringHeight("A")+10);

        if (bShowImageTwo&& textTwoChar==drohneTwo->tooltip.size())
            drohneTwoImage.draw(655,drohneTwoFont.stringHeight(drohneTwoText)+10);

    ofPopMatrix();



}

void testApp::drawPreview(){

    ofPushMatrix();
        ofTranslate(xOffset,yOffset);

        //window dressing
        ofScale(0.3,0.3);
        ofSetColor(40,40,40);
        ofRect(0,0,1280,800);

        ofSetColor(255,255,255);
        ofRect(635,0,10,800);

        //PREVIEW
        ofSetColor(240,240,180);

        int i=0;
        float yPO=SCREENRESY+drohneOneFont.stringHeight("A") + 100;
        while(beat+i<myData->dialog.size()){

            stringstream ss;
            ss << beat+i;

            if (myData->dialog[beat+i].whoIs==1){
                string previewText=linebreak(myData->dialog[beat+i].dialogText,&drohneOneFont);
                ofSetColor(255,100,100);
                drohneOneFont.drawString(ss.str()+".)",-drohneOneFont.stringWidth(ss.str()+".)"),yPO);
                ofSetColor(200,200,180);
                drohneOneFont.drawString(previewText,5,yPO);
                yPO += drohneOneFont.stringHeight(previewText)+drohneOneFont.stringHeight("A");
            }else{
                string previewText=linebreak(myData->dialog[beat+i].dialogText,&drohneTwoFont);
                ofSetColor(255,100,100);
                drohneTwoFont.drawString(ss.str()+".)",645-drohneTwoFont.stringWidth(ss.str()+".)"),yPO);
                ofSetColor(200,200,180);
                drohneTwoFont.drawString(previewText,650,yPO);
                yPO += drohneTwoFont.stringHeight(previewText)+drohneTwoFont.stringHeight("A");
            }

            i++;
        }

        //END PREVIEW

        ofSetColor(255,255,100);
        stringstream ss;
        ss << beat-1;
        drohneOneFont.drawString(ss.str()+".)",-10-drohneOneFont.stringWidth(ss.str()+".)"),drohneOneFont.stringHeight("A")+10);

        ofSetColor(255,255,255);
        //proper text animation
        // only show pictures after text os finished displaying...
        if (bShowTextOne)
            drohneOneFont.drawString(drohneOneText,5,drohneOneFont.stringHeight("A")+10);

        if (bShowImageOne && textOneChar==drohneOne->tooltip.size())
            drohneOneImage.draw(5,drohneOneFont.stringHeight(drohneOneText)+10);



        // only show pictures after text os finished displaying...
        if (bShowTextTwo)
            drohneTwoFont.drawString(drohneTwoText,650,drohneTwoFont.stringHeight("A")+10);

        if (bShowImageTwo&& textTwoChar==drohneTwo->tooltip.size())
            drohneTwoImage.draw(655,drohneTwoFont.stringHeight(drohneTwoText)+10);

    ofPopMatrix();

}


string testApp::linebreak(string text, ofTrueTypeFont* myFont){

    //always go from the back of the text - THIS ONLY WORKS WHEN WE TYPE! NOT WITH PREDEFINED TEXT!
    if (input->bTextInput){
        unsigned foundBreak = text.find_last_of("\n");
        string lastLine= text.substr(foundBreak+1);
        //are we over capacity? then let's insert a linebreak at the last space we find
        if (myFont->stringWidth(lastLine)>590){
            //so let's do it!
            //Find a whitespace
             unsigned foundSpace= lastLine.find_last_of(' ');
             text.replace(foundBreak+foundSpace+1,1,"\n");
        }
        return text;
    }

    //whereas this only works if we don't type!
    text=text+' ';  //make sure we have a white space at the end to tokenize last word!
    int lineWidth=0;
    string finalText;


    std::istringstream iss(text);
    std::string token;

    while (getline(iss, token, ' '))   //go word for word
    {
        //get width of word
        lineWidth+=myFont->stringWidth(token+' ');  //add word width to lineWidth
        if (lineWidth>linebreakWidth){ //must make new line
            finalText+='\n';//break old line
            finalText+=token+' ';//add first word in next line
            lineWidth=myFont->stringWidth(token+' ');//next line width starts with first word
        }else{
            finalText+=token+' ';//add word to old line
        }
    }
    return finalText;


}


void testApp::trigger(Actor* other){

    input->bTextInput=false;

    if (other->name=="Scroll to beat"){
        sli=(SliderButton*)other;
        beat=sli->sliderValue * (myData->dialog.size());
        setTextFromBeat(beat-1, false);
    }
    if (other->name=="Next Beat"){
        sli->sliderValue= beat/myData->dialog.size();
    }


    if (other->name=="insert1"){
        dialogPart newDialogPart;
        newDialogPart.dialogText=drohneOne->tooltip;
        newDialogPart.whoIs=1;
        newDialogPart.dialogVoiceFile="NULL";
        myData->dialog.insert(myData->dialog.begin()+beat,newDialogPart);
        //void all further sound files?
        for (int i=beat;i<myData->dialog.size();i++){
            myData->dialog[i].dialogVoiceFile="NULL";
        }
    }

    if (other->name=="insert2"){
        dialogPart newDialogPart;
        newDialogPart.dialogText=drohneTwo->tooltip;
        newDialogPart.whoIs=2;
        newDialogPart.dialogVoiceFile="NULL";
        myData->dialog.insert(myData->dialog.begin()+beat,newDialogPart);
        //void all further sound files?
        for (int i=beat;i<myData->dialog.size();i++){
            myData->dialog[i].dialogVoiceFile="NULL";
        }
    }



    if (other->name=="replace1"){
        if (beat<0){
            cout << "cannot replace that which is not yet present!" << endl;
            return;
        }
        myData->dialog[beat-1].dialogText=drohneOne->tooltip;
        myData->dialog[beat-1].whoIs=1;
        myData->dialog[beat-1].dialogVoiceFile="NULL";
        if (myData->dialog[beat-1].dialogVoice.isLoaded())
            myData->dialog[beat-1].dialogVoice.unloadSound();
    }

    if (other->name=="replace2"){
        if (beat<0){
            cout << "cannot replace that which is not yet present!" << endl;
            return;
        }
        myData->dialog[beat-1].dialogText=drohneTwo->tooltip;
        myData->dialog[beat-1].whoIs=2;
        myData->dialog[beat-1].dialogVoiceFile="NULL";
        if (myData->dialog[beat-1].dialogVoice.isLoaded())
            myData->dialog[beat-1].dialogVoice.unloadSound();

    }


    if (other->name=="Drohne1"){
        //delete a post?
        if (drohneOne->tooltip=="NULL"){
            myData->dialog.erase(myData->dialog.begin()+beat);
            for (int i=beat;i<myData->dialog.size();i++){
                myData->dialog[i].dialogVoiceFile="NULL";
            }
        }

        bShowTextOne=true;
        textOneChar=0;
        bShowImageOne=parseForImage(&drohneOneImage,drohneOne->tooltip);
    }

    if (other->name=="Drohne2"){
        //delete a post?
        if (drohneTwo->tooltip=="NULL"){
            myData->dialog.erase(myData->dialog.begin()+beat);
            for (int i=beat;i<myData->dialog.size();i++){
                myData->dialog[i].dialogVoiceFile="NULL";
            }
        }
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

    unsigned fileStart=text.find('[');
    if (fileStart==string::npos){
        return false;
    }
    unsigned fileEnd=text.find(']');
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

    if (key==OF_KEY_UP)
        setTextFromBeat(--beat-1,false);

    if (key==OF_KEY_DOWN)
        setTextFromBeat(beat++,false);

    if (key==' ')
        setTextFromBeat(beat++);

    if (key=='S'){
        cout << "saving..."<<endl;
        myData->writeTextToFile("dialog.xml");
    }

    if (key=='L'){
        cout << "loading..."<<endl;
        myData->readTextFromFile("dialog.xml");
    }

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


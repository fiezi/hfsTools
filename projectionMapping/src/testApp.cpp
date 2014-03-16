#include "testApp.h"

//--------------------------------------------------------------

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


void testApp::setup(){

    xP[0]=0;
    xP[1]=1280;
    xP[2]=1280;
    xP[3]=0;
    yP[0]=0;
    yP[1]=0;
    yP[2]=800;
    yP[3]=800;

    xP2[0]=0;
    xP2[1]=1280;
    xP2[2]=1280;
    xP2[3]=0;
    yP2[0]=0;
    yP2[1]=0;
    yP2[2]=800;
    yP2[3]=800;

    xC[0]=0;
    xC[1]=640;
    xC[2]=640;
    xC[3]=0;
    yC[0]=0;
    yC[1]=0;
    yC[2]=480;
    yC[3]=480;

    mX=0;
    mY=0;

    mainW = 1920;
    mainH = 1080;
    firstW = 1920;
    firstH = 1080;
    secondW = 1280;
    secondH = 800;

    threshold=40;

    lineWidth=4.0;

    erodeAmount=0;
    dilateAmount=0;
    blurAmount=0;

    bInvertMask=false;

    ocvImage.allocate(640,480);
    ocvMask.allocate(640,480);
    ocvDiff.allocate(640,480);

    kinect.bImage = true;
    kinect.init();
    kinect.setVerbose(true);
    kinect.open();

	ofBackground(0,0,64);


    pixelBufferOne= new unsigned char[640*480];
    pixelBufferTwo= new unsigned char[640*480];
    pixelBufferThree= new unsigned char[640*480];


    //ofSetFrameRate(25);


    ofDisableArbTex();

    rgbaFbo.allocate(1280, 800, GL_RGBA); // with alpha, 8 bits red, 8 bits green, 8 bits blue, 8 bits alpha, from 0 to 255 in 256 steps

    rgbaFbo.begin();

	ofClear(255,255,255, 0);

    rgbaFbo.end();


    //stage simulation
    stageImage.loadImage("stage.tga");
    //stageImage.getTextureReference().texData.textureTarget=GL_TEXTURE_2D;

    msbSetup();

    loadSettings();

    interfaceSetup();


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

    createMemberID("XP",&xP,this);
    createMemberID("YP",&yP,this);

    createMemberID("XC",&xC,this);
    createMemberID("YC",&yC,this);

    createMemberID("THRESHOLD",&threshold,this);
    createMemberID("LINEWIDTH",&lineWidth,this);

    createMemberID("DILATEAMOUNT",&dilateAmount,this);
    createMemberID("ERODEAMOUNT",&erodeAmount,this);
    createMemberID("BLURAMOUNT",&blurAmount,this);


}

//--------------------------------------------------------------
void testApp::update(){

    kinect.update();
    renderer->update();

    rgbaFbo.begin();
        ofClear(0,0,0,0);
		drawGrid();
    rgbaFbo.end();

    ocvImage.setFromPixels(kinect.depthPixels, 640,480);

    //das eine minus das andere - wie geht?
    //ocvImage.absDiff(ocvDiff); // so gehts! - nee, so gehts nich
    //sondern:
    pixelBufferOne=ocvImage.getPixels();
    pixelBufferTwo=ocvDiff.getPixels();

    for (int i=0;i<640*480;i++){
        if (pixelBufferOne[i]-pixelBufferTwo[i]<0)
            pixelBufferThree[i]=pixelBufferOne[i];
        else{
            if (bInvertMask)
                pixelBufferThree[i]=0;
            else
                pixelBufferThree[i]=255;
        }
    }

    ocvImage.setFromPixels(pixelBufferThree,640,480);

    //ocvImage.threshold(threshold,true);
    contourFinder.findContours(ocvImage,1000,200000,3,false,true);
    for(int i = 0; i < contourFinder.nBlobs; i++) {
        mX=contourFinder.blobs[i].centroid.x *(1280.0f/640.0f);
        mY=contourFinder.blobs[i].centroid.y *(800.0/480.0f);
        rgbaFbo.begin();
            drawFill( (mX/128)*128,(mY/128)*128);
        rgbaFbo.end();
    }

}

//--------------------------------------------------------------
void testApp::drawGrid(){

    ofSetHexColor(0xffffff);
    ofNoFill();
    ofSetCircleResolution(128);
    ofSetLineWidth(lineWidth);
    //ofCircle(mX,mY,200);

    for(int grY=0; grY<=800; grY+=128){
            for(int grX=0; grX<=1280; grX+=128){
                ofRect(grX,grY,128,128);
            }
        }


}

void testApp::drawFill(int x, int y){

    ofSetHexColor(0xffffff);
    ofFill();
    ofSetLineWidth(lineWidth);
    ofRect(x,y,128,128);

}

void testApp::draw(){

    ofClear(0,0,0,255);
    ofNoFill();

    //glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, slBut->sliderValue * 10.0);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    stageImage.draw(mainW,0);
    //kinect.draw(1920,0);

    ofEnableAlphaBlending();


    ofPushMatrix();

        ofSetHexColor(0xffff00);
        ofRect(100,800,320,200);

        ofSetHexColor(0xffffff);
        ofTranslate(100,800,0);
        ofScale(0.25,0.25,0.25);
        rgbaFbo.draw(0,0);

    ofPopMatrix();

    /*
    mX=  (mouseX - 100)*4;
    mY=  (mouseY - 400)*4;
    */

    ofPushMatrix();
    ofTranslate(600,100);
    ofScale(0.5,0.5,1.0);
    ocvImage.draw(0,0);
    contourFinder.draw();

    ofPopMatrix();

            //was passiert hier eigentlich?

    //Draw Camera and Camtransform

    ocvDiff.draw(1000,500);

    kinect.drawDepth(1000,0);


            //do das crazy Mathematics

            ofPushMatrix();

            ofPoint dstC[]={ofPoint(0,0),ofPoint(640,0),ofPoint(640,480),ofPoint(0,480)};
            ofPoint srcC[]={ofPoint(xC[0],yC[0]),ofPoint(xC[1],yC[1]),ofPoint(xC[2],yC[2]),ofPoint(xC[3],yC[3])};


            GLfloat matrixC[16];
            findHomography(srcC,dstC,matrixC);
            glMultMatrixf(matrixC);

            //ofSetColor(1.0,0.0,0.0,0.5);
            //kinect.drawDepth(0,0);

            ofPopMatrix();


    //DRAW ALL TEH GRIDZ!!!11!


    //Draw first Projection Grid
    ofPushMatrix();

        //Draw tiny red rectangle in which we draw first Projection Grid
        ofSetHexColor(0xff0000);
        ofRect(100,100,320,200);

        ofTranslate(100,100,0);
        ofScale(0.25,0.25,0.25);


        ofPushMatrix();

            //do das crazy Mathematics
            ofPoint src[]={ofPoint(0,0),ofPoint(1280,0),ofPoint(1280,800),ofPoint(0,800)};
            ofPoint dst[]={ofPoint(xP[0],yP[0]),ofPoint(xP[1],yP[1]),ofPoint(xP[2],yP[2]),ofPoint(xP[3],yP[3])};

            GLfloat matrix[16];
            findHomography(src,dst,matrix);
            glMultMatrixf(matrix);
            ofSetHexColor(0xffffff);

            rgbaFbo.draw(0,0);

        ofPopMatrix();
    ofPopMatrix();

    //Draw second Projection Grid
    ofPushMatrix();

        //Draw tiny red rectangle in which we draw second Projection Grid
        ofSetHexColor(0xff0000);
        ofRect(100,500,320,200);

        ofTranslate(100,500,0);
        ofScale(0.25,0.25,0.25);

        //do das crazy Mathematics
        ofPushMatrix();

            ofPoint srcP2[]={ofPoint(0,0),ofPoint(1280,0),ofPoint(1280,800),ofPoint(0,800)};
            ofPoint dstP2[]={ofPoint(xP2[0],yP2[0]),ofPoint(xP2[1],yP2[1]),ofPoint(xP2[2],yP2[2]),ofPoint(xP2[3],yP2[3])};


            GLfloat matrixP2[16];
            findHomography(srcP2,dstP2,matrixP2);
            glMultMatrixf(matrixP2);
            ofSetHexColor(0xffffff);

            rgbaFbo.draw(0,0);

        ofPopMatrix();

    ofPopMatrix();

    ofPushMatrix();

        //draw final translated first Grid
        ofTranslate(mainW,0);//main monitor resolution
        ofPushMatrix();
            glMultMatrixf(matrix);
            ofSetColor(255,255,255);
            rgbaFbo.draw(0,0);
        ofPopMatrix();

        //draw final translated second Grid
        ofTranslate(firstW,0); //first additional screen resolution
        ofPushMatrix();
            glMultMatrixf(matrixP2);
            ofSetColor(255,255,255);
            rgbaFbo.draw(0,0);
        ofPopMatrix();


    ofPopMatrix();

    renderer->backgroundColor.a=0;

    renderer->draw();

}

void testApp::trigger(Actor* other){

    if (other->name=="thresholdSlider"){

        //cout << ((SliderButton*)other)->sliderValue << endl;
        threshold=((SliderButton*)other)->sliderValue * 255.0;

    }

    if (other->name=="stefano"){

        cout << "hippie!!!" << endl;

    }

    if (other->name=="threshold"){
        slBut->sliderValue=threshold/255.0;
    }

    if (other->name=="erode" ||other->name=="dilate" ||other->name=="blur"){

        ocvDiff.setFromPixels(ocvMask.getPixels(),640,480);

        for (int i=0;i<dilateAmount; i++)
            ocvDiff.dilate();

        for (int i=0;i<erodeAmount; i++)
            ocvDiff.erode();

        ocvDiff.blurGaussian(blurAmount);
    }

    //erstes Projektionsgrid

    if (other->name=="ecke1"){
        xP[0]=other->location.x-mainW+16;
        yP[0]=other->location.y+16;
    }

    if (other->name=="ecke2"){
        xP[1]=other->location.x-mainW+16;
        yP[1]=other->location.y+16;
    }

    if (other->name=="ecke3"){
        xP[2]=other->location.x-mainW+16;
        yP[2]=other->location.y+16;
    }

    if (other->name=="ecke4"){
        xP[3]=other->location.x-mainW+16;
        yP[3]=other->location.y+16;
    }


    //zweites Projektionsgrid

    if (other->name=="eckeB1"){
        xP2[0]=other->location.x-(mainW+firstW)+16;
        yP2[0]=other->location.y+16;
        cout << "yeah!" << endl;
    }

    if (other->name=="eckeB2"){
        xP2[1]=other->location.x-(mainW+firstW)+16;
        yP2[1]=other->location.y+16;
    }

    if (other->name=="eckeB3"){
        xP2[2]=other->location.x-(mainW+firstW)+16;
        yP2[2]=other->location.y+16;
    }

    if (other->name=="eckeB4"){
        xP2[3]=other->location.x-(mainW+firstW)+16;
        yP2[3]=other->location.y+16;
    }







    //Kameragrid

    if (other->name=="ecke1c"){
        xC[0]=other->location.x-1000+16;
        yC[0]=other->location.y+16;
    }

    if (other->name=="ecke2c"){
        xC[1]=other->location.x-1000+16;
        yC[1]=other->location.y+16;
    }

    if (other->name=="ecke3c"){
        xC[2]=other->location.x-1000+16;
        yC[2]=other->location.y+16;
    }

    if (other->name=="ecke4c"){
        xC[3]=other->location.x-1000+16;
        yC[3]=other->location.y+16;
    }



}

//--------------------------------------------------------------
void testApp::keyPressed(int key){


    input->normalKeyDown(key,mouseX,mouseY);
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);

    if (input->bTextInput)
        return;

    if (key=='1'){
        xP[0]=mouseX-mainW;
        yP[0]=mouseY;
    }

    if (key=='2'){
        xP[1]=mouseX-mainW;
        yP[1]=mouseY;
    }

    if (key=='3'){
        xP[2]=mouseX-mainW;
        yP[2]=mouseY;
    }

    if (key=='4'){
        xP[3]=mouseX-mainW;
        yP[3]=mouseY;
    }

    if (key=='m'){
        ocvMask.setFromPixels(kinect.depthPixels, 640,480);
        ocvDiff.setFromPixels(ocvMask.getPixels(),640,480);
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


void testApp::findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]){
	// arturo castro - 08/01/2010
	//
	// create the equation system to be solved
	//
	// from: Multiple View Geometry in Computer Vision 2ed
	//       Hartley R. and Zisserman A.
	//
	// x' = xH
	// where H is the homography: a 3 by 3 matrix
	// that transformed to inhomogeneous coordinates for each point
	// gives the following equations for each point:
	//
	// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
	// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
	//
	// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
	// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
	// after ordering the terms it gives the following matrix
	// that can be solved with gaussian elimination:
	float P[8][9]={
		{-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
		{  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12
		{-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
		{  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21
		{-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
		{  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23
		{-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
		{  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
	};
	gaussian_elimination(&P[0][0],9);
	// gaussian elimination gives the results of the equation system
	// in the last column of the original matrix.
	// opengl needs the transposed 4x4 matrix:
	float aux_H[]={ P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
		P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
		0      ,      0,0,0,       // 0    0   0 0
		P[2][8],P[5][8],0,1};      // h13  h23 0 h33
	for(int i=0;i<16;i++) homography[i] = aux_H[i];
}

void testApp::gaussian_elimination(float *input, int n){
	// arturo castro - 08/01/2010
	//
	// ported to c from pseudocode in
	// [http://en.wikipedia.org/wiki/Gaussian-elimination](http://en.wikipedia.org/wiki/Gaussian-elimination)
	float * A = input;
	int i = 0;
	int j = 0;
	int m = n-1;
	while (i < m && j < n){
		// Find pivot in column j, starting in row i:
		int maxi = i;
		for(int k = i+1; k<m; k++){
			if(fabs(A[k*n+j]) > fabs(A[maxi*n+j])){
				maxi = k;
			}
		}
		if (A[maxi*n+j] != 0){
			//swap rows i and maxi, but do not change the value of i
			if(i!=maxi)
				for(int k=0;k<n;k++){
					float aux = A[i*n+k];
					A[i*n+k]=A[maxi*n+k];
					A[maxi*n+k]=aux;
				}
			//Now A[i,j] will contain the old value of A[maxi,j].
			//divide each entry in row i by A[i,j]
			float A_ij=A[i*n+j];
			for(int k=0;k<n;k++){
				A[i*n+k]/=A_ij;
			}
			//Now A[i,j] will have the value 1.
			for(int u = i+1; u< m; u++){
				//subtract A[u,j] * row i from row u
				float A_uj = A[u*n+j];
				for(int k=0;k<n;k++){
					A[u*n+k]-=A_uj*A[i*n+k];
				}
				//Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
			}
			i++;
		}
		j++;
	}
	//back substitution
	for(int i=m-2;i>=0;i--){
		for(int j=i+1;j<n-1;j++){
			A[i*n+m]-=A[i*n+j]*A[j*n+m];
			//A[i*n+j]=0;
		}
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



void testApp::interfaceSetup(){
    //Adding MSB content
    BasicButton *but;

    cornerSetup();

    //anderes


    but= new TextInputButton();
    but->name="threshold";
    but->buttonProperty="THRESHOLD";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(500,700,0));
    but->scale.x=130;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="lineWidth";
    but->buttonProperty="LINEWIDTH";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(700,700,0));
    but->scale.x=130;
    but->scale.y=20;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="transformX";
    but->buttonProperty="XP";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(500,730,0));
    but->scale.x=130;
    but->scale.y=12;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="transformY";
    but->buttonProperty="YP";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(500,750,0));
    but->scale.x=130;
    but->scale.y=12;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="dilate";
    but->buttonProperty="DILATEAMOUNT";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(500,790,0));
    but->scale.x=130;
    but->scale.y=12;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="erode";
    but->buttonProperty="ERODEAMOUNT";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(500,810,0));
    but->scale.x=130;
    but->scale.y=12;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);

    but= new TextInputButton();
    but->name="blur";
    but->buttonProperty="BLURAMOUNT";
    but->bDrawName=true;
    but->tooltip="";
    but->setLocation(Vector3f(500,830,0));
    but->scale.x=130;
    but->scale.y=12;
    but->textureID="icon_flat";
    but->parent=this;
    but->color=Vector4f(0.5,0.5,0.5,1.0);
    but->setup();
    renderer->buttonList.push_back(but);


    slBut= new SliderButton();
    slBut->name="thresholdSlider";
    slBut->bDrawName=false;
    slBut->tooltip="set kinect distance";
    slBut->setLocation(Vector3f(600,620,0));
    slBut->scale.x=320;
    slBut->scale.y=16;
    slBut->bVertical=false;
    slBut->textureID="icon_flat";
    slBut->color=Vector4f(0.5,0.5,0.5,1.0);
    slBut->parent=this;
    slBut->setup();
    slBut->sliderValue=threshold/255.0;
    renderer->buttonList.push_back(slBut);
}


void testApp::cornerSetup(){

    BasicButton *but;

    //eckKnoepfe
    Vector3f eckScale=Vector3f(32,32,1);
    Vector4f eckColor=Vector4f(1,1,1,1.0);

        //corners
    but= new AssignButton;
    but->name="ecke1";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP[0]+mainW-16,yP[0]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="ecke2";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP[1]+mainW-16,yP[1]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="ecke3";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP[2]+mainW-16,yP[2]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="ecke4";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP[3]+mainW-16,yP[3]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    //projector2
    but= new AssignButton;
    but->name="eckeB1";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP2[0]+mainW+firstW-16,yP2[0]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="eckeB2";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP2[1]+mainW+firstW-16,yP2[1]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="eckeB3";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP2[2]+mainW+firstW-16,yP2[2]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="eckeB4";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xP2[3]+mainW+firstW-16,yP2[3]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);


// camera Corners
    but= new AssignButton;
    but->name="ecke1c";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xC[0]+1000-16,yC[0]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="ecke2c";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xC[1]+1000-16,yC[1]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="ecke3c";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xC[2]+1000-16,yC[2]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

    but= new AssignButton;
    but->name="ecke4c";
    but->bDrawName=true;
    but->bDragable=true;
    but->tooltip="drag Me!";
    but->setLocation(Vector3f(xC[3]+1000-16,yC[3]-16,0));
    but->scale=eckScale;
    but->color=eckColor;
    but->bTriggerWhileDragging=true;
    but->bResetAfterDrag=false;
    but->textureID="ecke";
    but->parent=this;
    but->setup();
    renderer->buttonList.push_back(but);

}

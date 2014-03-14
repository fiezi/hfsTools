#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofMain.h"
#include "msbOFCore.h"
#include "actor.h"

#include "basicButton.h"
#include "textInputButton.h"
#include "assignButton.h"
#include "sliderButton.h"



// Uncomment this to use a camera instead of a video file
#define CAMERA_CONNECTED
#define SCREENRESX 1920+1280
#define SCREENRESY 1080

struct actorID;
struct memberID;

class testApp : public ofBaseApp, public Actor{

	public:
	    testApp();
	    virtual ~testApp();

		void setup();
		void update();
		void draw();

		void interfaceSetup();
		//from msbOFCore
		void msbSetup();
		void registerProperties();
		void trigger(Actor* other);
        void loadSettings();


        void drawGrid();
        void drawFill(int x, int y);

		void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]);
		void gaussian_elimination(float *input, int n);

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        ofFbo rgbaFbo; // with alpha

        ofImage beginImage;
        ofImage baseImage;
        ofImage buildingImage;
        ofImage roadImage;

        ofImage stageImage;

        Vector4f xP;
        Vector4f yP;

        Vector4f xC;
        Vector4f yC;

        int mX;
        int mY;

        //Kinect stuff from here

        ofxKinect kinect;

        ofxCvGrayscaleImage ocvImage;
        ofxCvContourFinder contourFinder;

        int threshold;
        SliderButton* slBut;
};

#endif

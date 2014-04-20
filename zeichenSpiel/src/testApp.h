#ifndef _TEST_APP
#define _TEST_APP




#include "ofMain.h"

#include "ofxKinect.h"
#include "ofxOsc.h"
#include "ofxOpenCV.h"
#include "msbOFCore.h"
#include "actor.h"

#define SCREENRESX 1920
#define SCREENRESY 1080


struct actorID;
struct memberID;

class testApp : public ofBaseApp, public Actor{

	public:

        testApp();
        virtual ~testApp();

		void setup();

        void msbSetup();
		void interfaceSetup();
		void filemappingSetup();

		void update();

		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        //msbSpecific
        void registerProperties();
        void trigger(Actor* other);
        void loadSettings();

        ofxKinect           kinect;

        bool                bSetCutoffToZero;
        bool                bFullscreen;

        //msbTools specific

        Input*              input;
        Renderer*           renderer;

        float               cutOffDepth;

        float               thresh;

        int                 dilate;
        int                 erode;
        int                 blur;

        int                 dilateMask;
        int                 erodeMask;
        int                 blurMask;

        ofxCvGrayscaleImage          cvImage;
        ofxCvGrayscaleImage          cvFinal;
        ofxCvGrayscaleImage          cvMaskBase;
        ofxCvGrayscaleImage          cvMask;

        unsigned char*               pixelBufferOne;
        unsigned char*               pixelBufferTwo;
        unsigned char*               pixelBufferThree;


        BasicButton*        kinectDisplay;
        bool                bKinectIsSending;

};

#endif
#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenCv.h"
#include "ofMain.h"
#include "msbOFCore.h"
#include "actor.h"

#include "basicButton.h"
#include "textInputButton.h"
#include "assignButton.h"
#include "sliderButton.h"



#define SCREENRESX 1366+1280
#define SCREENRESY 768


struct actorID;
struct memberID;


class testApp : public ofBaseApp, public Actor{

	public:

	    testApp();
	    virtual ~testApp();

		void setup();
		void update();
		void draw();

        bool parseForImage(ofImage* myImage, string text);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);


        //from msbOFCore
		void msbSetup();
		void interfaceSetup();
		void registerProperties();
		void trigger(Actor* other);
        void loadSettings();

        void textFieldSetup();

        string linebreak(string text,ofTrueTypeFont* myFont);

        ofImage             drohneOneImage;
        ofImage             drohneTwoImage;
		BasicButton*        drohneOne;
		BasicButton*        drohneTwo;

		ofTrueTypeFont      drohneOneFont;
		ofTrueTypeFont      drohneTwoFont;

		float               fontSizeOne;
		float               fontSizeTwo;

		float               textSpeedOne,textSpeedTwo;

		string              drohneOneText;
		string              drohneTwoText;

		float               xOffset,yOffset;

		bool                bShowTextOne,bShowTextTwo;
		bool                bShowImageOne,bShowImageTwo;
		bool                bShowMovieOne,bShowMovieTwo;

		//for animation
		float               textOneChar,textTwoChar;
};

#endif

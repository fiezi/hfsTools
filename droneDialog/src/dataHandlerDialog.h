#ifndef _DATAHANDLERDAILOG
#define _DATAHANDLERDIALOG

#include "actor.h"
#include "ofMain.h"
#include <Tlhelp32.h>
#include <winbase.h>


#define SYSTEMVOICE 0
#define DRONEONE 1
#define DRONETWO 2


struct dialogPart{

int  whoIs;
string  dialogText;
string  dialogVoiceFile;
ofSoundPlayer dialogVoice;
};


class DataHandlerDialog : public Actor{

	public:

	    DataHandlerDialog();
	    virtual ~DataHandlerDialog();

        void setup(string dialogFile);

        void makeVoiceFile(dialogPart* dp, int beatNumber);
        void toWav(const std::string &s);


        void readTextFromFile(string filename);
        void writeTextToFile(string filename);

        string unlinebreak(string text);

        string  dialogFileName;

	    string  droneOneVoice;
	    string  droneTwoVoice;
	    string  systemVoice;
	    string  voices[3];

	    vector<dialogPart>  dialog;


};

#endif

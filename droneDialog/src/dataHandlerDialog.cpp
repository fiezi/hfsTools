#include "dataHandlerDialog.h"

DataHandlerDialog::DataHandlerDialog(){

}

DataHandlerDialog::~DataHandlerDialog(){

}

void DataHandlerDialog::setup(string dialogFile){

    cout << "loading dialog from: " << dialogFile << endl;
    dialogFileName=dialogFile;
    readTextFromFile("data\\"+dialogFile);

}

void DataHandlerDialog::makeVoiceFile(dialogPart* dp, int beatNumber){

    cout << "this is where we make voice files!" << endl;

    stringstream ss;
    ss << beatNumber;

    ofstream myfile;
    string fn="data\\voice\\"+ss.str()+".txt";
    myfile.open (fn.c_str());
    myfile << dp->dialogText;;
    myfile.close();

    string command="\"C:\\Program Files (x86)\\Linguatec\\VoiceReaderStudio15\\vrs15cmd.exe\" ";
    command+= "voice="+voices[dp->whoIs]+ " ";
    command+= "format=wav ";
    command+= "input=data\\voice\\"+ ss.str()+".txt ";
    command+= "codepage=65001 coding=9 ";
    command+= "output=data\\voice\\"+ss.str()+".wav";
    cout << command << endl;
    LPSTR szRun=(char*)command.c_str();
    PROCESS_INFORMATION     pi;
    STARTUPINFOA            si;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char szDir[1024];
    GetCurrentDirectoryA(sizeof(szDir), szDir);


    if (!CreateProcessA(NULL, szRun, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi))
    {
        return;
    }

    Sleep(5000);
    //ExitProcess(0);
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, "vrs15cmd.exe") == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
    Sleep(100);
    dp->dialogVoiceFile=ss.str()+".wav";
    dp->dialogVoice.loadSound(dp->dialogVoiceFile);

}

void DataHandlerDialog::toWav(const std::string &s){


    LPSTR szRun="test.bat";
    PROCESS_INFORMATION     pi;
    STARTUPINFOA            si;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char szDir[1024];
    GetCurrentDirectoryA(sizeof(szDir), szDir);


    if (!CreateProcessA(NULL, szRun, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi))
    {
        return;
    }

    Sleep(3000);
    //ExitProcess(0);
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, "vrs15cmd.exe") == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);



    //TerminateProcess(pi.hProcess,0);
    //CloseHandle(pi.hThread);

	/*
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,s.size());
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg),s.c_str(),s.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
	*/

}

void DataHandlerDialog::readTextFromFile(string filename){

    TiXmlDocument doc( filename );
    if (!doc.LoadFile()) {
        cout << "Cannot find dialog file, or dialog file corrupt. Exiting..." << endl;
        return;
        }


    TiXmlHandle hDoc(&doc);
    TiXmlElement * element;
    TiXmlHandle hRoot(0);

    //***********************************************************************
    //Get the "Dronedialog" element
    //***********************************************************************
    element=hDoc.FirstChildElement().Element();
    // should always have a valid root but handle gracefully if it doesn't
    if (!element) return;

    // save this for later
    hRoot=TiXmlHandle(element);
    element=hRoot.FirstChild( "SETTINGS" ).Element();

    if (!element)
        cout << "no settings element found!" << endl;
    //GLOBAL SETTINGS:
    droneOneVoice=element->Attribute("Drone1Voice");
    droneTwoVoice= element->Attribute("Drone2Voice");
    systemVoice=element->Attribute("SystemVoice");

    voices[0]=systemVoice;
    voices[1]=droneOneVoice;
    voices[2]=droneTwoVoice;

        cout << "loaded global voice settings..." << endl;


    //BEATS
    element=hRoot.FirstChild( "BEAT" ).Element();

    for( ; element!=NULL; element=element->NextSiblingElement("BEAT")){
        //cout << "next element: " << element->Value() << " " << element->GetText() <<endl;
        dialogPart myBeat;

        TiXmlElement * xmlProperty=NULL;                     //always initialise if you want to check against NULL!

        xmlProperty=element->FirstChildElement("TEXT");
        if (xmlProperty && xmlProperty->GetText()!=NULL){        //check if the saved file has this element
            int val=-1;
            xmlProperty->Attribute("WHO",&val);
            myBeat.whoIs=val;
            myBeat.dialogText=xmlProperty->GetText();
            cout << "YAY! Found some text!" << endl;
            cout << "Speaker is: " << voices[myBeat.whoIs] << endl;
            cout << "Found Text: " << myBeat.dialogText << endl;
        }
        else
            cout << "no text in beat!" << endl;

        /*
        xmlProperty=element->FirstChildElement("IMAGE");
        if (xmlProperty && xmlProperty->GetText()!=NULL){        //check if the saved file has this element
            myBeat.dialogImageFile=xmlProperty->GetText();
            if (myBeat.dialogImageFile!="NULL"){
                myBeat.dialogImage.loadImage(myBeat.dialogImageFile);
                cout << "YAY! Loaded the picture: " << myBeat.dialogImageFile << endl;
            }
        }
        else
            cout << "no picture in beat!" << endl;
        */
        xmlProperty=element->FirstChildElement("VOICE");
        if (xmlProperty && xmlProperty->GetText()!=NULL){        //check if the saved file has this element
            myBeat.dialogVoiceFile=xmlProperty->GetText();
            if (myBeat.dialogVoiceFile!="NULL")
                myBeat.dialogVoice.loadSound("voice\\" + myBeat.dialogVoiceFile);
            else
                makeVoiceFile(&myBeat,dialog.size());
        }
        else{
            cout << "no voice in beat. Generating..." << endl;
            makeVoiceFile(&myBeat,dialog.size());
        }

    dialog.push_back(myBeat);

    }

}

void DataHandlerDialog::writeTextToFile(string filename){


    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
    TiXmlElement * root = new TiXmlElement( "Dronedialog" );
    doc.LinkEndChild( root );


//save controller location and rotation
    TiXmlElement * settingsElement=new TiXmlElement("SETTINGS");
    settingsElement->SetAttribute("Drone1Voice",droneOneVoice);
    settingsElement->SetAttribute("Drone2Voice",droneTwoVoice);
    settingsElement->SetAttribute("SystemVoice",systemVoice);
    root->LinkEndChild(settingsElement);
    cout << "saved global voice settings..." << endl;

    //first Actors
    for (unsigned int i=0;i<dialog.size();i++)
      {
      TiXmlElement * beatElement=new TiXmlElement("BEAT");

        TiXmlElement * textElement=new TiXmlElement("TEXT");
        textElement->SetAttribute("WHO",dialog[i].whoIs);
        textElement->LinkEndChild(new TiXmlText( unlinebreak(dialog[i].dialogText)) );
        beatElement->LinkEndChild(textElement);

        TiXmlElement * voiceElement=new TiXmlElement("VOICE");
        voiceElement->LinkEndChild(new TiXmlText( dialog[i].dialogVoiceFile) );
        beatElement->LinkEndChild(voiceElement);

      root->LinkEndChild(beatElement);
      }

    doc.SaveFile("data\\dialoge\\" + filename );
}

string DataHandlerDialog::unlinebreak(string text){

    unsigned found=0;
    while (found!=string::npos){
        found=text.find('\n');
        if (found!=string::npos)
            text.replace(found,1," ");
    }

    return text;
}

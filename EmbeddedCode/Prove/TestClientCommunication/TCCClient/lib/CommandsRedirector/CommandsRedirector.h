#ifndef COMMANDS_REDIRECTOR
#define COMMANDS_REDIRECTOR

/*
    LIST OF COMMANDS
    About files 
        /deleteAudio/audioName
        /uploadAudio/audioName file
        /connectTossid/ssid
        /pswforssid/psw
        /clearwifisettings
    
    About devices
    /test/device
    /setvol/device=dev&volume=vol
    /setSound/device=dev&sound=soundName
    /setLoop/device=dev&loop=[on, off]
    

*/

class CommandsRedirector
{
public:
    CommandsRedirector();
    bool ReadCommand(String cmd);

private:

};

bool CommandsRedirector::ReadCommand(String cmd)
{
}

#endif
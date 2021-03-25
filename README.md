![](https://raw.githubusercontent.com/RaffaelloFornasiere/SoundArtProject/main/SoundArtLogoSM.png)

# SoundArt Project
#### 
Powered by [Raffaello Fornasiere](https://raffaellofornasiere.github.io/)
### Origin
**SoundArt Project** was born from a Master's thesis on Primary Educations Science. 
The aim is to makes possible to childrens to discover and experience different combination of sounds in realation with their body.
In order to achive this, SoundArt Project provides and extensible system composed by modules that reproduces sounds whenever 
something is posed in front of them. Thanks to multiple devices placed at different places they can move their arms and legs to 
play sounds and different combination of them. 

### Description
The system is composed by indipendend modules capable to detect the presence of a body and reproduce a pre-set sound when the body 
is close enought to the module. All modules are provided by an ultrasonic sensor, that detect the presenco of the body, and a speaker
that reprocudes the setted sound. 
The following features are by now supported (hoping in future additions):
- Setting any .mp3 audio (**basic feature**)
  - (*work in progress*) mulitple file extensions 
- Control the volume of the module (**basic feature**)
- Loop mode: always restart the audio when finished (**basic feature**)
- Web page controlled
- Auto connect to wifi
- (*work in progress*) Setting new wifi
- auto detecting of new modules


### Tecnical details
The systems is based on a server-client architecture, where each module is a client except one that is the server. 
The all modules connects to the same wifi (by now there is the assumption that there is at most one known wifi)
and the user connects to the local page hosted by the server (a DSN is not supported yet, so user can connect to 
the AP provided by the server, connect to the gateway ip and get the ip of the other network).
Once on the page the user have access to all functionalities.

##### Board
The project is developed for the ESP8266 and by now is the only supported uC. 

##### Ultrasonic Sensor
To detect the presence of a body, the HC-SR04 sensor have been used. Since the reproduction of audio 
needs the device not to have long delays, it has been used and async library found [here](https://github.com/jazzycamel/arduino/).

##### ESP8266Audio
In order to reproduce .mp3 files I used [this](https://github.com/earlephilhower/ESP8266Audio) library that perfectly met the needs of the project. 
Thanks to it, each device is able to read an mp3 file from the SD card and then send audio to a PCM5102 through I2S protocol, that will provide 
the input for a class-d amplifier (a generic amp based on tpa3118).






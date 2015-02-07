//-DARDUINO_ARCH_AVR

#include <Time.h>
#include <stream.h>
#include "StringStream.h"

// this is for my home open door  need to reverse low ad high
//#define REVERSEOUPUT  

#define OLED
#define MEGA
//#define SERIAL_DEBUG_OFF

#ifdef OLED
#include "OLED.h"
int previousSecond;
#endif


#ifdef MEGA
//MEGA
  HardwareSerial& dbgTerminal = Serial;
  #define DEBUG_SERIAL_SPEED 115200
  #define BUFFER_SIZE 1024
#endif

#ifdef NANO
// Nano
#include <SoftwareSerial.h>
  SoftwareSerial dbgTerminal(10, 11); // RX, TX
  #define DEBUG_SERIAL_SPEED 19200
  #define BUFFER_SIZE 100
#endif


#define EEPROM_EVENTS_START_ADDRESS 20
#define NUM_OF_INPUT_PORTS 2
#define RL0 2 //pin 3 drives relay 3 
#define RL1 3 //pin 3 drives relay 3 
#define RI0 12 //pin 8 drives relay 7 


boolean Relays[] = {0,0,0,0,0};
boolean Inputs[] = {0,0,0,0,0};

int once=0;
bool portChanged=false;

Stream* WifiStream;

#include <avr/pgmspace.h>
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#ifndef SERIAL_DEBUG_OFF
#define dbgTerminalprint(x)     dbgTerminal.print (x)
#define dbgTerminalprintln(x)     dbgTerminal.println (x)
#else
#define dbgTerminalprint(x)    
#define dbgTerminalprintln(x)    
#endif

void setup()
{
  WifiStream = GetWifiSerial();
  dbgTerminal.begin(DEBUG_SERIAL_SPEED);

#ifdef OLED
  InitializeDisplay();
#endif

  #ifdef OLED
  println(0,F("Initializing"));
  println(1,F(""));
  println(2,F(""));
  println(3,F(""));
  #endif

  dbgTerminalprintln(F("initializing..."));

  InitializeWifi();

  pinMode (RL0,OUTPUT);
  pinMode (RL1,OUTPUT);
  Relays[0]=0;
  Relays[1]=0;

  pinMode (RI0,INPUT);
  SetOutputState();
   
  dbgTerminalprintln(F("_Started_"));

  #ifdef OLED
  printPortStatus();
  #endif

  #ifndef OLED
  SendIPToServer();
  #endif
 }

void SendIPToServer()
{
  // send IP to server
    String IP = F("184.106.153.149"); //http://api.thingspeak.com";
    String addr = GetIPAddress();
    int idx = addr.indexOf('.');
    while (idx != -1)
    {
      addr.remove(idx, 1); 
      idx = addr.indexOf('.');
    }
    String URI = F("GET /update?key=5ZFIUM7ZZE8RITMD&field1=") ;
    URI += addr;
  OpenWebage(IP, URI);    
}

void SetOutputState()
{

#ifndef REVERSEOUPUT
  digitalWrite(RL0,Relays[0] == 0 ? LOW : HIGH );
  digitalWrite(RL1,Relays[1] == 0 ? LOW : HIGH );
#else
  digitalWrite(RL0,Relays[0] == 0 ? HIGH : LOW );
  digitalWrite(RL1,Relays[1] == 0 ? HIGH : LOW );
#endif //REVERSEOUPUT
}


void loop()
{

  CheckPinOn();
  SetOutputState();
  process(WifiStream);
  
}

void CheckInput()
{
  Inputs[0] = digitalRead(RI0);
}
int sendcounttest = 0;
void CheckPinOn()
{
  boolean last = Inputs[0];
  CheckInput();
  if (once == 0 && Inputs[0] != 0 && last == 0)
  {
    once = 1;
    sendcounttest++;
    // String IP = "184.106.153.149"; //http://api.thingspeak.com";
    // String URI = "GET /update?key=WY9TFU3NWUX56PWM&field1=" + String(sendcounttest);
    // OpenWebage(IP, URI);
    SendPushNotification();
  }
  else if (last ==1 && Inputs[0] == 0)
  {
    once = 0;
  }
}


void SendPushNotification()
{
  // String IP = "184.106.153.149"; //http://api.thingspeak.com";
  // String URI = "GET /update?key=WY9TFU3NWUX56PWM&field1=" + String(sendcounttest);

  // Office door
  // String IP = "84.108.63.91"; //http://sshakuf.bug3.com";
  // String URI = "GET /~bobthebuilder/opendoor.php?key=openthedoor4123&action=ringring"; 
  
  // Shahaf notify
  // String IP = "31.170.166.127"; //http://sshakuf.bug3.com";
  // String URI = "GET /default2.php?key=ssn&action=notify"; 
  String IP = F("54.164.208.219"); //http://sshakuf.parseapp.com";
  String URI = F("GET /ESPRest/push.html"); 
  //http://sshakuf.bugs3.com/default2.php?key=ssn&action=notify

  OpenWebage(IP, URI);
}

void SetRelayState(String inClient)
{
  int eventId = inClient.toInt();
  Relays[eventId] = Relays[eventId] == 0 ? 1:0;     // to FIX!!!!!
  if (Relays[eventId] != 0){
    dbgTerminalprintln(String(F("Port ")) + eventId + F(" On"));
  }
  else{
    dbgTerminalprintln(String(F("Port ")) + eventId + F(" Off"));
  }
  portChanged = true;
  SetOutputState();
}

String GetPortsJson()
{
  String command =F("\"ports\":{");

  for (int i=0; i < NUM_OF_INPUT_PORTS; i++)
  {
   command +=  "\"RL" + String(i)+ "\":\"" + String(Relays[i]) + "\"";
   if (i < NUM_OF_INPUT_PORTS-1)
   {
      command += ",";
   }
  }
  command += '}';
  return command;
}
void SendOutputStateJson(int ch_id)
{
  String command;

  command = "{";
  command += GetPortsJson();
  command += "}\r\n";

  SendResponseJson(ch_id, &command);
  
}

void printPortStatus()
{
  #ifdef OLED
  String str = "";
  for (int eventId=0; eventId < NUM_OF_INPUT_PORTS; eventId++)
  {
    if (Relays[eventId] != 0){
      str += String("P") + eventId + " On ";
    }
    else{
      str += String("P") + eventId + " Off ";
    }
  }
  println(3,str);

  #endif
}


void homepage(int ch_id);


String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

typedef struct 
{
  char* command;
  void(*f)(int, String);
} RestPtrs;


void doFlipinput(int ch_id, String inURL)
{
    dbgTerminalprintln("doFlipinput --" + getValue(String(inURL),'/',1));
    SetRelayState(getValue(String(inURL),'/',1));
    SendOutputStateJson(ch_id);

}
void doOpen(int ch_id, String inURL)
{
    dbgTerminalprintln("doOpen --" );
    SetRelayState("0");

  String command = F("{'door':'opened'}");
  SendResponseJson(ch_id, &command);
  delay(500);
  clearSerialBuffer(true);
  delay(2500);
  SetRelayState("0");
}
void doStatus(int ch_id, String inURL)
{
    dbgTerminalprint(F("Status command recived"));
    SendOutputStateJson(ch_id);

}

RestPtrs RestPtrsTable[] = { 
  {"flipinput",&doFlipinput},
  {"open", &doOpen},
  {"status", &doStatus},
};
#define NUMOFCOMMANDS 3

void process(Stream* client)
{

  char buffer[BUFFER_SIZE];

  int ch_id, packet_len;
  char *pb;  

if (dbgTerminal.available())
  {
    client->write(dbgTerminal.read());
  }

  
  while (client->available())
  {
  // clear buffer
  memset(buffer,0,sizeof(buffer));

  client->readBytesUntil('\n', buffer, BUFFER_SIZE);
  // if (buffer[0] != '\0')
  // {
  //   dbgTerminalprint("::");
  //   dbgTerminalprintln(buffer);
  // }
  

  if(strncmp(buffer, "+IPD,", 5)==0) {
    int count =0;
    while (count < 50)
    {
      dbgTerminalprint("--> ");
      count++;
      String str = client->readStringUntil('\n');
      dbgTerminalprintln(str);
      if ((str.indexOf("Link") != -1) || str.indexOf("OK") != -1 || str.indexOf("Unlink") != -1)
      {
        break;
      }
    }


    pb = buffer;
    // request: +IPD,ch,len:data
    sscanf(buffer+5, "%d,%d", &ch_id, &packet_len);
    dbgTerminalprintln("connected - " + String(ch_id) + " size-" + String(packet_len));
    if (packet_len > 0) {
      // read serial until packet_len character received
      // start from :
      pb = buffer+5;
      while(*pb!=':') pb++;
      pb++;
      if (strncmp(pb, "GET /", 5) == 0) {
        // dbgTerminalprintln("GET /");
        pb +=5;
        
        String URL = String(pb);
        String command = getValue(String(URL),'/',0);
        // dbgTerminalprintln("URL - " + URL);


        bool handeled = false;
        for (int idx = 0; idx < NUMOFCOMMANDS; idx++)
        {
          // dbgTerminalprintln(command + " " + RestPtrsTable[idx].command);
          if (command == RestPtrsTable[idx].command)
          {
            RestPtrsTable[idx].f(ch_id, URL);
            handeled = true;
            clearSerialBuffer(true);
            break;
          }
        }
        if (!handeled)
        {
          // dbgTerminalprintln("NO REST REPLAY");
        }
      }
    }
  }
}
  #ifdef OLED
  
  if(second()!=previousSecond)
    {
      previousSecond = second();
      if(portChanged)
      {
        portChanged=false;
        printPortStatus();
      }
      LCDLoop();
    }
  #endif //OLED


}




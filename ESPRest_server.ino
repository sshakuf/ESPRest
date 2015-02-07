////// ESP version 0019000902

// #define SSID "klar_wifi_EXT"
// #define PASS "idonoa2013"
// #define SSID "Backup"
// #define PASS "!Ch399!M3"
// #define SSID "STARS2"
#define SSID "STRS"
// #define SSID "STRS1"
#define PASS "1011040311037"

// HardwareSerial & dbgTerminal = Serial;


#ifdef OLED
#include "OLED.h"
#endif


////MEGA2560 
#ifdef MEGA
HardwareSerial & espSerial = Serial2;
#endif

#ifdef NANO
// Nano
#include <SoftwareSerial.h>
SoftwareSerial* GetDebugSerial();
HardwareSerial & espSerial = Serial;
#endif


// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
const int ESP8266_CHPD = 39;

// Variables will change:
int ledState = HIGH;             // ledState used to set the LED

// #define BUFFER_SIZE 1024
// #define CONTENT_LENGHT 1020


Stream* GetWifiSerial()
{
  return &espSerial;
}

String ipAddress;
String GetIPAddress()
{
  return ipAddress;
}
void InitializeWifi() { 
  // dbgTerminal.begin(115200); // Serial monitor
  espSerial.begin(115200 ); // ESP8266

  dbgTerminalprintln(F("Initialize Wifi ESP8266"));

  pinMode(ledPin, OUTPUT);  
  pinMode(ESP8266_CHPD, OUTPUT);
  digitalWrite(ESP8266_CHPD,HIGH); 


  delay(2000);

  clearSerialBuffer();

 //Change to mode 1
  GetResponse(F("AT+CWMODE=1"));

  //set the multiple connection mode
  GetResponse(F("AT+CIPMUX=1"));

  //connect to router
  // connectWiFi("CheggMain", "Ch399!M3");
  connectWiFi(SSID, PASS);

  //test if the module is ready
  delay(200);
  GetResponse(F("AT+GMR"));

  ipAddress = GetResponse(F("AT+CIFSR"));
  ipAddress.remove(ipAddress.length()-2,2); // remove OK in the end

  #ifdef OLED
  println(0,ipAddress);
  println(1,"Connected.");
  println(2," ");
  println(3," ");
  LCDLoop();
  #endif


  // //Change to mode 1
  // GetResponse(F("AT+CWMODE=1"));

  // //set the multiple connection mode
  // GetResponse(F("AT+CIPMUX=1"));

  // //set the server of port 80 check "no change" or "OK"
  GetResponse(F("AT+CIPSERVER=1,80"));

  //set time out
  GetResponse(F("AT+CIPSTO=3"));   // 15

  //print the ip addr

  dbgTerminalprintln(" ");

  dbgTerminalprintln(F("initialized..."));
  digitalWrite(ledPin,ledState); 
}

void OpenWebage(String inIP, String inURI)
{

  int count =0;
  clearSerialBuffer();

  String cmd = "AT+CIPSTART=4,\"TCP\",\"";
  cmd += inIP;
  cmd += "\",80";
  String res = SendESP(cmd,2000);
  if (res.indexOf("Linked")!= -1 || res.indexOf("ALREAY CONNECT") != -1)
  {
    String sendLen = "AT+CIPSEND=4,";
    sendLen += String(inURI.length()+2);    // FOR ADDING \R\N IN THE END !!!
    String res2 = SendESP(sendLen,1000);
    if (res2.indexOf('>') != -1)
    {
      String res3 = SendESP(inURI,1000);

    }
    clearSerialBuffer(true);
    delay(4000);
    clearSerialBuffer(true);
    
    String res4 = GetResponse("AT+CIPCLOSE");
  }
}

void SendESP(int ch_id, String* Content, boolean isJson)
{
    SendESPPacket(ch_id, Content, isJson, true);
}

void SendESPPacket(int ch_id, String* Content, boolean isJson, boolean shouldClose)
{

  String Header = F("HTTP/1.1 200 OK\r\n");
  if (isJson){
    Header += F("Content-Type: application/json\r\n");
  }
  else
  {
    Header += F("Content-Type: text/html\r\n");
  }

  Header +=F("Access-Control-Allow-Origin: *\n\r");
  Header +=F("Access-Control-Allow-Methods: GET, POST, PUT\n\r");
  if (shouldClose)
  {
    Header += F("Connection: close\r\n");
  }
  Header += F("Content-Length: ");
  Header += (int)(Content->length());
  Header += "\r\n\r\n";

  SendESPPacket(ch_id, Header, Content);
}



void SendESPPacket(int ch_id, String Header, String* Content)
{

  clearSerialBuffer(true);

  if (!Content->endsWith("\r\n"))
  {
    Content->concat("\r\n");
  }


  while (espSerial.available() > 0)
  {
    unsigned int p = espSerial.read();
  #ifndef NANO
    dbgTerminal.write(p);
  #endif //nano
  }
  espSerial.print(F("AT+CIPSEND="));
  espSerial.print(String(ch_id));
  espSerial.print(",");
  espSerial.println(Header.length()+Content->length());
  
  #ifndef NANO
  dbgTerminalprint("AT+CIPSEND " + String(ch_id) + ", ");
  dbgTerminalprintln(Header.length()+Content->length());
  #endif  // nano

  delay(10);
  //WaitESPInput();
  while (espSerial.available() > 0)
  {
    unsigned int p = espSerial.read();
    dbgTerminal.write(p);
    if (p == '>')   // need to validate
    {
  #ifndef NANO
      dbgTerminalprintln("");
      dbgTerminalprintln("found ");
  #endif //NANO
      espSerial.print(Header);
      espSerial.print(*Content);
      delay(10);
      break;
    }
  }
  dbgTerminalprintln(F("SendESPPacket END"));
}


void SendResponseJson(int ch_id, String* Content)
{
  SendESP(ch_id, Content, true);
}

void SendResponse(int ch_id, String* Content)
{
  SendESP(ch_id, Content, false);
}

void homepage(int ch_id) {
  // String Header;

  // Header += "Content-Type: text/html\r\n";
  // Header += "Connection: close\r\n";  
  // //Header += "Refresh: 5\r\n";

  // String Content;
  // Content = "Hello World!";
  // Content += String(ledState);

  // Header += "Content-Length: ";
  // Header += (int)(Content.length());
  // Header += "\r\n\r\n";


  // espSerial.print("AT+CIPSEND=");
  // espSerial.print(String(ch_id));
  // espSerial.print(",");
  // espSerial.println(Header.length()+Content.length());
  // delay(10);

  // if (espSerial.find(">")) {
  //   espSerial.print(Header);
  //   espSerial.print(Content);
  //   delay(10);
  // }
}

// Get the data from the WiFi module and send it to the debug serial port
String GetResponse(String AT_Command){
  String tmpData;
  
  dbgTerminalprint(AT_Command);
  dbgTerminalprint(" : ");
  espSerial.println(AT_Command);
  
  // delay(wait);
  delay(150);
  WaitESPInput();
  while (espSerial.available() >0 )  {
    char c = espSerial.read();
    tmpData += c;
    
    if ( tmpData.indexOf(AT_Command) > -1 )         
      tmpData = "";
    else
      tmpData.trim();       
          
   }
  dbgTerminalprintln(tmpData);
   return tmpData;
}

String SendESP(String AT_Command, int wait){
  String tmpData;
  
  dbgTerminalprint(AT_Command);
  dbgTerminalprint(" : ");
  espSerial.println(AT_Command);
  
  delay(wait);
  // delay(150);
  WaitESPInput();
  while (espSerial.available() >0 )  {
    char c = espSerial.read();
    tmpData += c;
    
    int idx = tmpData.indexOf(AT_Command);
    if ( idx > -1 )         
      tmpData = tmpData.substring(idx + AT_Command.length());
    // else
    //   tmpData.trim();       
          
   }
  dbgTerminalprintln(tmpData);
  if (tmpData.indexOf("busynow") > -1)
  {
    hardReset();
  }
  return tmpData;
}


boolean hardReset() {
  String tmpData;

  digitalWrite(ESP8266_CHPD,LOW);
  delay(100);
  digitalWrite(ESP8266_CHPD,HIGH);
  delay(1000);

  while ( espSerial.available() > 0 ) {
    char c = espSerial.read();
    tmpData +=c;
    espSerial.write(c);
    if ( tmpData.indexOf("Ready") > -1 ) {
      //Serial.println("Ready");
      return 1;
    } 
  }
}

boolean WaitESPInput()
{
  int timeout = 5000;
  int count =0;
  while (espSerial.available() <= 0 ) 
  {
   delay(1);
   count++;
   if (count >= timeout)
    return false; 
  }
  return true;
}

void clearSerialBuffer(void) {
  //dbgTerminalprintln("clearSerialBuffer");
  while ( espSerial.available() > 0 ) {
  espSerial.read();
  }
}
void clearSerialBuffer(bool inEcho) {
  //dbgTerminalprintln("clearSerialBuffer");
  while ( espSerial.available() > 0 ) {
  unsigned int i = espSerial.read();
  if (inEcho != false)
  {
  #ifndef NANO
    dbgTerminal.write(i);
  #endif // nano
  }
  }
}

boolean WaitForIP()
{
  String s = ".";
  int count = 0;
  while (count < 10)
  {

    #ifdef OLED
    println(2,s);
    s+=".";
    LCDLoop();
    #endif

    delay(1000);
    count++;
    ipAddress = GetResponse(F("AT+CIFSR"));
    dbgTerminalprint(F("WAIT- "));
    dbgTerminalprintln(ipAddress);
    ipAddress.trim();
    dbgTerminalprintln(ipAddress);
    dbgTerminalprintln(String(ipAddress.indexOf(F("ERROR"))));
    if (ipAddress.indexOf(F("ERROR")) == -1 && ipAddress.indexOf(F("busy")) == -1)
    { 
      if (ipAddress.indexOf(".") > 0)
      {
        return true;
      }
    }
  }

  return false;
}

boolean connectWiFi(String NetworkSSID,String NetworkPASS) {
  String cmd = F("AT+CWJAP=\"");
  cmd += NetworkSSID;
  cmd += "\",\"";
  cmd += NetworkPASS;
  cmd += "\"";

  #ifdef OLED
  println(0,F("connecting"));
  println(1,NetworkSSID);
  LCDLoop();
  #endif


//  for (int i=4; i >0 ; i--)
//  {
//    dbgTerminalprint(i); 
    delay(2000);
//  }
  dbgTerminalprintln(""); 

  //dbgTerminalprintln(cmd); 
  dbgTerminalprintln(GetResponse(cmd));
  WaitForIP();

}


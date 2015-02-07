void SendResponse(int ch_id, String* Content);
String str;
void SendHTML(int ch_id, String ipAddress)
{
  str="";
  str+=F("<!DOCTYPE html><html><body><script type='text/javascript'>window.location.replace('http://sshakuf.parseapp.com/SchedulerWifiESP/html.html?ip=");
  str+= ipAddress;
  str+= F("');</script></body></html>");
str+=F("");
SendResponse(ch_id, &str);
}

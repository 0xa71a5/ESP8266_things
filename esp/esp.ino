#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#define led 16
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);
int count=0;
String essidStr="";
String passwdStr="";


void webTask()
{
    WiFiClient client1;
    String host="119.29.5.72";
    Serial.println("Query for www.baidu.com");
    if (client1.connect(host.c_str(), 6666))
    {
      digitalWrite(led,0);
      Serial.println("connected");
      client1.print("at+password=qwertyuiop");
      while (client1.connected())
      {
        if (client1.available())
        {
          String line = client1.readStringUntil('\n');
          Serial.println(line);
        }
      }
      client1.stop();
      Serial.println("Communication End");
    }
    else
    {
      Serial.println("Connection failed!");
      client1.stop();
    }
    digitalWrite(led,1);
    Serial.println("Done!");
        while(1)
    {
      delay(10);
    }
}
String prepareHtmlPage()
{
  String htmlPage = 
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            //"Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE html><html><body><div align='center'><form action='/' method='get'>  Wifi Name: <input type='text' name='essid' /><br><br>  Wifi Password: <input type='text' name='password' /><br><br>  <input type='submit' value='Connect' /></form></div></body></html>"+
            "\r\n";
  return htmlPage;
}
void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(led,OUTPUT);
  digitalWrite(led,0);
  delay(1000);
  digitalWrite(led,1);
  
  //while(Serial.available()==0);
  
}
bool newSetting=false;
int stateMachine=0;//0:AP 1:Station
void loop() {
 // Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
        while(stateMachine==0)
        {
          Serial.print("Setting soft-AP configuration ... ");
          Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
        
          Serial.print("Setting soft-AP ... ");
          Serial.println(WiFi.softAP("ESPsoftAP_01") ? "Ready" : "Failed!");
        
          Serial.print("Soft-AP IP address = ");
          Serial.println(WiFi.softAPIP());
          server.begin();
          Serial.println("Server is listening on port 80");

          while(1)
          {
                WiFiClient client = server.available();  // wait for a client (web browser) to connect
                if (client)
                {
                  Serial.println("\n[Client connected]");
                  while (client.connected())
                  {
                    // read line by line what the client (web browser) is requesting
                    if (client.available())
                    {
                      String line = client.readStringUntil('\r');
                      Serial.print(line);
                      if(line.indexOf("essid")!=-1&&line.indexOf("POST")!=-1)
                      {
                        int index1=line.indexOf("essid");
                        int index2=line.indexOf("&",index1);
                        int index3=line.indexOf("password",index2);
                        int index4=line.indexOf("HTTP",index3);
                        essidStr=line.substring(index1+6,index2);
                        passwdStr=line.substring(index3+9,index4-1);
                        newSetting=true;
                      }
                      if (line.length() == 1 && line[0] == '\n')
                      {
                        client.println(prepareHtmlPage());
                        if(newSetting)
                        {
                          Serial.println("\nNew setting!");
                          Serial.print("SSID:");Serial.println(essidStr);
                          Serial.print("PASSWORD:");Serial.println(passwdStr);
                          newSetting=false;
                          stateMachine=0;//1
                        }
                        break;
                      }
                    }
                  }
                  delay(1); 
                  client.stop();
                  Serial.println("[Client disonnected]");
                  if(stateMachine!=0)
                    break;
                }
          }
        }
        
        while(stateMachine==1)
        {
          if(WiFi.isConnected())
          {
            WiFi.disconnect();
            Serial.println("Disconnect!");
            delay(10);
          }
          WiFi.begin(essidStr.c_str(),passwdStr.c_str());
          Serial.print("Connecting to wifi "+essidStr);
          Serial.println(WiFi.localIP());
          while (WiFi.status() != WL_CONNECTED)
            {
              delay(500);
              Serial.print(".");
            }
            Serial.println();
            Serial.print("Connected, IP address: ");
            Serial.println(WiFi.localIP());
            while(1)
            {
              Serial.println("Enter webTask()");
              webTask();
            }
        }
  }

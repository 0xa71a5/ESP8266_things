#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#define led 16
char data;
String Base64(String pSrc);
char Encode_GetChar(byte num);
String RawInput(String out);
String cardNumber;
String password;
void setup() {
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  digitalWrite(led,1);
  delay(1000);
  cardNumber=RawInput("\nCardNumber:");
  password=RawInput("Password:");
  password=Base64(password);
  Serial.print("Password Base64:");
  Serial.println(password);
  while(Serial.available()==0);
  data=Serial.read();
  if(WiFi.isConnected())
          {
            WiFi.disconnect();
            Serial.println("Disconnect!");
            delay(10);
          }
     WiFi.begin("seu-wlan");    
     Serial.print("Connecting to wifi seu-wlan");
        
    while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print(".");
      }   
     Serial.print("Connected, IP address: ");
            Serial.println(WiFi.localIP()); 
}

void loop() {
  if(data=='e')
  {
    WiFiClient client1;
    if (client1.connect("119.29.5.72", 6666))
    {
      Serial.println("Connected to 119.29.5.72");
      client1.print("password=qwertyuiop");
      while (client1.connected())
      {
        if (client1.available())
        {
          String line = client1.readStringUntil('\n');
          Serial.println(line);
          if(line.indexOf("on")!=-1)
            digitalWrite(led,0);
          else if(line.indexOf("off")!=-1)
            digitalWrite(led,1);
        }
      }
       client1.stop();
       Serial.println("Communication End");
    }
    else
    {
      Serial.println("Connection failed");
    }
  }
  if(data=='w')
  {
    WiFiClient client1;
    Serial.println("Try to login seuwlan");
    if (client1.connect("121.248.62.222", 80))
    {
      Serial.println("Connected!\nSending!");
      String *loginStr=new String("POST http://w.seu.edu.cn/index.php/index/login HTTP/1.1\r\nHost: w.seu.edu.cn\r\nConnection: keep-alive\r\nContent-Length: 64\r\nAccept: application/json, text/javascript, */*; q=0.01\r\nOrigin: http://w.seu.edu.cn\r\nX-Requested-With: XMLHttpRequest\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://w.seu.edu.cn/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.8\r\nCookie: think_language=zh-CN; PHPSESSID=q25m4hapdqj5fkdtug3jej2pb5\r\n\r\nusername="+cardNumber+"&password="+password+"&enablemacauth=0");
      //String loginStr="POST http://w.seu.edu.cn/index.php/index/login HTTP/1.1\r\nHost: w.seu.edu.cn\r\nConnection: keep-alive\r\nContent-Length: 64\r\nAccept: application/json, text/javascript, */*; q=0.01\r\nOrigin: http://w.seu.edu.cn\r\nX-Requested-With: XMLHttpRequest\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://w.seu.edu.cn/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.8\r\nCookie: think_language=zh-CN; PHPSESSID=q25m4hapdqj5fkdtug3jej2pb5\r\n\r\nusername="+cardNumber+"&password="+password+"&enablemacauth=0";
      client1.print(*loginStr);
      delete loginStr;
      Serial.println("Finished writing!");
      Serial.println("Recv:\n");
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
      Serial.println("Failed to connect!");
    }
    data=' ';
  }
  while(Serial.available()==0);
  data=Serial.read();

}

String RawInput(String out)
{
  String ret="";
  Serial.println(out);
  while(Serial.available()==0);
  while(Serial.available())
  {
    ret+=char(Serial.read());
    delay(2);
  }
  return ret;
}

char Encode_GetChar(byte num)
{
    return 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/="[num];
}

String Base64(String pSrc)
{
    char pDest[30];
    byte input[3], output[4];
    size_t i, index_src = 0, index_dest = 0;
    int srclen=pSrc.length();
    for(i = 0; i < srclen; i += 3)
    {
        //char [0]
        input[0] = pSrc[index_src++];
        output[0] = (byte)(input[0] >> 2);
        pDest[index_dest++] = Encode_GetChar(output[0]);
        //char [1]
        if(index_src < srclen)
        {
            input[1] = pSrc[index_src++];
            output[1] = (byte)(((input[0] & 0x03) << 4) + (input[1] >> 4));
            pDest[index_dest++] = Encode_GetChar(output[1]);
        }
        else
        {
            output[1] = (byte)((input[0] & 0x03) << 4);
            pDest[index_dest++] = Encode_GetChar(output[1]);
            pDest[index_dest++] = '=';
            pDest[index_dest++] = '=';
            break;
        }
        
        //char [2]
        if(index_src < srclen)
        {
            input[2] = pSrc[index_src++];
            output[2] = (byte)(((input[1] & 0x0f) << 2) + (input[2] >> 6));
            pDest[index_dest++] = Encode_GetChar(output[2]);
        }
        else
        {
            output[2] = (byte)((input[1] & 0x0f) << 2);
            pDest[index_dest++] = Encode_GetChar(output[2]);
            pDest[index_dest++] = '=';
            break;
        }

        //char [3]
        output[3] = (byte)(input[2] & 0x3f);
        pDest[index_dest++] = Encode_GetChar(output[3]);
    }
    //null-terminator
    pDest[index_dest] = 0;
    String ret(pDest);
    return ret;
}

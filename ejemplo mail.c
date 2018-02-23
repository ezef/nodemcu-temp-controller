#include <ESP8266WiFi.h>  // the ESP8266WiFi.h  lib
#DEFINE SMPT_SERVER "smtp.gmail.com";
#DEFINE SMPT_PORT 25;
#DEFINE SMPT_USER "";
#DEFINE SMPT_PASS "";	

const char* SSID = "YourSSID";
const char* PASS = "YourPW";

ADC_MODE(ADC_VCC);

WiFiClient client;
void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  byte ret = sendEmail();
}

void loop()
{
}

byte sendEmail()
{

  if (client.connect(SMPT_SERVER, SMTP_PORT) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!eRcv()) return 0;

  Serial.println(F("Sending EHLO"));
  client.println("EHLO www.facebook.com/elmasoncervezaartesanal/");
  if (!eRcv()) return 0;
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!eRcv()) return 0;
  Serial.println(F("Sending User"));
  // Change to your base64, ASCII encoded user
  client.println(SMPT_USER); // SMTP UserID
  if (!eRcv()) return 0;
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  client.println(SMPT_PASS);//  SMTP Passw
     if (!eRcv()) return 0;
    Serial.println(F("Sending From"));   // change to your email address (sender)
   client.println(F("MAIL From: cervezaelmason@gmail.com"));// not important 
   if (!eRcv()) return 0;   // change to recipient address
    Serial.println(F("Sending To"));
    client.println(F("RCPT To: cervezaelmason@gmail.com"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending DATA"));
    client.println(F("DATA"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending email"));   // change to recipient address
   client.println(F("To: cervezaelmason@gmail.com"));   // change to your address
   client.println(F("From: cervezaelmason@gmail.com"));
 client.println(F("Subject: Banco de Frio no da abasto\r\n"));
    client.print(F("Power is: "));
    client.print(ESP.getVcc());
    client.println(F("mV"));
    client.print(F("Device Chip ID: "));
    client.println(ESP.getChipId());
    Serial.print(F("Voltage is: "));
    Serial.print(ESP.getVcc());
    client.println(F("."));
    if (!eRcv()) return 0;
    Serial.println(F("Sending QUIT"));
    client.println(F("QUIT"));
    if (!eRcv()) return 0;
    client.stop();
    Serial.println(F("disconnected"));
    return 1;
  }
  byte eRcv()
	  {
		byte respCode;
		byte thisByte;
		int loopCount = 0;
		while (!client.available())
	  {
		  delay(1);
		  loopCount++;     // if nothing received for 10 seconds, timeout
		  if (loopCount > 10000) {
		  client.stop();
		  Serial.println(F("\r\nTimeout"));
		  return 0;
		}
	  }

	  respCode = client.peek();
	  while (client.available())
	  {
		thisByte = client.read();
		Serial.write(thisByte);
	  }

	  if (respCode >= '4')
	  {
		//  efail();
		return 0;
	  }
	  return 1;
	}	
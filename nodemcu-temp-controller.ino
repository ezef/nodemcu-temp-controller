/*
 * Demonstrate using an http server and an HTML form to control an LED.
 * The http server runs on the ESP8266.
 *
 * Connect to "http://esp8266WebForm.local" or "http://<IP address>"
 * to bring up an HTML form to control the LED connected GPIO#0. This works
 * for the Adafruit ESP8266 HUZZAH but the LED may be on a different pin on
 * other breakout boards.
 *
 * Imperatives to turn the LED on/off using a non-browser http client.
 * For example, using wget.
 * $ wget http://esp8266webform.local/ledon
 * $ wget http://esp8266webform.local/ledoff
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <tempo.h>
#include "constantes.h"

#define ADDR1 0 //direccion de la EEPROM para la tempSet1
#define ADDR2 1 // direccion de la EEPROM para la tempSet2
#define ADDR3 2 //direccion de la EEPROM para la tempSet3
#define ADDR4 3 // direccion de la EEPROM para la tempSet4

#define RELAY1 5 
#define RELAY2 4 
#define RELAY3 0 
#define RELAY4 2 

#define ONE_WIRE_BUS 14
#define TEMPERATURE_PRECISION 9
#define HISTERESIS 0.3

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465



// Fill in your WiFi router SSID and password
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

byte tempset1;
byte tempset2;
byte tempset3;
byte tempset4;

byte relay1 = 0;
byte relay2 = 0;
byte relay3 = 0;
byte relay4 = 0;

float tempsensada1;
float tempsensada2;
float tempsensada3;
float tempsensada4;
float tempsensada5;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress direccionsensor1 = { 0x28, 0xEE, 0xE3, 0xF, 0x15, 0x16, 0x2, 0xAF };
DeviceAddress direccionsensor2 = { 0x28, 0xEE, 0x3A, 0x15, 0x18, 0x16, 0x1, 0xD3 };
DeviceAddress direccionsensor3 = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
DeviceAddress direccionsensor4 = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
DeviceAddress direccionsensor5 = { 0x28, 0xEE, 0x2C, 0x93, 0x19, 0x16, 0x1, 0x48 };

Tempo t_temp(15*1000); // temporizador para la lectura de temperatura
unsigned long ultimoTiempo;
int frecuenciaEnvioMails = (60 * 1000);


ESP8266WebServer server(80);
WiFiClientSecure client;

String html_login(){
return "<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>ESP8266 Web Form Demo</title>"
"</head>"
"<body>"
"  <FORM action=\"/\" method=\"post\">"
"    <P>"
"      Login: "
"      <INPUT type=\"text\" name=\"password\">  "    
"      <INPUT type=\"submit\" value=\"Enviar\">"
"      <br/>"
"    </P>"
"  </FORM>"  
"</body>"
"</html>";
}

String html_principal(){
String ret= "<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>Control Masonico</title>"
"</head>"
"<body>"
"  <h1>Control Masonico</h1>"
"  <FORM action=\"/ferm1\" method=\"post\">"
"    <P>"
"      Fermentador 1: <b><label for=\"sarasa\">";
ret +=tempsensada1;
ret += "</label></b> grados ---- Seteado: <label for=\"sarasa\">";
ret += EEPROM.read(ADDR1);
ret += "</label> Grados" 
"      <INPUT type=\"text\" name=\"tempset\">"      
"      <INPUT type=\"submit\" value=\"Enviar\">"
"      <br/>"
"    </P>"
"  </FORM>"
"  <FORM action=\"/ferm2\" method=\"post\">"
"    <P>"
"      Fermentador 2: <b><label for=\"sarasa\">";
ret +=tempsensada2;
ret += "</label></b> grados ---- Seteado: <label for=\"sarasa\">";
ret += EEPROM.read(ADDR2);
ret += "</label> Grados" 
"      <INPUT type=\"text\" name=\"tempset\">"      
"      <INPUT type=\"submit\" value=\"Enviar\">"
"      <br/>"
"    </P>"
"  </FORM>"
"  <FORM action=\"/ferm3\" method=\"post\">"
"    <P>"
"      Fermentador 3: <b><label for=\"sarasa\">";
ret +=tempsensada3;
ret += "</label></b> grados ---- Seteado: <label for=\"sarasa\">";
ret += EEPROM.read(ADDR3);
ret += "</label> Grados" 
"      <INPUT type=\"text\" name=\"tempset\">"      
"      <INPUT type=\"submit\" value=\"Enviar\">"
"      <br/>"
"    </P>"
"  </FORM>"
"  <FORM action=\"/ferm4\" method=\"post\">"
"    <P>"
"      Fermentador 4: <b><label for=\"sarasa\">";
ret +=tempsensada4;
ret += "</label></b> grados ---- Seteado: <label for=\"sarasa\">";
ret += EEPROM.read(ADDR4);
ret += "</label> Grados "
"      <INPUT type=\"text\" name=\"tempset\">" 
"      <INPUT type=\"submit\" value=\"Enviar\">"
"      <br/>"
"    </P>"
"  </FORM>"
"  <br />Temperatura Banco: ";
ret +=tempsensada5;
ret += " Grados"
"</body>"
"</html>";
return ret;
}

void handleRoot()
{
  if (server.hasArg("password")) {
    handleSubmit();
  }
  else {
    server.send(200, "text/html", html_login());
  }
}

void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit()
{
  String pass;

  if (!server.hasArg("password")) return returnFail("BAD ARGS");
  pass = server.arg("password");
  if (pass == "angela") {
    server.send(200, "text/html", html_principal());
  }  else {
    returnFail("shupala");
  }
}

void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
void setferm1(){
  if ( server.arg("tempset") != ""){
    Serial.print(" Seteo de Ferm 1: ");
    Serial.println(server.arg("tempset"));
    tempset1= (byte)server.arg("tempset").toInt();
    EEPROM.put(ADDR1,tempset1);
    EEPROM.commit();
    returnOK();
  }else{
    returnFail("Temperatura seteada Vacia");
  }  
}
void setferm2(){
  if ( server.arg("tempset") != ""){
    Serial.print(" Seteo de Ferm 2: ");
    Serial.println(server.arg("tempset"));
    tempset2= (byte)server.arg("tempset").toInt();
    EEPROM.put(ADDR2,tempset2);
    EEPROM.commit();
    returnOK();
  }else{
    returnFail("Temperatura seteada Vacia");
  }  
}
void setferm3(){
  if ( server.arg("tempset") != ""){
    Serial.print(" Seteo de Ferm 3: ");
    Serial.println(server.arg("tempset"));
    tempset3= (byte)server.arg("tempset").toInt();
    EEPROM.put(ADDR3,tempset3);
    EEPROM.commit();
    returnOK();
  }else{
    returnFail("Temperatura seteada Vacia");
  }  
}
void setferm4(){
  if ( server.arg("tempset") != ""){
    Serial.print(" Seteo de Ferm 4: ");
    Serial.println(server.arg("tempset"));
    tempset4= (byte)server.arg("tempset").toInt();
    EEPROM.put(ADDR4,tempset4);
    EEPROM.commit();
    returnOK();
  }else{
    returnFail("Temperatura seteada Vacia");
  }  
}
void setup(void)
{
  Serial.begin(115200);
  delay(10);
  EEPROM.begin(512);
  delay(10);
  sensors.begin();
  delay(10);
  WiFi.begin(ssid, password);
  delay(10);
  Serial.println("");
  ultimoTiempo=millis();

  // Wait for connection
  int veces=0;
  while (WiFi.status() != WL_CONNECTED && veces < 6) {
    delay(500);
    Serial.print(".");
    veces++;
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  server.on("/", handleRoot);
  server.on("/ferm1", setferm1);
  server.on("/ferm2", setferm2);
  server.on("/ferm3", setferm3);
  server.on("/ferm4", setferm4);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("Connect to http://");
  Serial.println(WiFi.localIP());

  tempset1= EEPROM.read(ADDR1);
  tempset2= EEPROM.read(ADDR2);
  tempset3= EEPROM.read(ADDR3);
  tempset4= EEPROM.read(ADDR4);

  getTemps();

  Serial.print(" Sensor1: ");
  Serial.print(tempsensada1);
  Serial.print(" Sensor2: ");
  Serial.print(tempsensada2);
  Serial.print(" Sensor3: ");
  Serial.print(tempsensada3);
  Serial.print(" Sensor4: ");
  Serial.print(tempsensada4);
  Serial.print(" Sensor5: ");
  Serial.println(tempsensada5);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
}
void getTemps(){
  sensors.requestTemperatures();
  tempsensada1= sensors.getTempC(direccionsensor1);
  tempsensada2= sensors.getTempC(direccionsensor2);
  tempsensada3= sensors.getTempC(direccionsensor3);
  tempsensada4= sensors.getTempC(direccionsensor4);
  tempsensada5= sensors.getTempC(direccionsensor5);
}

void informar(){
  WiFiClient client;
  
  if (client.connect(DATOS_IP,DATOS_PORT) == 1) {
    Serial.println("conectado a la API");
    client.println("GET /api.php?tempsensada1=" + String(tempsensada1) + "&tempset1="+String(tempset1)+"&accioncontrol1="+String(relay1)+"&tempsensada2="+String(tempsensada2)+"&tempset2="+String(tempset2)+"&accioncontrol2="+String(relay2)+"&tempsensada3="+String(tempsensada3)+"&tempset3="+String(tempset3)+"&accioncontrol3="+String(relay3)+"&tempsensada4="+String(tempsensada4)+"&tempset4="+String(tempset4)+"&accioncontrol4="+String(relay4)+"&tempsensada5="+String(tempsensada5)+" HTTP/1.0");
    Serial.println("GET /api.php?tempsensada1=" + String(tempsensada1) + "&tempset1="+String(tempset1)+"&accioncontrol1="+String(relay1)+"&tempsensada2="+String(tempsensada2)+"&tempset2="+String(tempset2)+"&accioncontrol2="+String(relay2)+"&tempsensada3="+String(tempsensada3)+"&tempset3="+String(tempset3)+"&accioncontrol3="+String(relay3)+"&tempsensada4="+String(tempsensada4)+"&tempset4="+String(tempset4)+"&accioncontrol4="+String(relay4)+"&tempsensada5="+String(tempsensada5)+" HTTP/1.0");
    client.println("Host: cervezaelmason.com.ar");
    Serial.println("Host: cervezaelmason.com.ar");
    client.println();
    Serial.println();
    int loopCount = 0;
    while (!client.available() && loopCount <= 3000)
    {
      delay(1);
      loopCount++;     // if nothing received for 10 seconds, timeout
    }
    if (loopCount >= 3000) {
      Serial.println(F("\r\nTimeout"));
    }else{
      Serial.println(loopCount);
      String msj=client.readStringUntil('\r');
      Serial.println(msj);
    }
    client.stop();
  } else {
    Serial.println(F("fallo conexion a la api"));
  }
}

void control(){
  getTemps();
  
  if (tempsensada1 > tempset1 + HISTERESIS){
    digitalWrite(RELAY1, LOW);
    Serial.print("Relay 1 ON - T1:");
    Serial.println(tempsensada1);
    relay1=true;
  }else{
    if (tempsensada1 < tempset1 - HISTERESIS){
      digitalWrite(RELAY1,HIGH);
      Serial.print("Relay 1 OFF- T1:");
      Serial.println(tempsensada1);
      relay1=false;
    }
  } 

  if (tempsensada2 > tempset2 + HISTERESIS){
    digitalWrite(RELAY2, LOW);
    Serial.print("Relay 2 ON - T2:");
    Serial.println(tempsensada2);
    relay2= true;
  }else{
    if (tempsensada2 < tempset2 - HISTERESIS){
      digitalWrite(RELAY2,HIGH);
      Serial.print("Relay 2 OFF- T2:");
      Serial.println(tempsensada2);
      relay2=false;
    }
  }

  if (tempsensada3 > tempset3 + HISTERESIS){
    digitalWrite(RELAY3, LOW);
    Serial.print("Relay 3 ON - T3:");
    Serial.println(tempsensada3);
    relay3 = true;
  }else{
    if (tempsensada3 < tempset3 - HISTERESIS){
      digitalWrite(RELAY3,HIGH);
      Serial.print("Relay 3 OFF- T3:");
      Serial.println(tempsensada3);
      relay3=false;
    }
  }

  if (tempsensada4 > tempset4 + HISTERESIS){
    digitalWrite(RELAY4, LOW);
    Serial.print("Relay 4 ON - T4:");
    Serial.println(tempsensada4);
    relay4=true;
  }else{
    if (tempsensada4 < tempset4 - HISTERESIS){
      digitalWrite(RELAY4,HIGH);
      Serial.print("Relay 4 OFF- T4:");
      Serial.println(tempsensada4);
      relay4=false;
    }
  }

  //si el banco de frio se calienta, enviar mail avisando
  if ((millis() - ultimoTiempo)> frecuenciaEnvioMails){
    ultimoTiempo = millis();
    if (tempsensada5 >= tempset1 || tempsensada5 >= tempset2){
      sendEmail();
    }
  }
  informar();
 
}

byte sendEmail()
{

  if (client.connect(SMTP_SERVER, SMTP_PORT) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!eRcv()) return 0;

  Serial.println(F("Sending EHLO"));
  client.println("EHLO www.elmason.com.ar");
  if (!eRcv()) return 0;
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!eRcv()) return 0;
  Serial.println(F("Sending User"));
  // Change to your base64, ASCII encoded user
  client.println(SMTP_USER); // SMTP UserID
  if (!eRcv()) return 0;
  Serial.println(F("Sending Password"));
  // change to your base64, ASCII encoded password
  client.println(SMTP_PASS);//  SMTP Passw
     if (!eRcv()) return 0;
    Serial.println(F("Sending From"));   // change to your email address (sender)
   client.println(F("MAIL From: <cervezaelmason@gmail.com>"));// not important 
   if (!eRcv()) return 0;   // change to recipient address
    Serial.println(F("Sending To"));
    client.println(F("RCPT To: <cervezaelmason@gmail.com>"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending DATA"));
    client.println(F("DATA"));
    if (!eRcv()) return 0;
    Serial.println(F("Sending email"));   // change to recipient address
   client.println(F("To: cervezaelmason@gmail.com"));   // change to your address
   client.println(F("From: cervezaelmason@gmail.com"));
 client.println(F("Subject: Banco de Frio no da abasto\r\n"));
    client.print(F("Temp1: "));
    client.println(tempsensada1);
    client.print(F("Temp2: "));
    client.println(tempsensada2);
    client.print(F("Temp3: "));
    client.println(tempsensada3);
    client.print(F("Temp4: "));
    client.println(tempsensada4);
    client.print(F("Temp5: "));
    client.println(tempsensada5);
    client.println(WiFi.localIP());
    client.println("Eh Chileno que se siente vivir en un pasillo");
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


void loop(void)
{
  server.handleClient();
  if (t_temp.state()){ // realiza la lectura de la temperatura, actualiza el lcd y comanda los relays.
    control();
  }
  
}

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

#define ADDR1 0 //direccion de la EEPROM para la tempSet1
#define ADDR2 1 // direccion de la EEPROM para la tempSet2
#define ADDR3 2 //direccion de la EEPROM para la tempSet3
#define ADDR4 3 // direccion de la EEPROM para la tempSet4



// Fill in your WiFi router SSID and password
const char* ssid = "cokiba";
const char* password = "colegioki2000";

byte tempset1;
byte tempset2;
byte tempset3;
byte tempset4;

float tempsensada1;
float tempsensada2;
float tempsensada3;
float tempsensada4;





ESP8266WebServer server(80);

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
  if (pass == "angelica") {
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
    EEPROM.put(ADDR1,(byte)server.arg("tempset").toInt());
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
    EEPROM.put(ADDR2,(byte)server.arg("tempset").toInt());
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
    EEPROM.put(ADDR3,(byte)server.arg("tempset").toInt());
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
    EEPROM.put(ADDR4,(byte)server.arg("tempset").toInt());
    EEPROM.commit();
    returnOK();
  }else{
    returnFail("Temperatura seteada Vacia");
  }  
}
void setup(void)
{

  Serial.begin(115200);
  EEPROM.begin(512);
  WiFi.begin(ssid, password);
  Serial.println("");

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
  Serial.print("Connect to http://esp8266WebForm.local or http://");
  Serial.println(WiFi.localIP());

  tempset1= EEPROM.read(ADDR1);
  tempset2= EEPROM.read(ADDR2);
  tempset3= EEPROM.read(ADDR3);
  tempset4= EEPROM.read(ADDR4);

  tempsensada1=12;
  tempsensada2=13;
  tempsensada3=14;
  tempsensada4=15;
  
}

void loop(void)
{
  server.handleClient();
  
}

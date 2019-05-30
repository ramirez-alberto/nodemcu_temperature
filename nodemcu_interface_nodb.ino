#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <math.h>

// Incluimos librería
#include <DHT.h>
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

const char *ssid = "test";
const char *password = "testtest";
const char *host = "esp8266-weather.herokuapp.com";

ESP8266WebServer server(80);

const String HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const String HTTP_STYLE = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.6rem;background-color:#1fb3ec;color:#fdd;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .button2 {background-color: #008CBA;} .button3 {background-color: #f44336;} .button4 {background-color: #e7e7e7; color: black;} .button5 {background-color: #555555;} .button6 {background-color: #4CAF50;} </style>";
const String HTTP_SCRIPT = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const String HTTP_HEAD_END = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const String HTTP_TEMP_STYLE = "<style>html {background-color: rgb(0, 238, 177);font-family: 'Courier New', Courier, monospace;font-size: 1rem;color: white;padding-bottom: 50px;}html,body {height: 100%;-ms-box-sizing: border-box;-webkit-box-sizing: border-box;-moz-box-sizing: border-box;box-sizing: border-box;}.container {min-height: 100%;min-width: 250px;max-width: 500px;margin: 0 auto;border: 8px solid aliceblue;border-radius: 10px;}span {margin: 1rem;display: block;text-align: center;}.temp_c {font-size: 5rem;}.secundario {font-weight: bold;}.hum_val {color: rgba(255, 255, 255, 0.8);}</style></head><body><div class=\"container\"><center><span class=\"temp_c\">";
const String HTTP_TEMP_END = "</span><div class=\"secundario\"><span class=\"humedad\">Humedad</span><span class=\"hum_val\">";
const String HOMEPAGE = "<form action=\"/loginCL\" method=\"POST\"><input type=\"text\" name=\"ssid\"><input type=\"password\" name=\"pass\"> <button type=\"submit\">Submit</button></form><br/><form action=\"/temp\" method=\"get\"><button class=\"button6\">TEMP</button></form><br/> <form action=\"/cmd3\" method=\"get\"><button class=\"button2\">Blue</button></form><br/><form action=\"/cmd4\" method=\"get\"><button class=\"button4\">Off</button></form><br/>    ";

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot()
{
  String s = HTTP_HEAD;
  s += HTTP_STYLE;
  s += HTTP_SCRIPT;
  s += HTTP_HEAD_END;

  s += HOMEPAGE;
  server.send(200, "text/html", s);
}

void loginCL()
{
  if (server.hasArg("ssid") && server.hasArg("pass"))
  {
    Serial.println(server.arg("ssid") + " " + server.arg("pass"));
    wifiConnect(server.arg("ssid"), server.arg("pass"));
    String s = "Connected";
    server.send(200, "text/html", s);
  }
}

void wifiConnect(const String cli_ssid, const String pass_cli)
{
  //Conectamos a la red WiFi
  Serial.println();
  Serial.println("Conectando a ");
  Serial.println(cli_ssid);
  WiFi.disconnect();
  WiFi.begin(cli_ssid, pass_cli);
  //Esperamos a que se conecte a el ssid
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // Mostramos la IP
}

void showTemp()
{

  String s = HTTP_HEAD;
  s += HTTP_TEMP_STYLE;

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t))
  {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    s += "<h1>Error obteniendo los datos del sensor DHT11</h1>";
    server.send(200, "text/html", s);  
    return;
  }

  
  Serial.printf("\nMemoria libre en el ESP8266: %d Bytes\n\n", ESP.getFreeHeap());
  Serial.println();

  int a = round(t);
 

  String page = "";
  page = String(a) + HTTP_TEMP_END;
  page += String(h) + " %</span>"+"</br>Memoria libre en el ESP8266: " + String(ESP.getFreeHeap()) + "Bytes</br>"+"</div></center></div></body></html>";
  
  s += page;

  server.send(200, "text/html", s);  
}

void setup()
{
  //Inicia el monitor serial
  Serial.begin(9600);
  delay(10);
  Serial.println("starting");
  dht.begin();

  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  WiFi.mode(WIFI_AP_STA);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/loginCL", loginCL);
  server.on("/temp", showTemp);

  // Inicializamos el servidor HTTP.
  server.begin();

  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
}

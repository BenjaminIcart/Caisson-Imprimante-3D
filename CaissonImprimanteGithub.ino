#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHT.h"
#include <EEPROM.h>

WiFiClient client; 

char* ssid = "YOUR_SSID"; // votre SSID
char* password = "YOUR_PASSWORD"; // votre mot de passe wifi
const char* host = "maker.ifttt.com";
MDNSResponder mdns; 
long port = 3100;
ESP8266WebServer server(port); // serveur HTTP

int relayPin = D8;
int relayPin2 = D7;

DHT dht(D6, DHT11);
float t;
float h;
String Etat = "...";
int temperatureMax = 0;  // Valeur à stocker
String UrlVideo = "http://X.X.X.X:";

void writeStringToEEPROM(int addr, const String &str) {
    int len = str.length();
    EEPROM.write(addr, len); // Stocker la longueur de la chaîne
    for (int i = 0; i < len; i++) {
        EEPROM.write(addr + 1 + i, str[i]); // Stocker chaque caractère
    }
    EEPROM.commit();
}

String readStringFromEEPROM(int addr) {
    int len = EEPROM.read(addr); // Lire la longueur de la chaîne
    char data[len + 1]; // Créer un tableau de caractères pour stocker la chaîne
    for (int i = 0; i < len; i++) {
        data[i] = EEPROM.read(addr + 1 + i); // Lire chaque caractère
    }
    data[len] = '\0'; // Ajouter un caractère nul pour terminer la chaîne
    return String(data); // Convertir le tableau de caractères en String
}




void handleRoot() {
    String html = "<html><body>";
    html += "<h1>Temp&eacuterature et humidit&eacutee actuelles</h1>";
    html += "<p>Temp&eacuterature: " + String(t) + "</p>";
    html += "<p>Humidit&eacutee: " + String(h) + "</p>";

    html += "<h1>Temperature Max</h1>";
    html += "<p>Temp&eacuterature actuelle max: " + String(temperatureMax) + "</p>";
    html += "<form action='/setTemperatureMax' method='GET'>";
    html += "Temp&eacuterature Max: <input type='number' name='value'>";
    html += "<input type='submit' value='Set'>";
    html += "</form>";

    html += "<h1>Url Video</h1>";
    html += "<p>Url vid&eacuteo actuelle: " + UrlVideo + "</p>";
    html += "<form action='/setUrlVideo' method='GET'>";
    html += "Url Video: <input type='text' name='url'>";
    html += "<input type='submit' value='Set'>";
    html += "</form>";

    // Add video player section
    html += "<h1>Vid&eacuteo Stream</h1>";
    html += "<img style='display: block; -webkit-user-select: none; margin: auto; background-color: hsl(0, 0%, 25%);' ";
    html += "src='" + UrlVideo + "' width='1065' height='599'>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}



void handleSetTemperatureMax() {
    if (server.hasArg("value")) {
        int newTemperatureMax = server.arg("value").toInt();
        if (newTemperatureMax > 0) { // validation simple
            temperatureMax = newTemperatureMax;
            EEPROM.write(0, temperatureMax);  // Stocker dans l'EEPROM à l'adresse 0
            EEPROM.commit();
            server.send(200, "text/plain", "temperatureMax updated to " + String(temperatureMax));
            return;
        }
    }
    server.send(400, "text/plain", "Invalid Request");
}

void handleSetUrlVideo() {
    if (server.hasArg("url")) {
        String newUrlVideo = server.arg("url");
        writeStringToEEPROM(1, newUrlVideo);  // Stocker à l'adresse 1 (en évitant l'adresse 0)
        UrlVideo = newUrlVideo;
        server.send(200, "text/plain", "UrlVideo updated to " + UrlVideo);
        return;
    }
    server.send(400, "text/plain", "Invalid Request");
}


void handleEtat()
{
    server.send(200, "text/plain", Etat);
}


void handleTemp()
{
    server.send(200, "text/plain", String(t,1));
}


void handleHumidite()
{
    server.send(200, "text/plain", String(h,1));
}


void connect(char *_SSID, char* _PWD) 
{
  Serial.println("");
  Serial.print("Connecting ");
  Serial.print(_SSID);
  Serial.print(" password = ");
  Serial.print( _PWD);

  WiFi.begin(_SSID, _PWD);
  Serial.println("");

  int h = 0;

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");

    if (h++ > 40) 
    { // si trop long on abandonne
      Serial.println();
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}


void setup() {
    Serial.begin(9600);
    EEPROM.begin(512);

    pinMode(relayPin, OUTPUT);
    pinMode(relayPin2, OUTPUT);
    dht.begin();

    // Serveur web configuration
    server.on("/", handleRoot);
    server.on("/etat", handleEtat);
    server.on("/temperature", handleTemp);
    server.on("/humidite", handleHumidite);
    server.on("/setTemperatureMax", handleSetTemperatureMax); // Nouvelle route
    server.on("/setUrlVideo", handleSetUrlVideo);

    connect(ssid, password); // connexion au reseau Wifi

    if (mdns.begin("esp8266", WiFi.localIP())) {
        Serial.println("MDNS responder started");
    }
    server.begin();
    Serial.println("HTTP server started");

    // Lire la valeur stockée dans l'EEPROM
    temperatureMax = EEPROM.read(0);
    Serial.print("Initial temperatureMax: ");
    Serial.println(temperatureMax);

    UrlVideo = readStringFromEEPROM(1); // Lire l'URL stockée à partir de l'adresse 1
    Serial.print("Stored UrlVideo: ");
    Serial.println(UrlVideo);
}


 
void loop() {
    server.handleClient(); // gestion du serveur
    h = dht.readHumidity();
    t = dht.readTemperature();
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Temperature: ");
    Serial.println(t);
    delay(1000);

    if (t <= temperatureMax) {
        digitalWrite(relayPin2, HIGH);
        digitalWrite(relayPin, HIGH);
        Etat = "Eteins";
    } else {
        digitalWrite(relayPin2, LOW);
        digitalWrite(relayPin, LOW);
        Etat = "Allumés";
    }
}

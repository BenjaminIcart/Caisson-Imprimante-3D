#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>   //#
#include "DHT.h"
#include <EEPROM.h>


WiFiClient client; 

const char* ssid = "YOUR_SSID"; // votre SSID
const char* password = "YOUR_PASSWORD"; // votre mot de passe wifi
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

const char* Loginauth = "YOUR_LOGIN";
const char* Passwordauth = "YOUR_PASSWORD";
ESP8266HTTPUpdateServer httpUpdater(true);



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

  if (!server.authenticate(Loginauth, Passwordauth)){
    return server.requestAuthentication();
  }
    String html = "<!DOCTYPE html>"
                  "<html lang='fr'>"
                  "<head>"
                  "<meta charset='UTF-8'>"
                  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                  "<title>Contrôle de Température et Vidéo</title>"
                  "<link href='https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap' rel='stylesheet'>"
                  "<style>"
                  "body {"
                  "    font-family: 'Poppins', sans-serif;"
                  "    margin: 0;"
                  "    padding: 0;"
                  "    background-color: #ffff;"
                  "    color: #333;"
                  "}"
                  ".container {"
                  "    max-width: 800px;"
                  "    margin: 0 auto;"
                  "    padding: 20px;"
                  "}"
                  "h1 {"
                  "    font-size: 2.5rem;"
                  "    color: #007bff;"
                  "    text-align: center;"
                  "    margin-bottom: 20px;"
                  "}"
                  "p {"
                  "    font-size: 1.25rem;"
                  "    text-align: center;"
                  "    margin: 10px 0;"
                  "}"
                  "form {"
                  "    text-align: center;"
                  "    margin: 20px 0;"
                  "}"
                  "input[type='number'],"
                  "input[type='text'] {"
                  "    font-size: 1rem;"
                  "    padding: 10px;"
                  "    margin: 5px;"
                  "    border: 1px solid #ddd;"
                  "    border-radius: 5px;"
                  "    width: 200px;"
                  "}"
                  "input[type='submit'] {"
                  "    font-size: 1rem;"
                  "    padding: 10px 20px;"
                  "    border: none;"
                  "    border-radius: 5px;"
                  "    background-color: #007bff;"
                  "    color: #fff;"
                  "    cursor: pointer;"
                  "    transition: background-color 0.3s ease;"
                  "}"
                  "input[type='submit']:hover {"
                  "    background-color: #0056b3;"
                  "}"
                  "img {"
                  "    display: block;"
                  "    margin: 20px auto;"
                  "    max-width: 100%;"
                  "    border: 2px solid #ddd;"
                  "    border-radius: 10px;"
                  "}"
                  "</style>"
                  "</head>"
                  "<body>"
                  "<div class='container'>"
                  "<h1>Température et Humidité</h1>"
                  "<p>Température: " + String(t) + "°C</p>"
                  "<p>Humidité: " + String(h) + "%</p>"
                  "<h1>Température Max</h1>"
                  "<p>Température actuelle max: " + String(temperatureMax) + "°C</p>"
                  "<form action='/setTemperatureMax' method='GET'>"
                  "Température Max:"
                  "<input type='number' name='value'>"
                  "<input type='submit' value='Définir'>"
                  "</form>"
                  "<h1>Url Vidéo</h1>"
                  "<p>Url vidéo actuelle: <a href='" + UrlVideo + "' target='_blank'>" + UrlVideo + "</a></p>"
                  "<form action='/setUrlVideo' method='GET'>"
                  "Url Vidéo:"
                  "<input type='text' name='url'>"
                  "<input type='submit' value='Définir'>"
                  "</form>"
                  "<h1>Vidéo Stream</h1>"
                  "<img src='" + UrlVideo + "' alt='Stream Video'>"
                  "</div>"
                  "</body>"
                  "</html>";
    server.send(200, "text/html", html);
}



void handleSetTemperatureMax() {
    if (!server.authenticate(Loginauth, Passwordauth)){
      return server.requestAuthentication();
    }
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
    if (!server.authenticate(Loginauth, Passwordauth)){
      return server.requestAuthentication();
    }
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

    // Connexion au Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Configuration OTA
    if (!MDNS.begin("esp8266")) {
        Serial.println("Error setting up MDNS responder!");
    }
    httpUpdater.setup(&server, "/update", Loginauth, Passwordauth);
    server.begin();
    Serial.println("HTTP server started");

    // Routes pour le serveur web
    server.on("/", handleRoot);
    server.on("/etat", handleEtat);
    server.on("/temperature", handleTemp);
    server.on("/humidite", handleHumidite);
    server.on("/setTemperatureMax", handleSetTemperatureMax);
    server.on("/setUrlVideo", handleSetUrlVideo);

    // Lire les valeurs de l'EEPROM
    temperatureMax = EEPROM.read(0);
    Serial.print("Initial temperatureMax: ");
    Serial.println(temperatureMax);

    UrlVideo = readStringFromEEPROM(1);
    Serial.print("Stored UrlVideo: ");
    Serial.println(UrlVideo);
}


 
void loop() {
    server.handleClient(); // Traitement des requêtes HTTP

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


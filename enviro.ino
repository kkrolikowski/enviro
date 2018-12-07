#include <dht11.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

/***************************************** Inicjalizacja Sprzetu ********************************/


// sensor settings
dht11 sensor;
#define SENSOR_PIN 2
const char * sensor_id = "sensor1";

// wifi settings
const char *ssid = "wifi_ssid";
const char *pass = "wifi_pass";

// stats collector
const char *server = "backend.server.name";
WiFiClientSecure http;

void setup() 
{
	Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while( WiFi.status()  != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
	int sensStatus = sensor.read(SENSOR_PIN);
  String httphdrs, httpReq;
	Serial.print("Sensor status: ");
	switch (sensStatus)
	{
	case DHTLIB_OK:
		ConsolePrint(sensor);
    httphdrs = buildHTTPHeaders();
    httpReq = buildHTTPReq(sensor);
     if ( http.connect(server, 443))
     {
      Serial.println("HTTP server connected");
      http.println(httpReq);
      http.println(httphdrs);
      http.println();
     }
		break;
	case DHTLIB_ERROR_CHECKSUM:
		Serial.print("Sensor data CHECKSUM ERROR\n");
		break;
	case DHTLIB_ERROR_TIMEOUT:
		Serial.print("Sensor timeout\n");
		break;
	default:
		Serial.print("Unknown ERROR\n");
		break;
	}

	delay(1000);
}
void ConsolePrint(dht11 data)
{
	Serial.print("Humidity (%): ");
	Serial.print((float)data.humidity, 2);
	Serial.print("\t\t");
	Serial.print("Temperature (C): ");
	Serial.println((float)data.temperature, 2);
}
String buildHTTPReq(dht11 data)
{
  String httpreq = "GET /collect.php?temp=";
  httpreq += String((float)data.temperature, 2);
  httpreq += "&hum=";
  httpreq += String((float)data.humidity,2);
  httpreq += "&sensor_id=";
  httpreq += sensor_id;
  httpreq += " HTTP/1.0";
  
  return httpreq;
}
String buildHTTPHeaders()
{
  String hdr = "Host: ";
  hdr += server;

  return hdr;
}


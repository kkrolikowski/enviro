#include <dht11.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <base64.h>

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

// API Authentication
String apiuser = "apilogin";
String apipass = "password";

// LED pins
int wifi_led = 15;
int api_ok = 13;
int api_err = 12;

void setup() 
{
  pinMode(wifi_led, OUTPUT);
  pinMode(api_ok, OUTPUT);
  pinMode(api_err, OUTPUT);
  
	Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while( WiFi.status()  != WL_CONNECTED)
  {
    digitalWrite(wifi_led, LOW);
    delay(500);
    Serial.print(".");
  }
  digitalWrite(wifi_led, HIGH);
  
  Serial.println("");
  Serial.println("Wifi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
	int sensStatus = sensor.read(SENSOR_PIN);
  String httphdrs, httpReq;
  String authplain = apiuser + ':' + apipass;
  String authdata = base64::encode(authplain);
  
  String json = jsonData(sensor);
  char buff[100];
  json.toCharArray(buff, 100);
  
	Serial.print("Sensor status: ");
	switch (sensStatus)
	{
	case DHTLIB_OK:
		ConsolePrint(sensor);

     if ( http.connect(server, 443))
     {
      digitalWrite(api_ok, HIGH);
      Serial.println("HTTP server connected");
      http.println("POST /collect.php HTTP/1.1");
      http.println("Host: " + String(server));
      http.println("User-Agent: Arduino/enviro IoT");
      http.println("Authorization: Basic " + authdata);
      http.println("Content-type: application/json");
      http.print("Content-Length: ");
      http.println(strlen(buff));
      http.println();
      http.println(buff);
      http.println();
      delay(500);
      digitalWrite(api_ok, LOW);
     }
     else
     {
      digitalWrite(api_err, HIGH);
      delay(500);
      digitalWrite(api_err, LOW);
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

	ESP.deepSleep(60e6);
}
void ConsolePrint(dht11 data)
{
	Serial.print("Humidity (%): ");
	Serial.print((float)data.humidity, 2);
	Serial.print("\t\t");
	Serial.print("Temperature (C): ");
	Serial.println((float)data.temperature, 2);
}
String jsonData(dht11 sensor_data)
{
  String data = "{\"sensor_id\":\"";
  data += sensor_id;
  data += "\",\"temp\":\"";
  data += String((float)sensor_data.temperature, 2);
  data += "\",\"hum\":\"";
  data += String((float)sensor_data.humidity, 2);
  data += "\"}";
  
  return data;
}


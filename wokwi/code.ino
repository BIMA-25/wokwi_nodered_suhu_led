#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>  // memanggil library DHT
#define DHTPIN 15   // pin data DHT22 dihubungkan ke GPIO2 (pin D4)
#define DHTTYPE DHT22   // menggunakan sensor DHT22
DHT dht(DHTPIN, DHTTYPE);  // inisialisasi sensor DHT

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); //setting wifi chip sebagai station/client
  WiFi.begin(ssid, password); //koneksi ke jaringan wifi

  while (WiFi.status() != WL_CONNECTED) { //perintah tunggu esp32 sampi terkoneksi ke wifi
    delay(500);
    Serial.print(".");
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
dht.begin();   // mulai sensor DHT
}


void reconnect() { 
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
   
    String clientId = "10121022";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
 
      client.publish("/10121022/test/mqtt", "Testing");
 
      client.subscribe("/10121022/test/mqtt"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String data = "";

void callback(char* topic, byte* payload, unsigned int length) { 
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) { //mengecek jumlah data yang ada di topik mqtt
    
    data += (char)payload[i];
  }
  Serial.println(data);
  Serial.println();

  if(data == "hidup4"){
    digitalWrite(4,HIGH);
  }
  else if(data == "mati4"){
    digitalWrite(4,LOW);
  }

   if(data == "hidup5"){
    digitalWrite(5,HIGH);
  }
  else if(data == "mati5"){
    digitalWrite(5,LOW);
  }

  data = "";
}

unsigned long last = 0;
long interval = 100;

void loop() {
  float humidity = dht.readHumidity();  // membaca data kelembaban
 float temperature = dht.readTemperature();  // membaca data suhu dalam Celsius
 String suhu = String (temperature);
 String kelembaban = String (humidity);
  Serial.print("Kelembaban: "); 
  Serial.print(humidity);
  Serial.print(" %\t");  // tanda tab
  Serial.print("Suhu: "); 
  Serial.print(temperature);
  Serial.println(" *C");  // baris baru

  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if(now - last > interval){
    // publish ke mqtt
    client.publish("/10121022/test/kirim/humidity", kelembaban.c_str());
    client.publish("/10121022/test/kirim/temperature", suhu.c_str());
    last = now;
  }


}

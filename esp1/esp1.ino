#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "";//kullanıcının wifi adresi
const char* password = "";//ve şifresi
const char* mqtt_server = "broker.hivemq.com";//mqtt nin adresi

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// wifi'ye bağlanma fonksiyonu
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//mqtt'den gelen mesajların okunduğu fonksiyon
void callback(char* topic, byte* message, unsigned int length) {

  String messageTemp;

  for (int i = 0; i < length; i++) {
  
    messageTemp += (char)message[i];
  }


  if (String(topic) == "ESP-out_01") {
    messageTemp = messageTemp.substring(2);
    Serial.println("0.Kullanıcıdan Sana özel mesaj: " + messageTemp);
  }
  else if (String(topic) == "ESP-out_21") {
    messageTemp = messageTemp.substring(2);
    Serial.println("2.Kullanıcıdan Sana özel mesaj: " + messageTemp);
  }
  else if (String(topic) == "ESP-out_0") {
    Serial.println("0:" + messageTemp);
  }
  else if (String(topic) == "ESP-out_2") {
    Serial.println("2:" + messageTemp);
  }
}

//mqtt'ye bağlanma fonksiyonu
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("SEVERUS2001")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("ESP-out_01");
      client.subscribe("ESP-out_21");
      client.subscribe("ESP-out_2");
      client.subscribe("ESP-out_0");
      client.publish("ESP-out_1", "kullanıcı 1 sohbete katıldı.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial.available() > 0) {//kullanıcıdan veri okuma
    // ve o veriyi gereken yere gönderme

    String mes = Serial.readStringUntil('\n');
    if (mes.startsWith("0.")) {//özel mesaj
      Serial.println("Kullanıcı 0'a özel Mesaj:" + mes);
      client.publish("ESP-out_10", mes.c_str());

    }
    else if (mes.startsWith("2.")) {//özel mesaj
      Serial.println("Kullanıcı 2'ye özel Mesaj:" + mes);
      client.publish("ESP-out_12", mes.c_str());

    }
    else {//herkese gönderilen mesaj
      Serial.println("sen(1):" + mes);
      client.publish("ESP-out_1", mes.c_str());
    }

  }
}

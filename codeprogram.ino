//melakukan define template blynk yang sudah dirancang
#define BLYNK_TEMPLATE_ID "TMPL6gTTc866m"
#define BLYNK_TEMPLATE_NAME "AirQuality"
#define BLYNK_AUTH_TOKEN "oivD4Nh-C3YBANiGk71x44-b1FjOkw6G"

//memasukkan library yang digunakan
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiManager.h>
#include <DHT.h>

char auth[] = "oivD4Nh-C3YBANiGk71x44-b1FjOkw6G";

BlynkTimer timer;

//melakukan deklarasi pin LED dan buzzer
#define pinR 15
#define pinG 2
#define pinB 4
#define buzzer 5
int state = 0;

/// The load resistance on the board
#define RLOAD 10.0
/// Calibration resistance at atmospheric CO2 level
#define RZERO 76.63
/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

//deklarasi pin sensor DHT11
DHT dht(13, DHT11);
WiFiManager wm;

//perhitungan untuk mendapatkan nilai ppm pada kualitas udara dengan MQ135
float getResistance(int pin) {
  float val = analogRead(34);
  return ((1023./val) * 5. - 1.)*RLOAD;
}

float getPPM(int pin) {
  return PARA * pow((getResistance(pin)/RZERO), -PARB);
}

void airQuality()
{
  float ppm = getPPM(34);
  Serial.print("PPM: ");
  Serial.println(ppm);

  classState(ppm);
  Blynk.virtualWrite(V0, ppm); //menambahkan data kualitas udara yang terdeteksi ke blynk
  delay(2000);
}

//melakukan pendeteksian suhu dan kelembaban dengan sensor DHT11
void dhtSensor(){
  float suhu = dht.readTemperature(); //pembacaan suhu
  float hum = dht.readHumidity(); //pembacaan kelembaban

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.println(" °C");
  Serial.print("Kelembaban: ");
  Serial.print(hum);
  Serial.println(" %");
  Blynk.virtualWrite(V1, suhu); //menambahkan data suhu yang terdeteksi ke blynk
  Blynk.virtualWrite(V2, hum); //menambahkan data kelembaban yang terdeteksi ke blynk
}

//pengelompokkan kualitas udara serta output pada LED dan buzzer
void classState(float ppm){
  if((int)ppm < 400){
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(buzzer, HIGH);
    state=0;
  }else if((int)ppm >= 400 && (int)ppm < 600){
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(buzzer, HIGH);
    state=0;
  }else if((int)ppm >=600){
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(buzzer, LOW);
    state=1;
  }
  Blynk.virtualWrite(V4, state); 
  delay(500);
}

void setup()
{
  Serial.begin(115200);

  //melakukan pengkoneksian ke WiFi
  // wm.resetSettings();
  bool res;
  res = wm.autoConnect("SmokeDetector", "password");
  if (!res)
  {
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("Connected :)");
    Blynk.config(auth);
  }
  dht.begin(); //memulai sensor DHT11
  //inisialisasi pin pada buzzer dan LED sebagai output
  pinMode(buzzer, OUTPUT);
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  
  timer.setInterval(1000L, airQuality);
  timer.setInterval(1000L, dhtSensor);
}

void loop()
{
  //menambahkan kondisi LED saat perangkat tidak terhubung ke WiFi
  if (WiFi.status() != WL_CONNECTED) {

    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, LOW);
    digitalWrite(pinB, HIGH);

    wm.setConfigPortalTimeout(120); 
    wm.setTimeout(20);
    bool res;
    res = wm.autoConnect("SmokeDetector", "password");
    Serial.println("Connected to Wifi");
 }else{
    Blynk.run();
    timer.run();
 }
}

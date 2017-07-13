#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>
#include <DHT_U.h>

// First install "DHT sensor library" via the Library Manager
#include <DHT.h>


//initialize the LoRa modeule - RN2483 on SoftwareSerial
SoftwareSerial loraSerial = SoftwareSerial(8, 7);
#define debugSerial Serial

// Set your AppEUI and AppKey
const char *appEui = "70B3D57EF000630E";
const char *appKey = "DC02B9CD60F635CD719BE06DD9EDC792";

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

#define DHTPIN 2
#define soilSensor A5

//Choose your DHT sensor moddel
#define DHTTYPE DHT11
//#define DHTTYPE DHT21
//#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

  dht.begin();
}

void loop()
{
  debugSerial.println("-- LOOP");

  // Read sensor values and multiply by 100 to effictively have 2 decimals
  uint16_t humidity = dht.readHumidity(false) * 100;

  // false: Celsius (default)
  // true: Farenheit
  uint16_t temperature = dht.readTemperature(false) * 100;

  //soilSensor reading
  uint16_t moistureValue = analogRead(soilSensor) *100 ;

  // Split both words (16 bits) into 2 bytes of 8
  byte payload[6];
  payload[0] = highByte(temperature);
  payload[1] = lowByte(temperature);
  payload[2] = highByte(humidity);
  payload[3] = lowByte(humidity);
  payload[4] = highByte(moistureValue);
  payload[5] = lowByte(moistureValue);

  debugSerial.print("Temperature: ");
  debugSerial.println(temperature);
  debugSerial.print("Humidity: ");
  debugSerial.println(humidity);
  debugSerial.print("Moisture: ");
  debugSerial.println(moistureValue);

  ttn.sendBytes(payload, sizeof(payload));

  delay(20000);
}

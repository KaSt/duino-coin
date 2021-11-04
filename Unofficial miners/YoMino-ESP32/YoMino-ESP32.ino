/*
  ,------.          ,--.                       ,-----.       ,--.         
  |  .-.  \ ,--.,--.`--',--,--,  ,---. ,-----.'  .--./ ,---. `--',--,--,  
  |  |  \  :|  ||  |,--.|      \| .-. |'-----'|  |    | .-. |,--.|      \ 
  |  '--'  /'  ''  '|  ||  ||  |' '-' '       '  '--'\' '-' '|  ||  ||  | 
  `-------'  `----' `--'`--''--' `---'         `-----' `---' `--'`--''--' 
  YoMino 1.00 based on:
  Official code for ESP32 boards                            version 2.7.6
  
  Original description <-
    Duino-Coin Team & Community 2019-2021 © MIT Licensed
    https://duinocoin.com
    https://github.com/revoxhere/duino-coin

    If you don't know where to start, visit official website and navigate to
    the Getting Started page. Have fun mining!
  ->

  YoMino is a copy of the official ESP32 Miner 2.7.6 with SoftAP and
  screen output implemented so that mining config can be saved to flash 
  and you can check your progress on the lcd screen (if attached to the board).
  Devices can be flashed and configured later via a web page. 

  The first time you start your ESP32 you'll have to connect to it's Wifi.
  You'll see something like "YoMino-XXXXX" on the wifi networks available
  around you. Connect to that network then open a browser to 
  http://192.168.72.1
  Once the page is loaded, input your Miner configuration and define a Wifi
  password. Once the configuration is saved, the Miner will start to mine
  and protect the Wifi network "YoMino-XXXXX" with the password you chose.
  In case you want to change the configuration, no need to reflash, only
  to return to the configuration page. You can access it also locally to your 
  network once configured. Happy mining!
*/

/* Most of the code is from the original ESP32 miner.
 * Some others were put there by Ka.
 * 
 * Dev notes: Pins and screen are related to the board I developed it on.
 *            Change those values to the ones corresponding to your board.
 *            
 * Changelog:
 * Starting off the official ESP32 version 2.7.6
 *     1.0    * Use of the LCD screen on TTGO T-Display devices. 
 *              Two display modes available. Press button on GPIO0 at 
 *              boot to change from verical to horizontal. Designed for 
 *              a 240x135 pixels screen. 
 *            * SoftAP for miner web configuration, no need to burn the
 *              miner config in the code. Give a miner to someone who  
 *              wouldn't know how to flash it. 
 *            * Factory reset: press on the button connected to pin 35
 *              while resetting the device to wipe SPIFFS data at boot                 
 *     1.01   * Bugfixes, moved html in separate file.
 */


#define YOMINO_VERSION 1.01
#define OFFICIAL_VERSION 2.76
 
/***************** START OF MINER CONFIGURATION SECTION *****************/
// Change this to your WiFi SSID
String wifi_ssid;
// Change this to your WiFi wifi_password
String wifi_password;
// Change this to your Duino-Coin username
String username;
// Change this if you want a custom rig identifier
String rig_identifier;
//Miner AP Password
String ownPassword;


// Change this if your board has built-in led on non-standard pin
#define LED_BUILTIN 2
#define BLINK_SHARE_FOUND    1
#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 3
#define BLINK_RESET_DEVICE   5

// Define watchdog timer seconds
#define WDT_TIMEOUT 60

#include "hwcrypto/sha.h" // Uncomment this line if you're using an older
// version of the ESP32 core and sha_parellel_engine doesn't work for you
//#include "sha/sha_parallel_engine.h"  // Include hardware accelerated hashing library

/* If you would like to use mqtt monitoring uncomment
   the ENABLE_MQTT defition line(#define ENABLE_MQTT).
   NOTE: enabling MQTT could slightly decrease hashrate */
// #define ENABLE_MQTT
// Change this to specify MQTT server (ip only no prefixes)
const char *mqtt_server = "";
// Port mqtt server is listening at (default: 1883)
const int mqtt_port = 1883;

/* If you're using the ESP32-CAM board or other board
  that doesn't support OTA (Over-The-Air programming)
  comment the ENABLE_OTA definition line (#define ENABLE_OTA)
   NOTE: enabling OTA support could decrease hashrate (up to 40%) */
#define ENABLE_OTA

/* If you don't want to use the Serial interface comment
  the ENABLE_SERIAL definition line (#define ENABLE_SERIAL)*/
#define ENABLE_SERIAL
/****************** END OF MINER CONFIGURATION SECTION ******************/

#ifndef ENABLE_SERIAL
#define Serial DummySerial
static class {
  public:
    void begin(...) {}
    void print(...) {}
    void println(...) {}
    void printf(...) {}
} Serial;
#endif

#ifndef ENABLE_MQTT
#define PubSubClient DummyPubSubClient
class PubSubClient {
  public:
    PubSubClient(Client& client) {}
    bool connect(...) {
      return false;
    }
    bool connected(...) {
      return true;
    }
    void loop(...) {}
    void publish(...) {}
    void subscribe(...) {}
    void setServer(...) {}
};
#endif

// Data Structures
typedef struct TaskData
{
  TaskHandle_t handler;
  byte taskId;
  float hashrate;
  unsigned long shares;
  unsigned int difficulty;

} TaskData_t;

// Include required libraries
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <SPIFFSEditor.h>
#include <WebHandlerImpl.h>
#include <ESPAsyncWebServer.h>
#include <WebAuthentication.h>
#include <AsyncWebSynchronization.h>
#include <AsyncWebSocket.h>
#include <WebResponseImpl.h>
#include <StringArray.h>
#include <SPIFFS.h>

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_task_wdt.h>  //Include WDT libary
#include <DNSServer.h>
#include "home.h"

// ENABLE_LCD to enable screen output on the TTGO T-Display
#define ENABLE_LCD

#ifdef ENABLE_LCD
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
#define TFT_BLACK 0x0000
#include "duco_small.h"
#include "duco_xsmall.h"
#endif

#ifdef ENABLE_OTA
#include <ArduinoOTA.h>
#endif

#ifdef ENABLE_MQTT
#include <PubSubClient.h>
#endif

// Global Definitions
#define NUMBEROFCORES 2
#define MSGDELIMITER ','
#define MSGNEWLINE '\n'
#define BUTTON_PIN 35 // the number of the pushbutton pin
#define SHORT_PRESS_TIME 7500 // 7500 milliseconds

#define BUTTON_LCD 0

// Handles for additional threads
TaskHandle_t WiFirec;
TaskData_t TaskThreadData[NUMBEROFCORES];
TaskHandle_t MqttPublishHandle;
SemaphoreHandle_t xMutex;

// Internal Variables
const char *get_pool_api[] = {"https://server.duinocoin.com/getPool"};
String miner_version_str = "YoMino "+String(YOMINO_VERSION) +" (based on official ESP32 "+String(OFFICIAL_VERSION)+")";
const char *miner_version = miner_version_str.c_str();
String pool_name = "";
String host = "";
int port = 0;
int walletid = 0;
volatile int wifi_state = 0;
volatile int wifi_prev_state = WL_CONNECTED;
volatile bool ota_state = false;
volatile char chip_id[23];  // DUCO MCU ID
char rigname_auto[23]; // SPACE TO STORE RIG NAME
int mqttUpdateTrigger = 0;
String mqttRigTopic = "";
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool verticalLcd = false;

// Wifi and Mqtt Clients
WiFiClient wifiMqttClient;
PubSubClient mqttClient(wifiMqttClient);

bool isConfigured = false;
String ownWifi;
AsyncWebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;

const char* apSsid = "YoMino-";
const char* PARAM_SSID = "Wifi";
const char* PARAM_PASSWORD = "Password";
const char* PARAM_USERNAME = "Username";
const char* PARAM_RIG = "myRig";
const char* PARAM_DIFFICULTY = "Difficulty";
const char* PARAM_OWN_PASSWORD = "ownPassword";


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// Util Functions
void(* resetFunc) (void) = 0; //declare reset function @ address 0
void factoryReset() {
  #ifdef ENABLE_LCD
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);  
  tft.setCursor(0, 0, 2);
  tft.println("YoMino");
  tft.setSwapBytes(false);
  tft.pushImage(120, 0,  38, 38, duco_small);  
  tft.setTextSize(1);
  tft.setCursor(0, 70, 2);
  tft.println("Peforming");
  tft.setCursor(0, 80, 2);
  tft.println("factory reset...");
  #endif
  Serial.println("Performing factory reset...");
  SPIFFS.format();
  #ifdef ENABLE_LCD
  tft.setCursor(0, 60, 2);
  tft.println("Rebooting .    ");  
  tft.setCursor(0, 90, 2);
  tft.println("                 ");
  delay(3000);
  #endif
  resetFunc();  //call reset  
}

String getHome() {
    int wifiCount = 0;
    bool alreadyAdded = false;    
    int n = WiFi.scanNetworks(false, true);
    String wifiList[n];
    for (int i = 0; i<=n; i++) {
      if (WiFi.SSID(i).length()>0) {
        alreadyAdded = false;
        for (int o = 0; o<=wifiCount; o++) {
          if (wifiList[o] == WiFi.SSID(i)) {
            alreadyAdded = true;
          }
        }
        if (!alreadyAdded) {
          wifiList[wifiCount] = WiFi.SSID(i);
          wifiCount++;
        }        
      }
    }

    String wifiOptions;
    for (int l=0; l<wifiCount; l++) {
      wifiOptions += "<option value='"+ wifiList[l] + "'>" + wifiList[l] +"</option>";
    }
    
    String home = home_html;
    home.replace("$wifi_ssid$", wifiOptions);
    home.replace("$wifi_password$", wifi_password);
    home.replace("$username$", username);
    home.replace("$rig_identifier$", rig_identifier);
    home.replace("$ownPassword$", ownPassword);
    return home;
}

void writeStatus(String status) {
  tft.setTextSize(1);
  if (!verticalLcd) {
    tft.setTextSize(1);  
    tft.setCursor(0, 118, 2);
    tft.println(status);
  } else {
    tft.setCursor(0, 225, 2);
    tft.println(status);    
  }
}

String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while (file.available()) {
    fileContent += String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

void blink(uint8_t count, uint8_t pin = LED_BUILTIN) {
  uint8_t state = LOW;

  for (int x = 0; x < (count << 1); ++x) {
    digitalWrite(pin, state ^= HIGH);
    delay(75);
  }
}

String httpGetString(String URL) {
  String payload = "";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  if (http.begin(client, URL)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  return payload;
}

void UpdateHostPort(String input) {
  // Thanks @ricaun for the code
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  const char *name = doc["name"];
  const char *h = doc["ip"];
  int p = doc["port"];

  host = h;
  port = p;

  // Send to MQTT
  mqttClient.publish((mqttRigTopic + "pool_name").c_str(), name);
  mqttClient.publish((mqttRigTopic + "pool_ip").c_str(), h);
  mqttClient.publish((mqttRigTopic + "pool_port").c_str(), String(p).c_str());

  // Send to Serial
  Serial.println("Fetched pool: " + String(name) + " - " + String(host) + ":" + String(port));
}

String genString() {
  int generated = 0;
  String result;
  while (generated < 6)
  {
    byte randomValue = random(0, 26);
    char letter = randomValue + 'a';
    if (randomValue > 26)
      letter = (randomValue - 26) ;
    result += letter;
    generated ++;
  }
  return result;
}

// Communication Functions
void UpdatePool() {
  String input = "";
  int waitTime = 1;
  int poolIndex = 0;
  int poolSize = sizeof(get_pool_api) / sizeof(char*);

  while (input == "") {
    Serial.println("Fetching pool (" + String(get_pool_api[poolIndex]) + ")... ");
    input = httpGetString(get_pool_api[poolIndex]);
    poolIndex += 1;

    // Check if pool index needs to roll over
    if ( poolIndex >= poolSize ) {
      Serial.println("Retrying pool list in: " + String(waitTime) + "s");
      poolIndex %= poolSize;
      delay(waitTime * 1000);

      // Increase wait time till a maximum of 16 seconds (addresses: Limit connection requests on failure in ESP boards #1041)
      waitTime *= 2;
      if ( waitTime > 16 )
        waitTime = 16;
    }
  }

  // Setup pool with new input
  UpdateHostPort(input);
}

void HandleMqttConnection()
{
  // Check Connection
  if (!mqttClient.connected()) {
    // Setup MQTT Client
    Serial.println("Connecting to mqtt server: " + String(mqtt_server) + " on port: " + String(mqtt_port));
    mqttClient.setServer(mqtt_server, mqtt_port);

    // Setup Rig Topic
    mqttRigTopic = "duinocoin/" + String(rig_identifier) + "/";

    // Try to connect
    if (mqttClient.connect(rig_identifier, (mqttRigTopic + "state").c_str(), 0, true, String(0).c_str())) {
      // Connection Succesfull
      Serial.println("Succesfully connected to mqtt server");

      // Output connection info
      mqttClient.publish((mqttRigTopic + "ip").c_str(), WiFi.localIP().toString().c_str());
      mqttClient.publish((mqttRigTopic + "name").c_str(), String(rig_identifier).c_str());
    }
    else {
      // Connection Failed
      Serial.println("Failed to connect to mqtt server");
    }
  }

  // Default MQTT Loop
  mqttClient.loop();
}

void WiFireconnect(void *pvParameters) {
  int n = 0;
  unsigned long previousMillis = 0;
  const long interval = 500;
  esp_task_wdt_add(NULL);
  for (;;) {
    wifi_state = WiFi.status();

#ifdef ENABLE_OTA
    ArduinoOTA.handle();
#endif

    if (ota_state)  // If OTA is working, reset the watchdog
      esp_task_wdt_reset();

    // check if WiFi status has changed.
    if ((wifi_state == WL_CONNECTED) && (wifi_prev_state != WL_CONNECTED)) {
      esp_task_wdt_reset();  // Reset watchdog timer

      // Connect to MQTT (will do nothing if MQTT is disabled)
      HandleMqttConnection();

      // Write Data to Serial
      Serial.println(F("\nConnected to WiFi!"));
      Serial.println("    IP address: " + WiFi.localIP().toString());
      Serial.println("      Rig name: " + String(rig_identifier));
      Serial.println();

      // Notify Setup Complete
      blink(BLINK_SETUP_COMPLETE);// Sucessfull connection with wifi network

      // Update Pool and wait a bit
      UpdatePool();
      yield();
      delay(100);
    }

    else if ((wifi_state != WL_CONNECTED) &&
             (wifi_prev_state == WL_CONNECTED)) {
      esp_task_wdt_reset();  // Reset watchdog timer
      Serial.println(F("\nWiFi disconnected!"));
      WiFi.disconnect();

      Serial.println(F("Scanning for WiFi networks"));
      n = WiFi.scanNetworks(false, true);
      Serial.println(F("Scan done"));

      if (n == 0) {
        Serial.println(F("No networks found. Resetting ESP32."));
        blink(BLINK_RESET_DEVICE);
        esp_restart();
      }
      else {
        Serial.print(n);
        Serial.println(F(" networks found"));
        for (int i = 0; i < n; ++i) {
          // Print wifi_ssid and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(F(": "));
          Serial.print(WiFi.SSID(i));
          Serial.print(F(" ("));
          Serial.print(WiFi.RSSI(i));
          Serial.print(F(")"));
          Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " "
                         : "*");
          delay(10);
        }
      }

      esp_task_wdt_reset();  // Reset watchdog timer
      Serial.println();
      Serial.println(
        F("Please, check if your WiFi network is on the list and check if "
          "it's strong enough (greater than -90)."));
      Serial.println("ESP32 will reset itself after " + String(WDT_TIMEOUT) +
                     " seconds if can't connect to the network");

      Serial.print("Connecting to: " + wifi_ssid);
      WiFi.reconnect();
    }

    else if ((wifi_state == WL_CONNECTED) &&
             (wifi_prev_state == WL_CONNECTED)) {
      esp_task_wdt_reset();  // Reset watchdog timer
      delay(1000);
    }

    else {
      // Don't reset watchdog timer
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        Serial.print(F("."));
      }
    }
    wifi_prev_state = wifi_state;
  }
}

// Miner Code
void TaskMining(void *pvParameters) {
  // Setup Thread
  esp_task_wdt_add(NULL); // Disable watchdogtimer for this thread

  // Setup thread data
  String taskCoreName = "CORE" + String(xPortGetCoreID());
  int taskId = xPortGetCoreID();

  // Start main thread loop
  for ( ;; ) {
    // If OTA needs to be preformed reset the task watchdog
    if (ota_state)
      esp_task_wdt_reset();

    // Wait for a valid network connection
    while (wifi_state != WL_CONNECTED) {
      delay(1000);
      esp_task_wdt_reset();
    }
#ifdef ENABLE_LCD
    writeStatus("Waiting Pool         ");
#endif
    // Wait for server to get pool information
    while ( port == 0 ) {
      Serial.println(String("MinerThread on " + taskCoreName + " waiting for pool"));
      delay(1000);
      esp_task_wdt_reset();
    }

#ifdef ENABLE_LCD
    writeStatus("Connecting...      ");
#endif

    // Setup WiFi Client and connection details
    Serial.println(String("MinerThread on " + taskCoreName + " connecting to Duino-Coin server..."));
    WiFiClient jobClient;
    jobClient.setTimeout(1);
    jobClient.flush();
    yield();

    // Start connection to Duino-Coin server
    if (!jobClient.connect(host.c_str(), port)) {
      Serial.println(String("MinerThread on " + taskCoreName + " failed to connect"));
      delay(500);
      continue;
    }

    // Wait for server connection
    Serial.println(String("MinerThread on " + taskCoreName + " is connected"));
    while (!jobClient.available()) {
      yield();
      if (!jobClient.connected()) break;
      delay(10);
    }

#ifdef ENABLE_LCD
    writeStatus("Connected.        ");
#endif
    // Server sends SERVER_VERSION after connecting
    String SERVER_VER = jobClient.readString();
    Serial.println(String("MinerThread on " + taskCoreName + " received server version: " + SERVER_VER));
    blink(BLINK_CLIENT_CONNECT); // Sucessfull connection with the server

#ifdef ENABLE_LCD
    writeStatus("Server Ver: " + SERVER_VER);
#endif

    // Define job loop variables
    int jobClientBufferSize = 0;

    // Start Job loop
    while (jobClient.connected()) {
      // Reset watchdog timer before each job
      esp_task_wdt_reset();

#ifdef ENABLE_LCD
      writeStatus("Asking for work        ");
#endif

      // We are connected and are able to request a job
      Serial.println(String("MinerThread on " + taskCoreName + " asking for a new job for user: " + username));
      jobClient.flush();
      jobClient.print("JOB," + String(username) + ",ESP32");
      while (!jobClient.available()) {
        if (!jobClient.connected()) break;
        delay(10);
      }
      yield();

      // Check buffer size is larget than 10
      jobClientBufferSize = jobClient.available();
      if (jobClientBufferSize <= 10) {
        Serial.println(String("MinerThread on " + taskCoreName + " buffer size is: " + jobClientBufferSize + " (FAILED, requesting new job...)"));
        continue;
      }
      else {
        Serial.println(String("MinerThread on " + taskCoreName + " buffer size is: " + jobClientBufferSize + " (OK)"));
      }

      // Read hash, expected hash and difficulty from job description
      String previousHash = jobClient.readStringUntil(MSGDELIMITER);
      String expectedHash = jobClient.readStringUntil(MSGDELIMITER);
      TaskThreadData[taskId].difficulty = jobClient.readStringUntil(MSGNEWLINE).toInt() * 100;
      jobClient.flush();

      // Global Definitions
      unsigned int job_size_task_one = 100;
      unsigned char *expectedHashBytes = (unsigned char *)malloc(job_size_task_one * sizeof(unsigned char));

      // Clear expectedHashBytes
      memset(expectedHashBytes, 0, job_size_task_one);
      size_t expectedHashLength = expectedHash.length() / 2;

      // Convert expected hash to byte array (for easy comparison)
      const char *cExpectedHash = expectedHash.c_str();
      for (size_t i = 0, j = 0; j < expectedHashLength; i += 2, j++)
        expectedHashBytes[j] = (cExpectedHash[i] % 32 + 9) % 25 * 16 + (cExpectedHash [i + 1] % 32 + 9) % 25;

      // Start measurement
      unsigned long startTime = micros();
      byte shaResult[20];
      String hashUnderTest;
      unsigned int hashUnderTestLength;
      bool ignoreHashrate = false;

#ifdef ENABLE_LCD
      writeStatus("Crunching             ");
#endif

      // Try to find the nonce which creates the expected hash
      for (unsigned long nonceCalc = 0; nonceCalc <= TaskThreadData[taskId].difficulty; nonceCalc++) {
        // Define hash under Test
        hashUnderTest = previousHash + String(nonceCalc);
        hashUnderTestLength = hashUnderTest.length();

        // Wait for hash module lock
        while ( xSemaphoreTake(xMutex, portMAX_DELAY) != pdTRUE );

        // We are allowed to perform our hash
        esp_sha(SHA1, (const unsigned char *)hashUnderTest.c_str(), hashUnderTestLength, shaResult);

        // Release hash module lock
        xSemaphoreGive(xMutex);

        // Check if we have found the nonce for the expected hash
        if ( memcmp( shaResult, expectedHashBytes, sizeof(shaResult) ) == 0 ) {
          // Found the nonce submit it to the server
          Serial.println(String("MinerThread on " + taskCoreName + " found hash with nonce: " + nonceCalc ));


          // Calculate mining time
          float elapsedTime = (micros() - startTime) / 1000.0 / 1000.0; // Total elapsed time in seconds
          TaskThreadData[taskId].hashrate = nonceCalc / elapsedTime;

          // Validate connection
          if (!jobClient.connected()) {
            Serial.println(String("MinerThread on " + taskCoreName + " Lost connection. Trying to reconnect"));
            if (!jobClient.connect(host.c_str(), port)) {
              Serial.println(String("MinerThread on " + taskCoreName + " connection failed"));
              break;
            }
            Serial.println(String("MinerThread on " + taskCoreName + " Reconnection successful."));
          }

          // Send result to server
          jobClient.flush();
          jobClient.print(
            String(nonceCalc) + MSGDELIMITER + String(TaskThreadData[taskId].hashrate) + MSGDELIMITER +
            String(miner_version) + MSGDELIMITER + rig_identifier + MSGDELIMITER +
            "DUCOID" + String((char *)chip_id) + MSGDELIMITER + String(walletid));
          jobClient.flush();

          // Wait for job result
          while (!jobClient.available()) {
            if (!jobClient.connected()) {
              Serial.println(String("MinerThread on " + taskCoreName + " Lost connection. Didn't receive feedback."));
              break;
            }
            delay(10);
            yield();
          }
          delay(50);
          yield();

          // Handle feedback
          String feedback = jobClient.readStringUntil(MSGNEWLINE);
          jobClient.flush();
          TaskThreadData[taskId].shares++;
          blink(BLINK_SHARE_FOUND);

          // Validate Hashrate
          if ( TaskThreadData[taskId].hashrate < 4000 && !ignoreHashrate) {
            // Hashrate is low so restart esp
            Serial.println(String("MinerThread on " + taskCoreName + " Low hashrate (" + TaskThreadData[taskId].hashrate + "), Feedback: " + feedback + ". Restarting..."));
            jobClient.flush();
            jobClient.stop();
            blink(BLINK_RESET_DEVICE);
            esp_restart();
          }
          else {
            // Print statistics
            Serial.println(String("MinerThread on " + taskCoreName + " Job Feedback: " + feedback + ", Hashrate: " + TaskThreadData[taskId].hashrate + ", shares found: #" + TaskThreadData[taskId].shares));

            // Calculate combined hashrate and average difficulty
            float avgDiff = 0.0;
            float totHash = 0.0;
            unsigned long totShares = 0;
            for (int i = 0; i < NUMBEROFCORES; i++) {
              avgDiff += TaskThreadData[i].difficulty;
              totHash += TaskThreadData[i].hashrate;
              totShares += TaskThreadData[i].shares;
            }

#ifdef ENABLE_LCD
            if (!verticalLcd) {
              tft.setTextSize(2);  
              tft.setCursor(0, 45, 2);
              tft.println("Shares: " + String(totShares));
              tft.setCursor(0, 75, 2);
              tft.println("Speed: " + String(totHash / 1000, 2) + " KH/s");
              tft.setTextSize(1);
            } else {
              tft.setTextSize(2);  
              tft.setCursor(0, 65, 2);
              tft.println("Shares:");
              tft.setCursor(0, 95, 2);
              tft.println(String(totShares));
              tft.setCursor(0, 130, 2);
              tft.println("Speed:");  
              tft.setCursor(0, 160, 2);
              tft.println(String(totHash / 1000, 2) + "KHs");
              tft.setTextSize(1);  
            }
#endif
          }

          // Stop current loop and ask for a new job
          break;
        }
      }
    }

    Serial.println(String("MinerThread on " + taskCoreName + " Not connected. Restarting core"));
#ifdef ENABLE_LCD
    writeStatus(String(taskCoreName) + " restarting");
#endif

    jobClient.flush();
    jobClient.stop();
  }
}

void setupMinerThreads() {
  // Create Mining Threads
  for ( int i = 0; i < NUMBEROFCORES; i++ ) {
    Serial.println("Creating mining thread on core: " + String(i));
    xTaskCreatePinnedToCore(
      TaskMining, String("Task" + String(i)).c_str(), 10000, NULL, 2 + i, &TaskThreadData[NUMBEROFCORES].handler,
      i);  // create a task with priority 2 (+ core id) and executed on a specific core
    delay(250);
  }
}

void setupMining() {
  if (isConfigured) {
    char mySSID[64];
    wifi_ssid.toCharArray(mySSID, 64);
    char myPassword[64];
    wifi_password.toCharArray(myPassword, 64);

    //if - and only if the last character read in is a line-ending character
    // a "CR" carriage return or a "LF"  line feed

    btStop();
    Serial.println("Connecting with " + String(mySSID) + " - " + String(myPassword));
#ifdef ENABLE_LCD
    writeStatus("Connecting...        ");
#endif

    WiFi.begin(mySSID, myPassword);  // Connect to wifi

    uint64_t chipid = ESP.getEfuseMac();  // Getting chip chip_id
    uint16_t chip =
      (uint16_t)(chipid >> 32);  // Preparing for printing a 64 bit value (it's
    // actually 48 bits long) into a char array
    snprintf(
      (char *)chip_id, 23, "%04X%08X", chip,
      (uint32_t)chipid);  // Storing the 48 bit chip chip_id into a char array.
    walletid = random(0, 2811);

    // Autogenerate ID if required
    if ( strcmp(rig_identifier.c_str(), "Auto") == 0 ) {
      snprintf(rigname_auto, 23, "ESP32-%04X%08X", chip, (uint32_t)chipid);
      rig_identifier = &rigname_auto[0];
    }

    ota_state = false;

#ifdef ENABLE_OTA
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
      // using SPIFFS.end()
      Serial.println("Start updating " + type);
      ota_state = true;
    })
    .onEnd([]() {
      Serial.println(F("\nEnd"));
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      esp_task_wdt_reset();
      ota_state = true;
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR)
        Serial.println(F("Auth Failed"));
      else if (error == OTA_BEGIN_ERROR)
        Serial.println(F("Begin Failed"));
      else if (error == OTA_CONNECT_ERROR)
        Serial.println(F("Connect Failed"));
      else if (error == OTA_RECEIVE_ERROR)
        Serial.println(F("Receive Failed"));
      else if (error == OTA_END_ERROR)
        Serial.println(F("End Failed"));
      ota_state = false;
      blink(BLINK_RESET_DEVICE);
      esp_restart();
    });

    ArduinoOTA.setHostname(rig_identifier.c_str());
    ArduinoOTA.begin();
#endif

    esp_task_wdt_init(WDT_TIMEOUT, true);  // Init Watchdog timer
    pinMode(LED_BUILTIN, OUTPUT);

    // Determine which cores to use
    int wifiCore = 0;
    int mqttCore = 0;
    if ( NUMBEROFCORES >= 2 ) mqttCore = 1;

    // Create Semaphore and main Wifi Monitoring Thread
    xMutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(
      WiFireconnect, "WiFirec", 10000, NULL, NUMBEROFCORES + 2, &WiFirec,
      mqttCore);  // create a task with highest priority and executed on core 0
    delay(250);

    // If MQTT is enabled create a sending thread
#ifdef ENABLE_MQTT
    Serial.println("Creating mqtt thread on core: " + String(mqttCore));
    xTaskCreatePinnedToCore(
      MqttPublishCode, "MqttPublishCode", 10000, NULL, 1, &MqttPublishHandle,
      mqttCore); //create a task with lowest priority and executed on core 1
    delay(250);
#endif
    setupMinerThreads();
  }
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LCD, INPUT_PULLUP);
  Serial.begin(500000);  // Start serial connection
  Serial.println(String(miner_version));

  currentState = digitalRead(BUTTON_PIN);
  if(currentState == LOW) {        
      factoryReset();
  }

 
  bool initSpiffsOk = false;
  initSpiffsOk = SPIFFS.begin();
  if (!initSpiffsOk) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    Serial.println("Trying to format it");
    SPIFFS.format();
    initSpiffsOk = SPIFFS.begin();
    if (!initSpiffsOk) {
      Serial.println("Really no way to start SPIFFS. Looping doing nothing");
      while (1) yield(); // Stay here twiddling thumbs waiting          
    }
  }

  String verticalLcdSetting = readFile(SPIFFS, "/vertical_lcd.txt");
  if (verticalLcdSetting.length()>0 && verticalLcdSetting == "VERTICAL") {
    verticalLcd = true;
  } else { 
    verticalLcd = false;
  }
  if (verticalLcdSetting.length() == 0) {
     writeFile(SPIFFS, "/vertical_lcd.txt", "HORIZONTAL");    
  }

  int verticalLcdButton = digitalRead(BUTTON_LCD);
  if (verticalLcdButton == LOW) {
    Serial.println("Button Pressed");
    verticalLcd = !verticalLcd;
    if (verticalLcd) {
      writeFile(SPIFFS, "/vertical_lcd.txt", "VERTICAL");
    } else {
      writeFile(SPIFFS, "/vertical_lcd.txt", "HORIZONTAL");
    }    
  }

  
  randomSeed(analogRead(0));
  ownWifi = readFile(SPIFFS, "/own_wifi.txt");
  if (ownWifi.length() <= 0) {
    ownWifi = apSsid + genString();
    writeFile(SPIFFS, "/own_wifi.txt", ownWifi.c_str());
  }
  IPAddress apIP(192, 168, 72, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  ownPassword = readFile(SPIFFS, "/own_password.txt");
  if (ownPassword.length() <= 0) {
    WiFi.softAP(ownWifi.c_str());
  } else {
    WiFi.softAP(ownWifi.c_str(), ownPassword.c_str());
  }

  dnsServer.start(DNS_PORT, "*", apIP);
  server.begin();

  wifi_ssid = readFile(SPIFFS, "/wifi_ssid.txt");
  wifi_password = readFile(SPIFFS, "/wifi_password.txt");
  username = readFile(SPIFFS, "/username.txt");
  rig_identifier = readFile(SPIFFS, "/rig.txt");


  if (wifi_ssid.length() > 0 && wifi_password.length() > 0 && username.length() > 0 && rig_identifier.length() > 0) {
    isConfigured = true;
    setupMining();
  }

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String home = getHome();
    request->send_P(200, "text/html", home.c_str());
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {

    if (request->hasParam(PARAM_SSID)) {
      wifi_ssid = request->getParam(PARAM_SSID)->value();
      if (wifi_ssid.length() > 0) {
        writeFile(SPIFFS, "/wifi_ssid.txt", wifi_ssid.c_str());
        Serial.println("Wifi SSID set to: " + wifi_ssid);
      }
    }
    if (request->hasParam(PARAM_PASSWORD)) {
      wifi_password = request->getParam(PARAM_PASSWORD)->value();
      if (wifi_password.length() > 0) {
        writeFile(SPIFFS, "/wifi_password.txt", wifi_password.c_str());
        Serial.println("Wifi password set to: " + wifi_password);
      }
    }

    if (request->hasParam(PARAM_USERNAME)) {
      username = request->getParam(PARAM_USERNAME)->value();
      if (username.length() > 0) {
        writeFile(SPIFFS, "/username.txt", username.c_str());
        Serial.println("Username set to: " + username);
      }
    }

    if (request->hasParam(PARAM_RIG)) {
      rig_identifier = request->getParam(PARAM_RIG)->value();
      if (rig_identifier.length() > 0) {
        writeFile(SPIFFS, "/rig.txt", rig_identifier.c_str());
        Serial.println("Rig set to: " + rig_identifier);
      }
    }

    if (request->hasParam(PARAM_OWN_PASSWORD)) {
      ownPassword = request->getParam(PARAM_OWN_PASSWORD)->value();
      if (ownPassword.length() > 0) {
        writeFile(SPIFFS, "/own_password.txt", ownPassword.c_str());
        Serial.println("Own password set to: " + ownPassword);
      }
    }

    if (wifi_ssid.length() <= 0 || wifi_password.length() <= 0 || username.length() <= 0 || rig_identifier.length() <= 0 || ownPassword.length() <= 0 )
    {
      Serial.println("Setup incomplete");
      request->send(200, "ERROR");
    } else {
      Serial.println("Setup Done, starting to mine");
      request->send(200, "OK");
      isConfigured = true;
      setupMining();
    }
  });
  server.onNotFound(notFound);

#ifdef ENABLE_LCD
  tft.init();
  if (!verticalLcd || !isConfigured) {      
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);
    tft.pushImage(1, 0,  38, 38, duco_small);      
    tft.setCursor(50, 5, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("YoMino");
    if (!isConfigured) {
      tft.setCursor(0, 60, 2);
      tft.println(ownWifi);
      tft.setCursor(0, 90, 2);
      tft.println(apIP);
    }
    tft.setTextSize(1);
  } else {
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);
    tft.pushImage(1, 10,  19, 19, duco_xsmall);      
    tft.setCursor(33, 4, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.println("YoMino");    
  }
#endif
}

// ************************************************************
void MqttPublishCode( void * pvParameters ) {
  unsigned long lastWdtReset = 0;
  unsigned long wdtResetDelay = 30000;

  for (;;) {
    if ((millis() - lastWdtReset) > wdtResetDelay) {
      // Reset timers
      esp_task_wdt_reset();
      lastWdtReset = millis();

      // Calculate combined hashrate and average difficulty
      float avgDiff = 0.0;
      float totHash = 0.0;
      unsigned long totShares = 0;
      for (int i = 0; i < NUMBEROFCORES; i++) {
        avgDiff += TaskThreadData[i].difficulty;
        totHash += TaskThreadData[i].hashrate;
        totShares += TaskThreadData[i].shares;
      }
      avgDiff /= NUMBEROFCORES;

      // Update Stats
      mqttClient.publish((mqttRigTopic + "state").c_str(), String(1).c_str());
      mqttClient.publish((mqttRigTopic + "hashrate").c_str(), String(totHash).c_str());
      mqttClient.publish((mqttRigTopic + "avgdiff").c_str(), String(avgDiff).c_str());
      mqttClient.publish((mqttRigTopic + "shares").c_str(), String(totShares).c_str());
    }

    mqttClient.loop();
    yield();
  }
}

void loop() {
  vTaskDelete(NULL); /* Free up The CPU */
  currentState = digitalRead(BUTTON_PIN);
  if(lastState == HIGH && currentState == LOW)        // button is pressed
    pressedTime = millis();
  else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_PRESS_TIME )
      factoryReset();
    }

  // save the the last state
  lastState = currentState;

  dnsServer.processNextRequest();
}


/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "ERR_NOT_FOUND_404"
#define WIFI_PASSWORD "revyn1029384756"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyA2pjQuptJ3mUFR24bbLR15SaFv7qIgTR0"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://fir-71190523-default-rtdb.asia-southeast1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "revynpradana76@gmail.com"
#define USER_PASSWORD ""

//Define Servo
#include <ESP32Servo.h>

//Define Firebase Data object
FirebaseData fbdo;
Servo servo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

#define touchPin 4
#define servoPin 14

String path = "/Node1";
// variables will change:
int angle = 0;         // the current angle of servo motor
int lastTouchState;    // the previous state of touch sensor
int currentTouchState; // the current state of touch sensor

void setup()
{

  Serial.begin(115200);
  
  pinMode(touchPin, INPUT);
  servo.attach(servoPin);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  //config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = "X5kjRFgPLIIkXTIp0UVbTVYwAwwEI4nhJEKOXoyQ";

  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  /** Timeout options.

  //WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
  config.timeout.wifiReconnect = 10 * 1000;

  //Socket connection and SSL handshake timeout in ms (1 sec - 1 min).
  config.timeout.socketConnection = 10 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  Note:
  The function that starting the new TCP session i.e. first time server connection or previous session was closed, the function won't exit until the 
  time of config.timeout.socketConnection.

  You can also set the TCP data sending retry with
  config.tcp_data_sending_retry = 1;

  */
}

void loop()
{
  //Flash string (PROGMEM and  (FPSTR), String, C/C++ string, const char, char array, string literal are supported
  //in all Firebase and FirebaseJson functions, unless F() macro is not supported.

  lastTouchState    = currentTouchState;             // save the last state
  currentTouchState = touchRead(touchPin); // read new state
  
  if(lastTouchState == LOW && currentTouchState == HIGH) {
    Firebase.setInt(fbdo, path + "/touch", 1);
    Serial.println("Touch State: 1");

    // change angle of servo motor
    if(angle == 0){
      angle = 90;
      Firebase.setInt(fbdo, path + "/angle", 90);
      Serial.println("Angle: 90 degree");
    }else if(angle == 90){
      angle = 0;
      Firebase.setInt(fbdo, path + "/angle", 0);
      Serial.println("Angle: 0 degree");
    }

    // control servo motor arccoding to the angle
    servo.write(angle);
  }else if(lastTouchState == HIGH && currentTouchState == LOW){
    Firebase.setInt(fbdo, path + "/touch", 0);
    Serial.println("Touch State: 0");
  }
}

/// PLEASE AVOID THIS ////

//Please avoid the following inappropriate and inefficient use cases
/**
 * 
 * 1. Call get repeatedly inside the loop without the appropriate timing for execution provided e.g. millis() or conditional checking,
 * where delay should be avoided.
 * 
 * Everytime get was called, the request header need to be sent to server which its size depends on the authentication method used, 
 * and costs your data usage.
 * 
 * Please use stream function instead for this use case.
 * 
 * 2. Using the single FirebaseData object to call different type functions as above example without the appropriate 
 * timing for execution provided in the loop i.e., repeatedly switching call between get and set functions.
 * 
 * In addition to costs the data usage, the delay will be involved as the session needs to be closed and opened too often
 * due to the HTTP method (GET, PUT, POST, PATCH and DELETE) was changed in the incoming request. 
 * 
 * 
 * Please reduce the use of swithing calls by store the multiple values to the JSON object and store it once on the database.
 * 
 * Or calling continuously "set" or "setAsync" functions without "get" called in between, and calling get continuously without set 
 * called in between.
 * 
 * If you needed to call arbitrary "get" and "set" based on condition or event, use another FirebaseData object to avoid the session 
 * closing and reopening.
 * 
 * 3. Use of delay or hidden delay or blocking operation to wait for hardware ready in the third party sensor libraries, together with stream functions e.g. Firebase.RTDB.readStream and fbdo.streamAvailable in the loop.
 * 
 * Please use non-blocking mode of sensor libraries (if available) or use millis instead of delay in your code.
 * 
 * 4. Blocking the token generation process.
 * 
 * Let the authentication token generation to run without blocking, the following code MUST BE AVOIDED.
 * 
 * while (!Firebase.ready()) <---- Don't do this in while loop
 * {
 *     delay(1000);
 * }
 * 
 */

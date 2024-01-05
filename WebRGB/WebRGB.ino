#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <time.h>
#define FASTLED_ALLOW_INTERRUPTS 0  
#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    29
#define MIN_BRIGHTNESS 10
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS  150
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define BLINK_DELAY 100
#define BLINK_ITERATIONS 10
#define BREATH_DELAY 10

#define ALARM_DURATION 150

CRGB leds[NUM_LEDS];
 
#define UPDATES_PER_SECOND 100

bool state = false;

// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Set web server port number to 80
ESP8266WebServer server(80);

String alarmTime = "00:00";
void (*animationFunction)() = &fixed;
CRGB ledColor;

void setTimezone(String timezone){
  Serial.printf("  Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone) {
  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");  // First connect to NTP server, with 0 TZ offset

  // Wait for time to be set
  time_t now;
  while (true) {
    time(&now);
    if (now > 8 * 3600 * 2) {
      break;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  Serial.println("Got the time from NTP");

  // Now we can set the real timezone
  setTimezone(timezone);
}

void getCurrentTime(char buffer[50]) {
  time_t now;
  struct tm timeinfo;

  // Wait for time to be set
  while (true) {
    time(&now);
    if (now > 8 * 3600 * 2) {
      break;
    }
    delay(500);
    Serial.print(".");
  }
  localtime_r(&now, &timeinfo);

  strftime(buffer, 50, "%H:%M", &timeinfo);
}

void printLocalTime(){
  char buffer[50];
  getCurrentTime(buffer);
  Serial.println(buffer);
}

void handleRoot() {
  String html = "<html lang='fr'>\
	<head>\
		<title>WebRGB Alarm</title>\
		<style>\
			html, body{\
				max-width: 100%;\
				max-height: 100%;\
				background-color: #2e3440;\
				font-family: 'Didact Gothic';\
			}\
			\#title{\
				color: #8fbcbb;\
				font-size: 2em;\
				text-align: center;\
			}\
			\#subtitle {\
				color: #88c0d0;\
				font-size: 1.8em;\
				text-align: center;\
			}\
			.form-container {\
				display: flex;\
				flex-direction: column;\
				justify-content: space-around;\
				height: 80%;\
				width: 100%;\
				padding-left: 20px;\
				padding-right: 20px;\
			}\
      .form-container-2 {\
				display: flex;\
				flex-direction: column;\
        height: 20%;\
				width: 100%;\
				justify-content: center;\
				padding-left: 20px;\
				padding-right: 20px;\
			}\
			.time-selector, .animation-selector{\
				display: flex;\
				flex-direction: row;\
				justify-content: space-around;\
			}\
			label {\
				color: #81a1c1;\
				font-size: 1.5em;\
				margin-right: 30px;\
			}\
			.submit-btn {\
				width: 180px;\
				background-color: #d08770;\
				height: 50px;\
				margin-left: auto;\
				margin-right: auto;\
				border-color: #bf616a;\
				font-size: 1.5em;\
				color: #4c566a;\
			}\
			select {\
				font-size: 1.5em;\
				padding: 10px;\
				background-color: #88c0d0;\
				color: #4c566a;\
				border-color: #81a1c1;\
			}\
      .color-control > select {\
        padding: 30px;\
      }\
		</style>\
	</head>\
<body>\
	<h1 id='title'>NodeMCU WebRGB Alarm</h1>\
	<h2 id='subtitle'>Select your preferred hour to wake up, and the type of animation you like:</h2>\
	<form action='/submit' method='post'>\
		<div class='form-container'>\
			<div class='time-selector'>\
				<div class='hour-option'>\
					<label for='hours'>Hour:</label>\
					<select id='hours' name='hours'>\
						<option value='00'>00</option>\
						<option value='01'>01</option>\
						<option value='02'>02</option>\
						<option value='03'>03</option>\
						<option value='04'>04</option>\
						<option value='05'>05</option>\
						<option value='06'>06</option>\
						<option value='07'>07</option>\
						<option value='08'>08</option>\
						<option value='09'>09</option>\
						<option value='10'>10</option>\
						<option value='11'>11</option>\
						<option value='12'>12</option>\
						<option value='13'>13</option>\
						<option value='14'>14</option>\
						<option value='15'>15</option>\
						<option value='16'>16</option>\
						<option value='17'>17</option>\
						<option value='18'>18</option>\
						<option value='19'>19</option>\
						<option value='20'>20</option>\
						<option value='21'>21</option>\
						<option value='22'>22</option>\
						<option value='23'>23</option>\
					</select>\
				</div>\
				<div class='minute-option'>\
					<label for='minutes'>Minute:</label>\
					<select id='minutes' name='minutes'>\
						<option value='00'>00</option>\
						<option value='01'>01</option>\
						<option value='02'>02</option>\
						<option value='03'>03</option>\
						<option value='04'>04</option>\
						<option value='05'>05</option>\
						<option value='06'>06</option>\
						<option value='07'>07</option>\
						<option value='08'>08</option>\
						<option value='09'>09</option>\
						<option value='10'>10</option>\
						<option value='11'>11</option>\
						<option value='12'>12</option>\
						<option value='13'>13</option>\
						<option value='14'>14</option>\
						<option value='15'>15</option>\
						<option value='16'>16</option>\
						<option value='17'>17</option>\
						<option value='18'>18</option>\
						<option value='19'>19</option>\
						<option value='20'>20</option>\
						<option value='21'>21</option>\
						<option value='22'>22</option>\
						<option value='23'>23</option>\
						<option value='24'>24</option>\
						<option value='25'>25</option>\
						<option value='26'>26</option>\
						<option value='27'>27</option>\
						<option value='28'>28</option>\
						<option value='29'>29</option>\
						<option value='30'>30</option>\
						<option value='31'>31</option>\
						<option value='32'>32</option>\
						<option value='33'>33</option>\
						<option value='34'>34</option>\
						<option value='35'>35</option>\
						<option value='36'>36</option>\
						<option value='37'>37</option>\
						<option value='38'>38</option>\
						<option value='39'>39</option>\
						<option value='40'>40</option>\
						<option value='41'>41</option>\
						<option value='42'>42</option>\
						<option value='43'>43</option>\
						<option value='44'>44</option>\
						<option value='45'>45</option>\
						<option value='46'>46</option>\
						<option value='47'>47</option>\
						<option value='48'>48</option>\
						<option value='49'>49</option>\
						<option value='50'>50</option>\
						<option value='51'>51</option>\
						<option value='52'>52</option>\
						<option value='53'>53</option>\
						<option value='54'>54</option>\
						<option value='55'>55</option>\
						<option value='56'>56</option>\
						<option value='57'>57</option>\
						<option value='58'>58</option>\
						<option value='59'>59</option>\
					</select>\
				</div>\
			</div>\
			<div class='animation-selector'>\
				<div class='animation-option'>\
					<label for='animation'>Animation:</label>\
					<select id='animation' name='animation'>\
						<option value='blink'>Blink</option>\
						<option value='breath'>Breath</option>\
						<option value='fixed'>Fixed</option>\
					</select>\
				</div>\
				<div class='color-option'>\
					<label for='color'>Color:</label>\
					<select class='color' name='color'>\
						<option value='red'>Red</option>\
						<option value='green'>Green</option>\
						<option value='blue'>Blue</option>\
						<option value='orange'>Orange</option>\
						<option value='white'>White</option>\
						<option value='yellow'>Yellow</option>\
					</select>\
				</div>\
			</div>\
			<input class='submit-btn' type='submit' value='Set Alarm'>\
		</div>\
	</form>\
  <h2 class='subtitle'>\
  <form action='/control' method='post'>\
    <div class='form-container-2'>\
      <div class='color-option-2'>\
				<label for='color-control'>Color:</label>\
				<select id='color-control' name='color-control'>\
					<option value='red'>Red</option>\
					<option value='green'>Green</option>\
					<option value='blue'>Blue</option>\
					<option value='orange'>Orange</option>\
					<option value='white'>White</option>\
					<option value='yellow'>Yellow</option>\
				</select>\
        <label for='brightness'>brightness</label>\
        <input type='range' min='1' max='255' value='127' class='slider' id='brightness' name='brightness'>\
			</div>\
      <input class='submit-btn' type='submit' value='Switch ON/OFF'>\
    </div>\
  </form>\
</body>\
</html>";
  server.send(200, "text/html", html);
}

void blink()
{
  Serial.println("Blink animation...");
  for (int j = 0; j < BLINK_ITERATIONS; j++)
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColor;
    }
    delay(5);
    FastLED.show();
    delay(BLINK_DELAY);
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::Black;
    }
    delay(5);
    FastLED.show();
    delay(BLINK_DELAY);
  }
}

void breath()
{
  Serial.println("Breath animation...");
  for (int ledBrightness = MIN_BRIGHTNESS; ledBrightness < MAX_BRIGHTNESS; ledBrightness++)
  {
    FastLED.setBrightness(ledBrightness);
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColor;
    }
    FastLED.show();
    delay(BREATH_DELAY);
  }
  for (int ledBrightness = MAX_BRIGHTNESS - 1; ledBrightness >= MIN_BRIGHTNESS; ledBrightness--)
  {
    FastLED.setBrightness(ledBrightness);
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColor;
    }
    FastLED.show();
    delay(BREATH_DELAY);
  }
}

void fixed()
{
  Serial.println("Fixed animation...");
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ledColor;
  }
  FastLED.show();
  delay(BREATH_DELAY * 200);
}

void handleSubmit() {
  String selectedTime = server.arg("hours") + ":" + server.arg("minutes");
  String animation = server.arg("animation");
  String color = server.arg("color");

  if (color == "red") {
    Serial.println("Color is red");
    ledColor = CRGB::Red;
  } else if (color == "green") {
    Serial.println("Color is green");
    ledColor = CRGB::Green;
  } else if (color == "blue") {
    Serial.println("Color is blue");
    ledColor = CRGB::Blue;
  } else if (color == "yellow") {
    Serial.println("Color is yellow");
    ledColor = CRGB::Yellow;
  } else if (color == "orange") {
    Serial.println("Color is orange");
    ledColor = CRGB::OrangeRed;
  } else if (color == "white") {
    Serial.println("Color is white");
    ledColor = CRGB::White;
  } else {
    Serial.println("No color");
  }

  if (animation == "blink") {
    Serial.println("Animation is blink");
    animationFunction = &blink;
  } else if (animation == "breath") {
    Serial.println("Animation is breath");
    animationFunction = &breath;
  } else if (animation == "fixed") {
    Serial.println("Animation is fixed");
    animationFunction = &fixed;
  } else {
    Serial.println("No animation");
  }
  alarmTime = selectedTime;
  Serial.println(selectedTime);
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void handleControl()
{
  String color = server.arg("color-control");
  int   brightness = atoi(server.arg("brightness").c_str());

  if (state)
    Serial.println("light is on, turning off");
  else
    Serial.println("light is off, turning on");
  Serial.println("Brightness : " + server.arg("brightness"));
  if (color == "red") {
    Serial.println("Color is red");
    ledColor = CRGB::Red;
  } else if (color == "green") {
    Serial.println("Color is green");
    ledColor = CRGB::Green;
  } else if (color == "blue") {
    Serial.println("Color is blue");
    ledColor = CRGB::Blue;
  } else if (color == "yellow") {
    Serial.println("Color is yellow");
    ledColor = CRGB::Yellow;
  } else if (color == "orange") {
    Serial.println("Color is orange");
    ledColor = CRGB::OrangeRed;
  } else if (color == "white") {
    Serial.println("Color is white");
    ledColor = CRGB::White;
  } else {
    Serial.println("No color");
  }
  FastLED.setBrightness(brightness);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (!state)
      leds[i] = ledColor;
    else
      leds[i] = CRGB::Black;
  }
  FastLED.show();
  state = !state;
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  initTime("CET-1CEST,M3.5.0,M10.5.0/3");
  printLocalTime();
  // Define server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.on("/control", HTTP_POST, handleControl);

  // Start server
  server.begin();
  Serial.println("HTTP server started");

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
}


void loop() {
  char currentTime[50];
  server.handleClient();
  getCurrentTime(currentTime);
  if (strcmp(currentTime, alarmTime.c_str()) == 0)
  {
    FastLED.setBrightness(BRIGHTNESS);
    for (int i = 0; i < ALARM_DURATION; i++)
    {
      animationFunction();
    }
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::Black;
    }
    FastLED.show();
  }
}
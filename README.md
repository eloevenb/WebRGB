Basic RGB led strip program to control with a esp8266 (nodemcu). You need to use the esp8266 core version 2.7.4 or it wont work. 
The RGB led strip data pin is on d5 but you can change it in the code. My strip is a ws2812b (5v), if yours is 12/24v, you will have to use a transistor (mosfet).
You can change the number of LEDS on your strip in the code, and the duration of the animations.

# YoMino

YoMino is a fork of the original and official ESP32 miner with a couple twists. Making use of ESP32's SoftAP capabilities, you can flash your ESP32 board and give it to someone who wouldn't have a clue about how to flash it. Power it up, either look for a new Wifi SSID named something like "YoMino-xxxxx" or simply read it from the board display if it has one. Once connected to the miner Wifi AP, go to http://192.168.72.1 and enter your miner configuration. Entering an YoMino AP password ensures only you can connect to the Miner's Wifi once you went through the initial setup. Settings are saved via SPIFFS.

The board I used has two buttons. One connected to GPIO0 and the other to GPIO35. Button connected to GPI0 if pressed at boot and you have a screen, changes display orientation. Button connected to GIO35 if pressed at boot performs a factory reset. You can reconfigure it.

It should work on any ESP32 board but the one I developed most of the added features is a TTGO T-Display which comes with an LCD screen. Setup info or mining stats are displayed on it.


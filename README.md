## HUE button - esp8266

Philips HUE works quite well, but lacks some features. This can be overcome
by adding some extra functionality to the button. For example have the button
react differently during a specific time of the day. During nighttime, I won't
have the light in the passage as bright as during the day. 

The HUE API currently also lacks the "switch" functionality. When pushing the
button, I don't need it to turn on or off, but I want it to change it's current
state. This program tries to fix this limitation (by checking the state, before
either switching the light on or off, based on that state). 

The code uses the https://platform.io/ framework. 

## Installation

Clone the repository first, then initialize the submodule(s):

    git clone https://github.com/steviethecat/huebutton-esp8266.git
    cd huebutton-eps8266
    git submodule init
    git submodule update 

Then, open the directory in atom and use <tt>PlatformIO: Initialize or Update PlatformIO Project</tt>. 

Select the correct board (<tt>Espressif Generic ESP8266 ESP-01 1M</tt>) and 
wait a while, depending on how much you need to download. 
    

### Troubleshooting

* <tt>fatal error: ArduinoJson.h: No such file or directory</tt>

Solution:

<pre>
pio lib install ArduinoJson
</pre>

## Hardware

I've tested the software on an esp8266-01, with the button connected between
GPIO0 and ground. 


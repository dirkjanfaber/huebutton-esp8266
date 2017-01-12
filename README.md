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
    git submodule init
    git submodule pull 
    

## Hardware

I've tested the software on an esp8266-01, with the button connected between
GPIO0 and ground. 


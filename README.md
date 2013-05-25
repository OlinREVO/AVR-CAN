AVR-CAN
======

CAN bus message handler for the ATmega16M1.

Demo v1: can.c
Sends a single message across the CAN bus to toggle an LED on and off.

Demo v2: client.c, server.c
This is a demonstration of two ATmega16M1s communicating across a CAN bus. The demo uses CAN messages to turn a remote LED on or off based on the state of a momentary button.

Two ATmegas are in use in the demo. The "server" (the lower chip in the video) has a button connected to one of its pins. It fires an external interrupt whenever the input from the button changes and sends the appropriate message across the CAN bus. When the input is 1 (5V), it sends 0xFF; when the input is 0 (0V), it sends 0x00. 

The client ATmega on the other side of the CAN bus listens for messages constantly and changes the state of the LED accordingly when it receives the message. It turns the LED on when it receives 0xFF and turns it off for 0x00.

You can see that whenever the button is pressed, the LED is on; otherwise, the LED is off.


To flash code onto an ATmega chip, run the following commands:
* `make FILE=can flash` for demo v1
* `make FILE=client flash` for demo v2's client
* `make FILE=server flash` for demo v2's server

You may need to append `sudo` to these commands based on your system setup.

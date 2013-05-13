AVR-CAN
======

CAN bus message handler for the ATmega16M1.

Demo v1: can.c
Sends a single message across the CAN bus to toggle an LED on and off.

Demo v2: client.c, server.c
Uses CAN to turn a remote LED on or off based on the state of a momentary button connected to a pin.

To flash code onto an ATmega chip, run the following commands:
* `make FILE=can flash` for demo v1
* `make FILE=client flash` for demo v2's client
* `make FILE=server flash` for demo v2's server

You may need to append `sudo` to these commands based on your system setup.

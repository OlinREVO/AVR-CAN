AVR-CAN
======

CAN bus message handler for the ATmega16M1

About the CAN API
-----
Files: api.h, api.c

The goal of this CAN API is to abstract the inner mechanisms of the CAN bus away from each individual sensor node. Nodes do not have to interact with the registers of the CAN controller, they need only use the available API methods.

Each node implementing this API has access to 2 methods (initCan and sendCANmsg) and must implement 1 method of its own in order to use the API (handleCANmsg). Please see the demo in this project for an example of these methods in use.

Using this API in your project
-----
In order to use this API, you need to implement handleCANmsg, even if you do not intend to send any messages to a given chip. Make sure to include the following in your Makefile: 
SOURCES += api.c
INC += -Isrc/
CFLAGS += $(INC)

Adding this API to your project as a subtree (subrepository inside of your project)
-----
Git subtree is a tool to embed a repository inside of another repository. This is desirable because we want to be able to embed AVR-CAN inside of all of REVO's other repositories. Follow the instructions below to add or update this repo as a subtree.

#### To add AVR-CAN as a subtree:
1. From the top level of your repository, run the following command: `git subtree add --prefix=src/AVR-CAN git@github.com:OlinREVO/AVR-CAN.git master --squash`
2. Find the line in your Makefile that starts with CFLAGS. Add "-Isrc/AVR-CAN/src" to the end of it. This allows gcc to find api.h.
3. Add src/AVR-CAN/src to your list of sources. You may be able to do that using a line such as "SOURCES += src/AVR-CAN/src", but this will vary based on your Makefile.
4. You will need to commit and push these changes to make them visible to others.

#### To update AVR-CAN from the master repository:
1. From the top level of your repository, run the following command: `git subtree pull --prefix=src/AVR-CAN git@github.com:OlinREVO/AVR-CAN.git master --squash`
2. You will need to commit and push these changes to make them visible to others.

Demo Code
-----
Files: client.c, server.c

This is a demonstration of two ATmega16M1s communicating across a CAN bus. The demo uses CAN messages to turn a remote LED on or off based on the state of a momentary button.

The "server" chip sends the state of the button (0x00 or 0xFF) across the CAN bus to the client, which turns an LED on or off in turn.

Making and flashing the demo
-----
To flash code onto an ATmega chip, run the following commands:
* `make FILE=client flash` for the client
* `make FILE=server flash` for the server

You may need to append `sudo` to these commands based on your system setup.

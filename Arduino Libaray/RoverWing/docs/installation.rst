.. _installation:

===============================
Installation And Initialization
===============================

Hardware
--------

This guide assumes that you have a Feather board plugged into the RoverWing,
and have powered the RoverWing using a 7-14 V power supply as described in
|guide| (note that RoverWing can not be powered through the USB port).
It also assumes that you have connected all peripherals you intend to use
(motors, servos, sonars,...) to the RoverWing board.

To upload your programs, connect the Feather board to your computer by a USB cable.

.. warning::
   Do not use the USB port on the RoverWing to upload your program. This port
   should only be used for updating the firmware on the RoverWing.

Library Installation
--------------------

To use RoverWing, you need Arduino IDE (version 1.6.2 or later) with the
support package for your Feather board; please see your Feather board
documentation for instructions on installing the hardware support package.

In addition to that, you will need to install the RoverWing library. The easiest way
to do that is by using Arduino Library Manager, by selecting from the menu
:menuselection:`Tools-->Manage Libraries...` and entering `roverwing` in the
search field.

After installation,  verify that library was successfully
installed by opening in the main menu :menuselection:`File-->Examples`; scroll all
the way down to section :guilabel:`Examples from custom libraries` and you
should see :guilabel:`RoverWing` there.

Basic Usage
-----------
The code below shows basic usage of RoverWing library::

  #include <Wire.h>
  #include <RoverWing.h>

  Rover myRover; //this is the name of the rover!

  void setup(){
    Wire.begin();
    Wire.setClock(400000); //use fast mode (400 kHz)
    delay(500); //wait for 0.5 second, so that roverwing initializes
    myRover.begin();
  }


Class Rover is defined in :file:`RoverWing.h`; this class describes the
RoverWing board. To access the RoverWing board, you need to create an object of
type Rover and then initialize it using :func:`begin()` function. You can use
any name you like, but you can only have one object of class Rover in your sketch.

Since the Feather board communicates with the RoverWing using I2C protocol, you
need to call :code:`Wire.begin()` before initializing the RoverWing. It is advised that
you use fast mode (400 kHz) I2C; if your feather board doesn't support it, or
you have trouble connecting, comment :code:`Wire.setClock()` line to use default speed
(100 kHz).

To test your setup, open the serial monitor (at 9600 bps) and run
:menuselection:`ConnectionTest` example (from
:menuselection:`File-->Examples-->RoverWing menu`).

Examples
--------

The RoverWing library comes with a number of example sketches; as always, they
are available from :menuselection:`File-->Examples-->RoverWing` menu of Arduino
IDE. These example sketches are amply commented, making it easy to adjust the
code for your purposes.

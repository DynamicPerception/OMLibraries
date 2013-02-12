OpenMoCo Libraries for AVR
=========================

Motion Control, User Interface, and MoCoBus Libraries for the Arduino Platform
------------------------------------------------------------------------------

This set of libraries provides several key components to aid in easily and quickly developing motion control applications using the nanoMoCo controller or for other Arduino-based devices which wish to use the MoCoBus protocol for communicating between devices.

Complete Doxygen documentation including tutorials and examples can be found in Docs/html/index.html

Key Libraries
-------------

### OMState

A simple, effective state engine for Arduino sketches.  A light-weight library for the quick and easy mapping of states to transitions via callbacks.  Simplifies core loop logic in sketches.

### OMCamera

The OMCamera class provides easy, non-blocking control of a camera connected to a nanoMoCo or other Arduino device via opto-couplers. It provides a rich feature-set with simple workflow designed to grealty reduce the amount of code written.

### OMMotor

The OMMotor class provides easy, truly non-blocking stepper motor control of a motor connected to a nanoMoCo device, or other Arduino-based device with an attached step/direction stepper driver. This class provides both continuous motion and motion with a specified number of steps, all without delay or blocking, designed to reduce the amount of code written for new applications. 

While OMMotor is limited to using a single motor per Arduino, it provides rich capabilities not found in most other motor control libraries, including speeds up to 10,000 steps/second for non-eased moves and up to 5,000 steps/second for eased moves, automatic evaluation of point-to-point moves with multiple easing profiles, planned motion interleaved between other events, step-by-step plan execution, and more.

### OMMenuMgr

The OpenMoCo Menu Manager provides a nearly complete automation of a menuing system. This class is designed to allow for rapid development of menuing systems on Arduino devices, using five input controls (buttons), and character displays of at least two rows. 

Designed to make it easy to implement menu systems, the Menu Manager requires the developer to do little more than specify the structure of the menu, and how to draw on the display.

Key features:

*    Menu structures automatically and easily stored in program memory instead of SRAM
*    Automatic handling of either analog or digital button inputs
*    Automatic management of user inputs for numeric and select list types
**        In-place editing with ability for user to abort
**        Write-back on user save to original variables
**        Specify lists of display values for users to select between
**        Control over user input precision in floats, and more
*    Ability to trigger code actions from menu items
*    One-method polling automatically handles and executes menu as-needed
*    Streamlined setup for complex and even recursive menus
*    Support for any width screen
*    Reduced memory footprint
*    No fixed limitations on menu depths, have as many sub-menus as you require
*    Flexible drawing methodology allows the user of any character-based (or even graphic) display

### OMMoCoBus

The OMMoCoBus class provides the core functionality needed for implementing the MoCoBus protocol on RS-485 networks.

### OMMoCoNode

The OMMoCoNode class provides everything you need to quickly and easily implement a new node type on a MoCoBus network, requiring you only to write the code to handle your packet data.

### OMMoCoMaster

The MoCoBus Master class provides all necessary components needed for creating a new MoCoBus master using an Arduino HardwareSerial object. This class provides the core functionality needed to send commands to devices and receive the response

### OMComHandler

The OMComHandler class manages common-line relationships between devices on a MoCoBus network.



 

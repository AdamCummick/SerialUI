/*
**  SerialUI demo/test and tutorial.
**  Copyright (C) 2013, 2014 Pat Deegan.  All rights reserved.
**
** http://www.flyingcarsandstuff.com/projects/SerialUI
**
** Please let me know if you use SerialUI in your projects, and
** provide a URL if you'd like me to link to it from the SerialUI
** home.
**
** This program (and accompanying library) is free software;
** you can redistribute it and/or modify it under the terms of
** the GNU Lesser General Public License as published by the
** Free Software Foundation; either version 3 of the License,
** or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
** See file LICENSE.txt for further informations on licensing terms.
**
**
** ************************* OVERVIEW *************************
**
** SerialUI is useful when you want to provide a user interface
** through the serial channel, i.e menus, submenus and commands.
** It provides built-in support for setting everything up, as
** well as navigation and online help.
**
** This example demonstrates usage by creating the UI for a
** fictitious "SuperBlinker" device--some sort of RGB
** illumination system.
**
**
** ************************** USAGE **************************
**
** To use, simply adjust your serial connection to the settings
** specified in the "Serial Settings" section below (defaults
** to 115200 baud and newline terminators) and set the
** ledpin define to a pin that tied to an LED (pin 13 by
** default).
**
** Next, compile, upload and connect using the serial monitor
** (ensuring the settings match those specified below) and
** press "enter".
**
**
** ************************** MENUS **************************
**
** The menu structure we'll create here looks like:
**
**  * information
**  |
**  |
**  + enable -----* on
**  |             |
**  |             * off
**  |
**  |
**  + settings ----------* red
**  |                    |
**  |                    * green
**  |                    |
**  |                    * blue
**  |                    |
**  |                    * deviceid
**  |                    |
**  |                    * show
**  * exit
**
** So, a three-option top level menu (information, enable, settings) with
** two of those options leading to sub-menus.  Every "leaf" (option that
** doesn't lead to a submenu) is a command that uses one of the callbacks
** defined below to do its work (see the Callbacks section).
**
**
**
** ********************* SAMPLE TRANSCRIPT **********************
**
** Here's a sample of the interaction through the serial
** connection:

  +++ Welcome to the SuperBlinker +++
  Enter '?' to list available options.
  > ?
  *** Help for SuperBlinker Main Menu

    information         Retrieve data and current settings
    enable              Enable/disable device
    settings            Perform setup and config

    quit                Exit SerialUI
    ?                   List available menu items
  > settings
  SuperBlinker Settings
  > ?
  *** Help for SuperBlinker Settings

  red                 Set color [0-255]
  green
  blue
  deviceid            Set dev ID [string]
  show

  ..                  Move up to parent menu
  ?                   List available menu items

  > red
  ... 10
  OK
  > green
  ... 20
  OK
  > blue
  ... 30
  > deviceid
  ... YayDevice
  OK
  > show
  (Called 'show_info' from menu: SuperBlinker Settings)
  ID: YayDevice
  Color--> R:10 G:20 B:30
  Device is OFF
  > ..
  SuperBlinker Main Menu
  > quit
**
**
** The code is fully commented, see below.
** Enjoy!
** Pat Deegan
*/

// to use SerialUI, you need to include the main header:
#include <SerialUI.h>


/*
** ***************** Serial Settings ******************
**
** Make sure you're serial port/serial monitor is setup
** with the correct baud rate or redefine the
**  serial_baud_rate (integer)
**
** You may also set
**  serial_input_terminator (character)
** according to your environment, though this latest
** version handles all the regular EOL situations
** automatically, with readBytesToEOL().
**
** NOTE: Setting the correct serial_input_terminator is
** only important if you are using a "strange" (i.e not
** NL, CR, or CR+NL) input terminator.
*/
#define serial_baud_rate           115200
#define serial_input_terminator   '\n'

/* We'll be using a blinker to show that we're alive,
** so set the ledpin to something connected to an LED
*/
#define ledpin  13


/*
** ********************* SerialUI instance ***********************
**
** You _could_ use multiple instances of SerialUI, but normally a
** single global should suffice.
**
** The constructor is called with two (optional) parameters:
**
**  SerialUI([uint8_t num_top_level_menuitems_hint]]);
**
**   - a message string to show on entry, declared with SUI_DeclareString
**   - a hint concerning the number of top level menu items (to avoid the cost
**     of memory re-allocation, when you have many--say, more than 3)
**
** As mentioned both are optional but a greeting is nice as it lets you know
** everything is working.
*/

SUI::SerialUI mySUI(4);



// We'll also define a struct to hold our "SuperBlinker" settings, just
// to excercise various functions in this example.
// It will hold RGB settings, a device id and an on/off state.

// dev_id_maxlen, max string length for the device id:
#define dev_id_maxlen  30

// deviceInfo struct, a place to store our settings
typedef struct deviceInfo {

  unsigned long red;
  unsigned long green;
  unsigned long blue;
  String dev_id;
  bool state;
}
deviceInfo;

// Just declare a global deviceInfo structure for
// use below, initialized to all-zeros:
deviceInfo myDevice = {0};
// NOTE: In real life, I think the device resets as we
// close the serial connection, meaning that if you want
// your config to be permanent, you probably have to save
// it to eeprom or somesuch.




/*
** ********************* setup() ***********************
**
** The standard Arduino setup() function.  Here we'll
** setup serial comm and the menu structure.
*/


void setup()
{

  mySUI.setGreeting(F("+++ Welcome to the SuperBlinker +++\r\nEnter '?' to list available options."));

  // SerialUI acts just like (is actually a facade for)
  // Serial.  Use it, rather than Serial, throughout the
  // program.
  // Setup of SerialUI:
  mySUI.begin(serial_baud_rate); // serial line open/setup
  mySUI.setTimeout(20000);      // timeout for reads (in ms), same as for Serial.
  mySUI.setMaxIdleMs(30000);    // timeout for user (in ms)
  // how we are marking the "end-of-line" (\n, by default):
  mySUI.setReadTerminator(serial_input_terminator);


  // Setup variable state tracking -- will report changes to
  // Druid4Arduino (v >= 1.3.0) so it can automatically display
  // these in GUI.  See the VariableTracking example for more on this...
  mySUI.trackState(F("enabled"), &(myDevice.state));
  mySUI.trackState(F("red"), &(myDevice.red));
  mySUI.trackState(F("green"), &(myDevice.green));
  mySUI.trackState(F("blue"), &(myDevice.blue));



  // The SerialUI menu setup is a bit involved, and it
  // needs to know about the functions we'll be using as
  // callbacks. Instead of having a bunch of function
  // declarations, all the work is contained in a function
  // of its own at the bottom of the program.
  // Yes: *DO* check it out!
  setupMenus();


  // set our blinker pin as an output.
  pinMode(ledpin, OUTPUT);

}



/*
** ********************* loop() ***********************
**
** The standard Arduino loop() function.  Here we'll
** handle SerialUI user interaction, and blink our
** blinker when nothing is going on.
*/

// we'll keep the blinker state in a boolean global
boolean cur_blink_state = true;
uint8_t on_print_count = 0;
void loop()
{

  /* We checkForUser() periodically, to see
  ** if anyone is attempting to send us some
  ** data through the serial port.
  **
  ** This code checks at every pass of the main
  ** loop, meaning a user can interact with the 
  ** system at any time.  Should you want to
  ** check for user access only once (say have a
  ** N second wait on startup, and then forgo
  ** allowing SerialUI access), then increase the
  ** delay parameter and use checkForUserOnce(), e.g.
  **
  **    mySUI.checkForUserOnce(15000);
  **
  ** to allow 15 seconds to connect on startup only.
  **
  ** Called without parameters, or with 0, checkForUser 
  ** won't delay the program, as it won't block at all.
  ** Using a parameter > 0:
  **	checkForUser(MAX_MS);
  ** will wait for up to MAX_MS milliseconds for a user,
  ** so is equivalent to having a delay(MAX_MS) in the loop,
  ** when no user is present.
  */
  if (mySUI.checkForUser())
  {
    
    /* Now we keep handling the serial user's
    ** requests until they exit or timeout.
    */
    while (mySUI.userPresent())
    {
      // actually respond to requests, using
      mySUI.handleRequests();

      if (myDevice.state && on_print_count++ > 10)
      {
        on_print_count = 0;
        mySUI.print(F("ON at "));
        mySUI.println(millis());
      }
    }

  } /* end if we had a user on the serial line */


  // we toggle the LED pin just to show we're alive
  // and not currently processing serial interaction
  cur_blink_state = !cur_blink_state;
  digitalWrite(ledpin, cur_blink_state);


}


/*
** ********************* Callbacks ***********************
**
** SerialUI is good at handling requests from users,
** providing navigation and help information but it is
** completely agnostic in terms of application.
**
** There are two types of SerialUI menu items:
**
**    * Sub menus: lead you to another level of menu items
**
**    * Commands: actually perform some type of action.
**
** Exactly _what_ happens when a user issues a command is
** determined by the callback implementations.
**
** A callback is just a function with a
**
**  "void NAME(void)"
**
** signature, like:
**
**  void do_something() { / * do something cool * / }
**
** Within the callback, you can do stuff like
**   get user input,
**   provide output,
**   determine the menu from which the callback was called.
** or anything else you want to do.
**
** The examples below go through setting and displaying
** values of the deviceInfo defined above.
*/

// callback: turn_on
// set device (and the ledpin) on
void turn_on()
{

  // do our business, normally you'd probably
  // use the RGB settings to do something more
  // interesting, but for this example we'll
  // just turn on the ledpin
  myDevice.state = true;
  digitalWrite(ledpin, HIGH);

  // provide some feedback
  mySUI.println(F("ON"));
  mySUI.returnOK();
}



// callback: turn_on
// set device (and the ledpin) off
void turn_off()
{
  // do our business
  myDevice.state = false;
  digitalWrite(ledpin, LOW);

  // provide some feedback
  mySUI.println(F("OFF"));
  mySUI.returnOK();
}


void color_has_changed()
{
  mySUI.println(F("Color changed!"));

}

// callback: set_red
// set the device value for "red"
bool validate_color(unsigned long & newRed)
{
  bool isValid = (newRed > 0 && newRed < 255);

  if (! isValid)
  {
    mySUI.println(F("Value must be between 1-255"));
  }

  return isValid;

}

void green_has_changed()
{
  mySUI.println(F("Oh my, green has been modified!"));
}


// callback: show_info
// Output some details about the device
void show_info()
{

  // this callback is used as a command in multiple
  // menus (see setupMenus(), below).  As a demonstration,
  // we'll start by telling the user where this function was
  // called from.

  // You can always get the currently active menu from
  // SerialUI's currentMenu():
  SUI::Menu * current_menu = mySUI.currentMenu();

  // output a line to indicate where we were called from:
  mySUI.print(F("(Called 'show_info' from menu: "));
  current_menu->showName();
  mySUI.println(')');


  // use Serial-like print statements to output the info
  mySUI.print(F("ID: "));
  mySUI.println(myDevice.dev_id);
  mySUI.print(F("Color--> R:"));
  mySUI.print(myDevice.red, DEC);
  mySUI.print(F(" G:"));
  mySUI.print(myDevice.green, DEC);
  mySUI.print(F(" B:"));
  mySUI.println(myDevice.blue, DEC);

  mySUI.print(F("Device is "));
  if (myDevice.state)
  {
    mySUI.println(F("ON"));
  }
  else {
    mySUI.println(F("OFF"));
  }


  mySUI.returnOK();

}

void do_exit() {
  // though you can always just use the "quit" command from
  // the top level menu, this demonstrates using exit(), which
  // will automatically close the Druid4Arduino GUI, if
  // being used.
  mySUI.print(F("Exit requested, terminating GUI if present"));
  mySUI.exit();
}

/*
** ********************* setupMenus() ***********************
**
** setupMenus creates the top level menu and 2 sub-menus, and
** demonstrates a few additional functions of SerialUI setup.
**
**
** Menu item order is set by the order with which they are added, using
** addCommand()/subMenu() (see below).

*/


void setupMenus()
{
  /*
  ** Now it's time to start creating menus.  SerialUI always has
  ** one "top level" menu available which you can use to create items for
  ** commands and access to sub menus.
  */

  // Get a handle to the top level menu
  // Note that menus are returned as pointers.
  SUI::Menu * mainMenu = mySUI.topLevelMenu();
  if (! mainMenu)
  {
    // what? Could not create :(
    return mySUI.returnError(F("Something is very wrong, could not get top level menu?"));
  }


  // we can set the name (title) of any menu using
  // setName().  This shows up in help output and when
  // moving up and down the hierarchy.  If it isn't set,
  // the menu key will be used, for sub-menus, and the default
  // SUI_SERIALUI_TOP_MENU_NAME ("TOP") for top level menus.
  mainMenu->setName(F("TOP"));


  /*
  ** addCommand(KEY, CALLBACK [, HELP])
  **
  ** Use addCommand() to add a command menu item to a menu.
  ** The parameters are:
  **
  **  KEY: the (SUI_DeclareString-created) string to use as the command
  **
  **  CALLBACK: the name of the void(void) callback function (as described
  **            in "Callbacks", above).
  **
  **  HELP: optional (SUI_DeclareString-created) string to display for
  **        this item when menu help (?) is invoked.
  **
  ** Return:  returns boolean true on success, false if command could not
  **          be added.
  */
  if (! mainMenu->addCommand(F("info"), show_info, F("show information")) )
  {
    // should check that addCommand succeeded -- we'll skip this below for brevity.
    // You might want to #ifdef these checks to enable during dev and disable when
    // you know everything's working, if space is tight.

    // what? Could not create :(
    return mySUI.returnError(F("Could not addCommand to mainMenu?"));

  }



  /*
  ** subMenu(KEY [, HELP])
  **
  ** Use subMenu to create a sub-menu accessible by KEY.  The params are:
  **
  **  KEY: The (SUI_DeclareString-created) string to use to enter the sub-menu
  **       from the current menu.
  **
  **  HELP: optional (SUI_DeclareString-created) string to display for
  **        this item when menu help (?) is invoked.
  **
  **  Return: returns a SUI::Menu pointer, which will be NULL if the submenu
  **          could not be created.
  **
  */
  SUI::Menu * enableMenu = mainMenu->subMenu(F("enable"), F("enable/disable blinker"));
  if (! enableMenu)
  {
    // ah, could not create :(
    return mySUI.returnError(F("Couldn't create enable menu!?"));


  }

  // enable menu created, add our commands
  enableMenu->addCommand(F("on"), turn_on);
  enableMenu->addCommand(F("off"), turn_off);


  SUI::Menu * settingsMenu = mainMenu->subMenu(F("settings"), F("Configure settings"));
  if (! settingsMenu)
  {
    // ah, could not create :(
    return mySUI.returnError(F("Couldn't create settings menu!?"));

  }
  // settings menu created.
  // set its name and add the commands
  settingsMenu->setName(F("Settings"));

  SUI_FLASHSTRING_PTR SetColorHelp = F("Set color [0-255]");


  SUI_FLASHSTRING_PTR CouldntAddRequset = F("Could not add request?");

  // we could use a regular command callback to get the device info
  // but since we only want the value, we use the new "addRequest"
  // addRequest may be called with (or without) a validator and/or a callback
  // to notify us when some value has changed
  if (!settingsMenu->addRequest(&(myDevice.red), F("red"), SetColorHelp, validate_color)) {
      return mySUI.returnError(CouldntAddRequset);

  }

  // we add a change notification callback to this one, just fer fun
  if (!settingsMenu->addRequest(&(myDevice.green), F("green"), SetColorHelp, validate_color, green_has_changed)) {
      return mySUI.returnError(CouldntAddRequset);

  }

  if (!settingsMenu->addRequest(&(myDevice.blue), F("blue"), SetColorHelp, validate_color)) {
      return mySUI.returnError(CouldntAddRequset);
  }


  // the String version of addRequest requires an addition param: the max size of the
  // string entered
  settingsMenu->addRequest(&(myDevice.dev_id), dev_id_maxlen, F("deviceid"), F("Set dev id [string]"));


  settingsMenu->addCommand(F("show"), show_info);


  if (! mainMenu->addCommand(F("exit"), do_exit, F("exit (and terminate Druid)")) )
  {
    return mySUI.returnError(F("Could not addCommand to mainMenu?"));
  }

  // Done setting up the menus!

}
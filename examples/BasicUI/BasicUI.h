
#ifndef _BasicUI_BasicUI_h
#define _BasicUI_BasicUI_h


/*
 * BasicUI.h -- part of the BasicUI project.
 * Declarations for everything that was generated...
 * Pat Deegan
 * Psychogenic.com 
 * 
 * Copyright (C) 2019 Pat Deegan, psychogenic.com
 * 
 * Generated by DruidBuilder [https://devicedruid.com/], 
 * as part of project "f479e26ae09b4eab8cb47b1383145f81zGgk4QUJUp",
 * aka BasicUI.
 * 
 * Druid4Arduino, Device Druid, Druid Builder, the builder 
 * code brewery and its wizards, SerialUI and supporting 
 * libraries, as well as the generated parts of this program 
 * are 
 *            Copyright (C) 2013-2019 Pat Deegan 
 * [https://psychogenic.com/ | https://inductive-kickback.com/]
 * and distributed under the terms of their respective licenses.
 * See https://devicedruid.com for details.
 * 
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
 * PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, 
 * YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
 * CORRECTION.
 * 
 * Keep in mind that there is no warranty and you are solely 
 * responsible for the use of all these cool tools.
 * 
 * Play safe, have fun.
 * 
 */



/* we need the SerialUI lib */
#include <SerialUI.h>


/* MySUI
 * Our SerialUI Instance, through which we can send/receive
 * data from users.
 */ 
extern SUI::SerialUI MySUI;









/* ********* callbacks and validation functions ********* */



/* *** Main Menu *** */
namespace MainMenu {

void doClickMe();

void YesOrNoChanged();

void LimitedChoiceChanged();

void NameChanged();

void OptionsChanged();



/* *** Main Menu -> sub menu *** */
namespace SubMenu {

void WhenChanged();

void doAnotherCommand();

} /* namespace SubMenu */

} /* namespace MainMenu */





/*
 * The container for MyInputs, which
 * holds all the variables for the various inputs.
 */
typedef struct MyInputsContainerStruct {
	
	SerialUI::Menu::Item::Request::Event When;
	SerialUI::Menu::Item::Request::Toggle YesOrNo;
	SerialUI::Menu::Item::Request::BoundedLong LimitedChoice;
	SerialUI::Menu::Item::Request::String Name;
	SerialUI::Menu::Item::Request::OptionsList Options;
	// constructor to set sane startup vals
	MyInputsContainerStruct() : 
		When(18 /*start hour*/, 0 /*min*/, 0 /*sec*/, 120 /*num mins*/ ,SUI_STR("When"),SUI_STR("an event"),MainMenu::SubMenu::WhenChanged),
		YesOrNo(false,SUI_STR("Yes Or No"),SUI_STR("A boolean toggle"),MainMenu::YesOrNoChanged),
		LimitedChoice(0,SUI_STR("Limited Choice"),SUI_STR("A range of available values"),1,50,MainMenu::LimitedChoiceChanged),
		Name("",SUI_STR("Name"),SUI_STR("a text string"),request_inputstring_maxlen,MainMenu::NameChanged),
		Options(1,SUI_STR("Options"),SUI_STR("Choose one of these"),SUI_STR("A"),SUI_STR("B"),SUI_STR("or C"),NULL,NULL,NULL,MainMenu::OptionsChanged)
	{}
} MyInputsContainerSt;

extern MyInputsContainerSt MyInputs;





/* ***** SetupSerialUI: where we'll be setting up menus and such **** */
bool SetupSerialUI();


#define DIE_HORRIBLY(msg)	for(;;){ MySUI.println(msg); delay(1000); }


#endif

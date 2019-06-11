/*
 * SerialUI.h
 *
 *  Created on: May 25, 2018
 *      Author: Pat Deegan
 *
 *  Part of the SerialUI project.
 *  Copyright (C) 2014-2019 Pat Deegan, https://psychogenic.com
 *  More information on licensing and usage at
 *  https://devicedruid.com/
 */

#ifndef SERIALUIV3_SRC_SERIALUI_H_
#define SERIALUIV3_SRC_SERIALUI_H_

#include "includes/menuitem/items.h"
#include "includes/tracked/tracked.h"
#include "includes/SerialUI.h"
#include "includes/SUIGlobals.h"

#include "includes/SerialUIPlatform.h"

#ifdef SERIALUI_BUILD_TESTS
#include "includes/test/tests.h"
#endif

#ifdef SERIALUI_SERIALIZER_JSON_ENABLE
#include "includes/settings/SerializerJson.h"
#include "includes/settings/DeSerializerJson.h"
#endif

#ifdef SERIALUI_PLATFORM_LINUX
#include "includes/platform/linux/LinuxStorageFilesystem.h"
#include "includes/python/ExternalModule.h"
#endif

#ifdef SERIALUI_AUTHENTICATOR_ENABLE
#include "includes/auth/Authenticator.h"
#include "includes/auth/AuthSimple.h"
#endif

#endif /* SERIALUIV3_SRC_SERIALUI_H_ */

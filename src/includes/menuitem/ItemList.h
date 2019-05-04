/*
 * List.h
 *
 *  Created on: Apr 29, 2019
 *      Author: malcalypse
 */

#ifndef SERIALUI_SRC_INCLUDES_MENUITEM_ITEMLIST_H_
#define SERIALUI_SRC_INCLUDES_MENUITEM_ITEMLIST_H_

#include "SubMenu.h"
namespace SerialUI {
namespace Menu {
namespace Item {

class List : public SubMenu {
public:
	List(ID parentId, StaticString key, StaticString help);
	List(ID id, ID parentId, StaticString key, StaticString help);

};

} /* namespace Item */
} /* namespace Menu */
} /* namespace SerialUI */

#endif /* SERIALUI_SRC_INCLUDES_MENUITEM_ITEMLIST_H_ */

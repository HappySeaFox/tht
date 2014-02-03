/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEFAULTSTYLES_H
#define DEFAULTSTYLES_H

#define THT_TARGET_DEFAULT_STYLESHEET \
"QLabel#target               { qproperty-pixmap: url(:/images/drag.png); }" \
"QLabel#target[alt=\"true\"] { qproperty-pixmap: url(:/images/drag_red.png); }"

#define THT_LIST_DETAILS_DEFAULT_STYLESHEET \
"QLabel#listDetails { border: 1px solid gray; background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffefef, stop:0.35 #f7db45, stop:0.65 #f7db45, stop:1 #ffefef); }" \
"QLabel#listDetails QLabel#currentTicker { color: black; }" \
"QLabel#listDetails QLabel#lineTickersSeparator { background-color: gray; }" \
"QLabel#listDetails QLabel#totalTickers { color: black; }"

#define THT_BUTTON_LINK_MANAGER_DEFAULT_STYLESHEET \
"QToolButton#pushLinkManager { qproperty-icon: url(:/images/links-load.png); }"

#define THT_LIST_BUTTONS_DEFAULT_STYLESHEET \
"QToolButton#pushList   { qproperty-icon: url(:/images/list.png);   }" \
"QToolButton#pushAdd    { qproperty-icon: url(:/images/add.png);    }" \
"QToolButton#pushSave   { qproperty-icon: url(:/images/save.png);   }" \
"QToolButton#pushExport { qproperty-icon: url(:/images/export.png); }"

#define THT_BUSY_ICONS_DEFAULT_STYLESHEET \
"QLabel#labelBusy    { qproperty-pixmap: url(:/images/locked.png); }" \
"QLabel#labelNotBusy { qproperty-pixmap: url(:/images/ready.png); }"

#endif // DEFAULTSTYLES_H

/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HOTKEY_H
#define HOTKEY_H

#include <Qt>

struct Hotkey
{
    Hotkey(const Qt::Key &_key = Qt::Key_0,
           const Qt::KeyboardModifiers &_modifiers = Qt::NoModifier)
        : key(_key), modifiers(_modifiers)
    {}

    Qt::Key key;
    Qt::KeyboardModifiers modifiers;
};

bool operator== (const Hotkey &a, const Hotkey &b);

#endif // HOTKEY_H

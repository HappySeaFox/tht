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

inline
bool operator== (const Hotkey &a, const Hotkey &b)
{
    return a.key == b.key
            && a.modifiers == b.modifiers;
}


#endif // HOTKEY_H

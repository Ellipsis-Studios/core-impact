//
//  CUTextField.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a single line text field. It is useful
//  for providing input forms for your application, such as saved games or
//  player settings.  Because it is only a single line, it is a subclass of
//  label.  A multiline text input would be a TextArea, and that is not
//  currently supported.
//
//  To make use of a TextField, BOTH Keyboard and TextInput input devices
//  must be activated.  In particular, TextInput allows the support of
//  virtual keyboards on mobile devices.
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White and Enze Zhou
//  Version: 8/20/20
//
#include <cugl/input/cu_input.h>
#include <cugl/scene2/ui/CUTextField.h>
#include <cugl/base/CUApplication.h>

using namespace cugl::scene2;

/** The pixel width of the cursor */
#define CURSOR_WIDTH  3
/** The number of frames to cycle before blinking the cursor */
#define CURSOR_PERIOD 25
/** The number of milliseconds to delay until continuous deletion */
#define DELETE_DELAY  500

#pragma mark -
#pragma mark Constructors
/**
 * Creates an uninitialized text field with no size or font.
 *
 * You must initialize this field before use.
 *
 * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a Node on the
 * heap, use one of the static constructors instead.
 */
TextField::TextField() :
_active(false),
_focused(false),
_mouse(true),
_altDown(false),
_metaDown(false),
_backDown(false),
_backCount(0),
_cursorBlink(0),
_cursorIndex(0),
_textLength(-1),
_tkey(0),
_kkey(0),
_fkey(0),
_nextKey(1) {
    _name = "TextField";
}

/**
 * Disposes all of the resources used.
 *
 * A disposed text field can be safely reinitialized. Any child will
 * be released.  They will be deleted if no other object owns them.
 *
 * It is unsafe to call this on a text field that is still currently
 * inside of a scene graph.
 */
void TextField::dispose() {
    if (_active) {
        deactivate(true);
    }
    
    _typeListeners.clear();
    _exitListeners.clear();
    _nextKey = 1;
    _tkey = 0;
    _kkey = 0;
    _fkey = 0;
	Label::dispose();
}


#pragma mark -
#pragma mark Listeners
/**
 * Returns the type listener for the given key
 *
 * This listener is invoked when the text changes.
 *
 * If there is no listener for the given key, it returns nullptr.
 *
 * @param key   The identifier for the listener
 *
 * @return the type listener for the given key
 */
const TextField::Listener TextField::getTypeListener(Uint32 key) const {
    auto item = _typeListeners.find(key);
    if (item == _typeListeners.end()) {
        return nullptr;
    }
    return item->second;
}

/**
 * Returns all type listeners for this text field
 *
 * This listener is invoked when the text changes.
 *
 * @return the type listeners for this text field
 */
const std::vector<TextField::Listener> TextField::getTypeListeners() const {
    std::vector<Listener> result;
    result.reserve(_typeListeners.size());
    for(auto kv : _typeListeners) {
        result.push_back(kv.second);
    }
    return result;
}

/**
 * Returns a key for a type listener after adding it to this text field.
 *
 * This listener is invoked when the text changes.
 *
 * C++ cannot hash functions types.  Therefore, the listener will
 * be identified by a unique key, returned by this function.  You should
 * remember this key to remove the listener if necessary.
 *
 * @param listener  The listener to add
 *
 * @return the key for the listener
 */
Uint32 TextField::addTypeListener(Listener listener) {
    CUAssertLog(_nextKey < (Uint32)-1, "No more available listener slots");
    _typeListeners[_nextKey++] = listener;
    return _nextKey;
}

/**
 * Removes a type listener from this text field.
 *
 * This listener is invoked when the text changes.
 *
 * Listeners must be identified by the key returned by the {@link #addListener}
 * method. If this text field does not have a listener for the given key,
 * this method will fail.
 *
 * @param key  The key of the listener to remove
 *
 * @return true if the listener was succesfully removed
 */
bool TextField::removeTypeListener(Uint32 key) {
    if (_typeListeners.find(key) == _typeListeners.end()) {
        return false;
    }
    _typeListeners.erase(key);
    return true;
}

/**
 * Clears all type listeners for this text field.
 *
 * These listeners are invoked when the text changes. This method
 * does not require you to remember the keys assigned to the
 * individual listeners.
 *
 * @return true if the listener was succesfully removed
 */
void TextField::clearTypeListeners() {
    _typeListeners.clear();
}
  
/**
 * Returns the exit listener for the given key
 *
 * This listener is invoked when the field loses focus.
 *
 * If there is no listener for the given key, it returns nullptr.
 *
 * @param key   The identifier for the listener
 *
 * @return the listener for the given key
 */
const TextField::Listener TextField::getExitListener(Uint32 key) const {
    auto item = _exitListeners.find(key);
    if (item == _exitListeners.end()) {
        return nullptr;
    }
    return item->second;
}

/**
 * Returns all exit listeners for this text field
 *
 * These listeners are invoked when the field loses focus.
 *
 * @return the exits listeners for this text field
 */
const std::vector<TextField::Listener> TextField::getExitListeners() const {
    std::vector<Listener> result;
    result.reserve(_exitListeners.size());
    for(auto kv : _exitListeners) {
        result.push_back(kv.second);
    }
    return result;
}

/**
 * Returns a key for a exit listener after adding it to this field.
 *
 * This listener is invoked when the field loses focus.
 *
 * C++ cannot hash functions types.  Therefore, the listener will
 * be identified by a unique key, returned by this function.  You should
 * remember this key to remove the listener if necessary.
 *
 * @param listener  The listener to add
 *
 * @return the key for the listener
 */
Uint32 TextField::addExitListener(Listener listener) {
    CUAssertLog(_nextKey < (Uint32)-1, "No more available listener slots");
    _exitListeners[_nextKey++] = listener;
    return _nextKey;
}

/**
 * Removes a listener from this text fieldexi.
 *
 * This listener is invoked when the field loses focus.
 *
 * Listeners must be identified by the key returned by the {@link #addListener}
 * method. If this text field does not have a listener for the given key,
 * this method will fail.
 *
 * @param key  The key of the listener to remove
 *
 * @return true if the listener was succesfully removed
 */
bool TextField::removeExitListener(Uint32 key) {
    if (_exitListeners.find(key) == _exitListeners.end()) {
        return false;
    }
    _exitListeners.erase(key);
    return true;
}

/**
 * Clears all listeners for this text field.
 *
 * These listener are invoked when the field loses focus. This
 * method does not require you to remember the keys assigned to
 * the individual listeners.
 *
 * @return true if the listener was succesfully removed
 */
void TextField::clearExitListeners() {
    _exitListeners.clear();
}


#pragma mark -
#pragma mark Editing
/**
 * Sets the text for this label.
 *
 * All unprintable characters will be removed from the string.  This
 * includes tabs and newlines. They will be replaced by spaces.
 *
 * The string must be in either ASCII or UTF8 format.  No other string
 * encodings are supported.  As all ASCII strings are also UTF8, you can
 * this effectively means that the text must be UTF8.
 *
 * If the font is missing glyphs in this string, the characters in the text
 * may be different than those displayed.
 *
 * If this text field has a type listener, it will be invoked when this
 * method is called.
 *
 * @oaram text      The text for this label.
 * @oaram resize    Whether to resize the label to fit the new text.
 */
void TextField::setText(const std::string& text, bool resize) {
    Label::setText(text, resize);
    _textLength = _font->getInternalBounds(_text.data()).size.width;
    updateCursor();
    
    for(auto it = _typeListeners.begin(); it != _typeListeners.end(); ++it) {
        it->second(getName(), _text);
    }
}

/**
 * Activates this text field to enable editing.
 *
 * This method attaches a listener to either the {@link Mouse} or
 * {@link Touchscreen} inputs to monitor when the text field is pressed
 * and/or released. The text field will favor the mouse, but will use the
 * touch screen if no mouse input is active. If neither input is active,
 * this method will fail.
 *
 * It will also attach a listener to {@link TextInput} to provide access
 * to a (possibly virtual) keyboard and collect user typing.  Finally, it
 * attaches a liatener to {@link Keyboard} to monitor special keys such
 * as Alt, Meta, and the arrow keys.
 *
 * Notice that activating a text field and requesting focus is separate.
 * The field will request focus if selected by a touch or press, but it
 * cannot be editted until it has focus.
 *
 * @return true if the text field was successfully activated
 */
bool TextField::activate() {
    if (_active) {
        return false;
    }

    // Verify we have all the right handers
    TextInput* textInput = Input::get<TextInput>();
    CUAssertLog(textInput, "The TextInput device has not been enabled");
    Keyboard* keyBoard   = Input::get<Keyboard>();
    CUAssertLog(keyBoard, "The keyboard device has not been enabled");
    Mouse* mouse = Input::get<Mouse>();
    Touchscreen* touch = Input::get<Touchscreen>();
    CUAssertLog(mouse || touch,  "Neither mouse nor touch input is enabled");


    bool check;
    if (!_tkey) { _tkey = textInput->acquireKey(); }
    check = textInput->addListener(_tkey, [this](const TextInputEvent& event, bool focus) {
        updateInput(event,focus);
    });
    if (!check) {
        return false;
    }
    
    if (!_kkey) { _kkey = keyBoard->acquireKey(); }
    check = keyBoard->addKeyUpListener(_kkey, [this](const KeyEvent& event, bool focus) {
        updateKey(event,focus,false);
    });
    if (!check) {
        textInput->removeListener(_tkey);
        return false;
    }

    check = keyBoard->addKeyDownListener(_kkey, [this](const KeyEvent& event, bool focus) {
        updateKey(event,focus,true);
    });
    if (!check) {
        textInput->removeListener(_tkey);
        keyBoard->removeKeyUpListener(_kkey);
        return false;
    }

    if (mouse) {
    	_mouse = true;
        if (!_fkey) { _fkey = mouse->acquireKey(); }
    	check = mouse->addPressListener(_fkey, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
    		updatePress(event.position, focus);
    	});
    } else {
    	_mouse = false;
        if (!_fkey) { _fkey = touch->acquireKey(); }
    	check = touch->addBeginListener(_fkey, [=](const TouchEvent& event, bool focus) {
    		updatePress(event.position, focus);
    	});
    }
    if (!check) {
    	textInput->removeListener(_tkey);
    	keyBoard->removeKeyUpListener(_kkey);
    	keyBoard->removeKeyDownListener(_kkey);
    	return false;
    }

    _active = true;
	return true;
}

/**
 * Deactivates this text field, ignoring any future input.
 *
 * This method removes its internal listener from either the {@link Mouse}
 * or {@link Touchscreen}, and from {@link Keyboard} and {@link TextInput}.
 *
 * When deactivated, the text field will no longer change its text on its own.
 * However, the user can still change manually with the
 * {@link setText(std::string&, bool)} method. Futhermore, the appropriate
 * type listener will be called when the text changes. However, any attempts
 * to manually acquire focus will fail.
 *
 * @return true if the text field was successfully deactivated
 */
bool TextField::deactivate(bool dispose) {
	if (!_active) {
		return false;
	}

	bool success = true;
    if (_focused && !dispose) {
        success = releaseFocus();
    }
    TextInput* textInput = Input::get<TextInput>();
    Keyboard* keyBoard  = Input::get<Keyboard>();
    
    success = textInput->removeListener(_tkey) && success;
    success = keyBoard->removeKeyUpListener(_kkey) && success;
    success = keyBoard->removeKeyDownListener(_kkey) && success;
    if (_mouse) {
        Mouse* mouse = Input::get<Mouse>();
        CUAssertLog(mouse,  "Mouse input is no longer enabled");
        success = mouse->removePressListener(_fkey);
    } else {
        Touchscreen* touch = Input::get<Touchscreen>();
        CUAssertLog(touch,  "Touch input is no longer enabled");
        success = touch->removeBeginListener(_fkey);
    }
    
	_active = false;
	return success;
}

/**
 * Requests text input focus for this text field.
 *
 * When a text field is activated, it does not immediately have focus.
 * A text field without focus cannot be editted.  By either clicking
 * on the field or calling thus function, you can acquire focus and edit
 * the field.
 *
 * This method will fail if the text field is not active.
 *
 * @return true if successfully requested focus.
 */
bool TextField::requestFocus() {
	if (!_active || _focused) {
		return false;
	}

    TextInput* textInput = Input::get<TextInput>();
    CUAssertLog(textInput, "The TextInput device has not been enabled");
    Keyboard* keyBoard   = Input::get<Keyboard>();
    CUAssertLog(keyBoard, "The keyboard device has not been enabled");

    if (!textInput->requestFocus(_tkey)) {
    	return false;
    } else if (!keyBoard->requestFocus(_kkey)) {
    	textInput->releaseFocus();
    	return false;
    }
    
    textInput->setValidator([this](const std::string& value) {
        return validate(value);
    });
    textInput->begin();

    _altDown  = false;
    _metaDown = false;
    _backDown = false;

    _focused = true;
    _cursorBlink = 0;
	_cursorIndex = 0;
	updateCursor();
    return true;
}

/**
 * Releases text input focus for this text field.
 *
 * When the focus is released, the label can no longer be editting.
 * Typically this means that the user has input the final value,
 * which is why the exit listener (if any exists) is called.
 *
 * In addition to calling this method manually, a user can release focus
 * either by pressing RETURN or clicking somewhere outside of the field.
 *
 * @return true if successfully released focus.
 */
bool TextField::releaseFocus() {
	if (!_focused) {
		return false;
	}

    TextInput* textInput = Input::get<TextInput>();
    CUAssertLog(textInput, "The TextInput device is no longer enabled");
    Keyboard* keyBoard   = Input::get<Keyboard>();
    CUAssertLog(keyBoard, "The keyboard device is no longer enabled");

    textInput->end();
    if (textInput->currentFocus() == _tkey) {
		textInput->releaseFocus();
    }
    if (keyBoard->currentFocus() == _kkey) {
		keyBoard->releaseFocus();
    }
    
    for(auto it = _exitListeners.begin(); it != _exitListeners.end(); ++it) {
        it->second(getName(), _text);
    }
    _focused = false;
    return true;
}


#pragma mark -
#pragma mark Rendering
/**
 * Draws this text field via the given SpriteBatch.
 *
 * This method only worries about drawing the current text field.  It does not
 * attempt to render the children.
 *
 * This method provides the correct transformation matrix and tint color.
 * You do not need to worry about whether the node uses relative color.
 * This method is called by render() and these values are guaranteed to be
 * correct.  In addition, this method does not need to check for visibility,
 * as it is guaranteed to only be called when the node is visible.
 *
 * This method overrides the one from {@link Label}. It adds the drawing of
 * a blinking cursor that indicates the edit position.
 *
 * @param batch     The SpriteBatch to draw with.
 * @param transform The global transformation matrix.
 * @param tint      The tint to blend with the Node color.
 */
void TextField::draw(const std::shared_ptr<SpriteBatch>& batch, const Mat4& transform, Color4 tint) {
    Label::draw(batch, transform, tint);

	if (_focused) {
		_cursorBlink--;
		if (_cursorBlink < 0) {
			batch->setTexture(Texture::getBlank());
			batch->setColor(_foreground);
			batch->fill(_cursor);
		}
        if (_cursorBlink == -CURSOR_PERIOD) {
			_cursorBlink = CURSOR_PERIOD;
        }
	}
}


#pragma mark -
#pragma mark Internal Helpers
/**
 * Deletes one character before the current cursor.
 *
 * If alt is pressed, it will delete an entire word. If meta is pressed,
 * it will delete the entire field.
 */
void TextField::deleteOne() {
	if (_cursorIndex >= _text.length()) {
        return;
    }
    
    std::string replace;
    if (_metaDown) {
        replace = "";
    } else if (_altDown) {
        replace = _text.substr(0, skipWord(false));
    } else {
        replace = _text.substr(0, _text.length() - _cursorIndex - 1);
    }

    if (_cursorIndex != 0) {
        replace += _text.substr(_text.length() - _cursorIndex, _cursorIndex);
    }
    
    setText(replace);
}

/**
 * Deletes one character if counter is the current back counter.
 *
 * This method is used to implement continous deletion.  When the
 * backspace key is held for a long time, the field schedules this
 * function as a callback with the application.  When in invoked,
 * it will delete a single letter each frame.  It will execute until
 * the back counter changes, which happens when the key is released.
 *
 * @param counter   The timer to validate deletion
 *
 * @return true if deletion should continue
 */
bool TextField::deleteMany(Uint32 counter) {
    if (!_backDown || counter != _backCount) {
        return false;
    }
    
    deleteOne();
    return true;
}

/**
 * Updates the text with the given input data.
 *
 * This method is a callback for the {@link TextInput} device.
 *
 * @param event        The text input event to be handled.
 * @param focus        Whether the text field currently has text input focus.
 */
void TextField::updateInput(const TextInputEvent& Event, bool focus) {
    if (!_focused) {
		return;
    }
    
	std::string replace;
	replace = _text.substr(0, _text.length() - _cursorIndex);
	replace += Event.added;
    if (_cursorIndex != 0) {
		replace += _text.substr(_text.length() - _cursorIndex, _cursorIndex);
    }
    
	setText(replace);
}

/**
 * Updates the state of any special keys.
 *
 * This method is a callback for the {@link Keyboard} device.
 *
 * @param event        The key event to be handled.
 * @param focus        Whether the text field currently has keyboard focus
 * @param down      Whether the key is pressed down (as opposed to released)
 */
void TextField::updateKey(const KeyEvent& Event, bool focus, bool down) {
    if (!_focused) {
    	return;
    }
    
    // Simple key tracking
    switch (Event.keycode) {
	case KeyCode::BACKSPACE:
        _backDown = down;
        _backCount++;
        break;
	case KeyCode::LEFT_ALT:
	case KeyCode::RIGHT_ALT:
        _altDown = down;
        break;
	case KeyCode::LEFT_META:
	case KeyCode::RIGHT_META:
        _metaDown = down;
        break;
    default:
        break;
    }
    
    if (!down) {
        return;
    }
    
    // Down specific functionality.
    Uint32 localcount = _backCount;
    switch (Event.keycode) {
    case KeyCode::BACKSPACE:
        deleteOne();
        Application::get()->schedule([=](void){
            return this->deleteMany(localcount);
        }, DELETE_DELAY, 0);
        break;
	case KeyCode::ARROW_LEFT:
		if (_cursorIndex < _text.length()) {
            if (_metaDown) {
				_cursorIndex = (int) _text.length();
            } else if (_altDown) {
                _cursorIndex = (int) _text.length()-skipWord(false);
            } else {
				_cursorIndex++;
            }
            _cursorBlink = 0;
			updateCursor();
        }
		break;
	case KeyCode::ARROW_RIGHT:
		if (_cursorIndex > 0) {
            if (_metaDown) {
				_cursorIndex = 0;
            } else if (_altDown) {
				_cursorIndex = (int) _text.length() - skipWord(true);
            } else {
				_cursorIndex--;
            }
            _cursorBlink = 0;
			updateCursor();
		}
		break;
    case KeyCode::ENTER:
    case KeyCode::KEYPAD_ENTER:
    case KeyCode::RETURN:
        releaseFocus();
        break;
	default:
        break;
    }
}

/**
 * Responds to a touch or press, changing the field focus.
 *
 * If the press is outside text field, focus will be released. If the
 * press is within the bounds, it will request focus (if not currently
 * in focus) and move the cursor to the position pressed.
 *
 * @param pos   The screen coordinate where the event happened.
 */
void TextField::updatePress(Vec2 pos, bool focus) {
	Vec2 cooPos = screenToNodeCoords(pos);
	if (!Rect(Vec2::ZERO, getContentSize()).contains(cooPos)) {
        if (_focused) {
            releaseFocus();
        }
    } else {
        if (!_focused) {
            requestFocus();
        }

        float last = 0;
        float next = 0;
        float offset = _textbounds.origin.x + _textLength;
        int index = -1;
        for (int ii = 1; last >= 0 && ii <= _text.length(); ii++) {
            next = _font->getInternalBounds(_text.data() + _text.length() - ii).size.width;
            if (offset - (last + next) / 2.0 < cooPos.x) {
                index = ii - 1;
                last  = -1;
            } else {
                last = next;
            }
        }

        _cursorIndex = (index == -1 ? (int) _text.length() : index);
        _cursorBlink = 0;
        updateCursor();
    }
}

/**
 * Updates the cursor position.
 *
 * This method is called whenever either the text changes or the cursor
 * moves.
 */
void TextField::updateCursor() {
    float cursorPos = 0;
    if (_cursorIndex > 0) {
        cursorPos = _font->getInternalBounds(_text.data() + _text.length() - _cursorIndex).size.width;
    }
    
    Vec2 origin = _textbounds.origin;
	origin.x += _textLength;
	origin.x -= cursorPos;
	origin.x -= 1.5;
	_cursor.origin = nodeToWorldCoords(origin);
	_cursor.size.height = _textbounds.size.height;
	_cursor.size.width = CURSOR_WIDTH;
}

/**
 * Returns true if value is a valid input character.
 *
 * This method is used by {@link TextInput} to validate input.  If a character
 * is not valid, it is dropped.
 *
 * @param value The (UTF8) character to validate
 *
 * @return true if value is a valid input character.
 */
bool TextField::validate(const std::string& value) {
    int letter = (int)value[0];
    if (letter < 32 || letter == 127) {
        return false;
    }
    
    std::string check = _text+value;
    Size bounds = _font->getInternalBounds(check.data()).size;
    
    return (bounds.width < _contentSize.width);
}

/**
 * Moves the cursor one word forward or backward.
 *
 * If there is any space between the cursor and the word in the correct
 * direction, it will move adjacent to the word.  Otherwise, it will
 * skip over the word.
 *
 * @param forward   Whether to move the cursor forward.
 *
 * @return the index of the new cursor position
 */
int TextField::skipWord(bool forward) {
    int amt = (forward ? 1 : -1);
    
    const char* textData = _text.data();
    int p = (int) _text.length() - _cursorIndex - (forward ? 0 : 1);
    bool shift = (p < _text.length() && *(textData + p) == ' ');
    while (p < _text.length() && *(textData + p) == ' ') {
        p += amt;
    }
    while (!shift && p < _text.length() && *(textData + p) != ' ') {
        p += amt;
    }
    while (!shift && p < _text.length() && *(textData + p) == ' ') {
        p += amt;
    }
    
    return (forward ? p : p+1);
}

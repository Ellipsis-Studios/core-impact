//
//  CIInputController.h
//  CoreImpact
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "CIInputController.h"

using namespace cugl;

#define RANGE_CLAMP(x,y,z)  (x < y ? y : (x > z ? z : x))

#pragma mark -
#pragma mark Input Factors

/** The key for the event handlers */
#define LISTENER_KEY                1
/** How far we must swipe up for a move gesture */
#define SWIPE_LENGTH                10
/** Id to use with mouse events */
#define MOUSE_ID                    0

#pragma mark -
#pragma mark Input
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() {}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    clear();
#ifndef CU_TOUCH_SCREEN
    if (_mouse == nullptr) {
        return;
    }
    _mouse->removePressListener(LISTENER_KEY);
    _mouse->removeMotionListener(LISTENER_KEY);
    _mouse->removeReleaseListener(LISTENER_KEY);
#else
    if (_touch == nullptr) {
        return;
    }
    _touch->removeBeginListener(LISTENER_KEY);
    _touch->removeMotionListener(LISTENER_KEY);
    _touch->removeEndListener(LISTENER_KEY);
#endif
}

/**
 * Initializes the input control for the given drawing scale.
 *
 * This method works like a proper constructor, initializing the input
 * controller and allocating memory.  However, it still does not activate
 * the listeners.  You must call start() do that.
 *
 * @param bounds    the scene graph bounds
 * 
 * @return true if the controller was initialized successfully
 */
bool InputController::init(const Rect bounds) {
    bool success = true;
    
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    clear();
    
// Only process keyboard on desktop
#ifndef CU_TOUCH_SCREEN
    _mouse = Input::get<Mouse>();
    _mouse->setPointerAwareness(Mouse::PointerAwareness::ALWAYS);
    _mouse->addPressListener(LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        this->mousePressedCB(event, clicks, focus);
    });
    _mouse->addMotionListener(LISTENER_KEY, [=](const MouseEvent& event, const Vec2 previous, bool focus) {
        this->mouseMovedCB(event, previous, focus);
    });
    _mouse->addReleaseListener(LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        this->mouseReleasedCB(event, clicks, focus);
    });
#else
    _touch = Input::get<Touchscreen>();
    _touch->addBeginListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    _touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
    _touch->addEndListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
#endif
    
    return success;
}

/**
 * Processes the currently cached inputs.
 *
 * This method is used to to poll the current input state.  This will poll the
 * keyboad and accelerometer.
 *
 * This method also gathers the delta difference in the touches. Depending on
 * the OS, we may see multiple updates of the same touch in a single animation
 * frame, so we need to accumulate all of the data together.
 */
void InputController::update(float dt) {
    //remove any touchInstances that have ended
    for (auto it = _touchInstances.cbegin(); it != _touchInstances.cend(); ) {
        if (!it->second.fingerDown) {
            Uint64 id = it->second.touchid;
            _touchIds.remove_if([id](Uint64 touchId){ return touchId == id; });
            _touchInstances.erase(it++);
        } else {
            ++it;
        }
    }
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    _touchIds.clear();
    _touchInstances.clear();
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 InputController::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 *
 */
void InputController::touchBeganCB(const TouchEvent& event, bool focus) {
    processBegan(event.position, event.touch);
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    processMoved(event.position, previous, event.touch);
}

/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    processEnded(event.touch);
}

/**
 * Callback for a mouse pressed event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::mousePressedCB(const MouseEvent& event, Uint8 clicks, bool focus) {
    processBegan(event.position, MOUSE_ID);
}

/**
 * Callback for a mouse moved event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::mouseMovedCB(const MouseEvent& event, const Vec2 previous, bool focus) {
    processMoved(event.position, previous, MOUSE_ID);
}

/**
 * Callback for a mouse released event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::mouseReleasedCB(const MouseEvent& event, Uint8 clicks, bool focus) {
    processEnded(MOUSE_ID);
}

/**
 * Process the start of a touch or click
 *
 * @param pos The screen coordinates of the touch or click
 * @param id The id of the touch or click to start
 */
void InputController::processBegan(cugl::Vec2 pos, Uint64 id) {
    if (_touchInstances.find(id) == _touchInstances.end()) {
        TouchInstance newTouch;
        newTouch.fingerDown = true;
        newTouch.position = touch2Screen(pos);
        newTouch.velocity = Vec2();
        newTouch.timestamp.mark();
        newTouch.touchid = id;
        
        _touchIds.push_back(id);
        _touchInstances.insert(std::pair<Uint64, TouchInstance>(id, newTouch));
    }
}

/**
 * Process movement during a touch or click
 *
 * @param pos The screen coordinates of the touch or click
 * @param prev The screen coordinates of the previous touch or click
 * @param id The id of the touch of click to move
 */
void InputController::processMoved(cugl::Vec2 pos, cugl::Vec2 prev, Uint64 id) {
    if (_touchInstances.find(id) != _touchInstances.end()) {
        std::map<Uint64, TouchInstance>::iterator it = _touchInstances.find(id);
        Vec2 scenePos = touch2Screen(pos);
        it->second.velocity = scenePos - touch2Screen(prev);
        it->second.position = scenePos;
    }
}

/**
 * Process the end of a touch or click
 *
 * @param id The id of the touch of click to end
 */
void InputController::processEnded(Uint64 id) {
    if (_touchInstances.find(id) != _touchInstances.end()) {
        std::map<Uint64, TouchInstance>::iterator it = _touchInstances.find(id);
        it->second.fingerDown = false;
    }
}

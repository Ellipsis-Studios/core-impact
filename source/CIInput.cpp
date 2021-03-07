//
//  SDInput.h
//  Ship Demo
//
//  This input controller is primarily designed for keyboard control.  On mobile
//  you will notice that we use gestures to emulate keyboard commands. They even
//  use the same variables (though we need other variables for internal keyboard
//  emulation).  This simplifies our design quite a bit.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include "CIInput.h"

using namespace cugl;

#define RANGE_CLAMP(x,y,z)  (x < y ? y : (x > z ? z : x))

#pragma mark -
#pragma mark Input Factors

/** The key for the event handlers */
#define LISTENER_KEY                1
/** How far we must swipe up for a move gesture */
#define SWIPE_LENGTH                10

#pragma mark -
#pragma mark Ship Input
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
ShipInput::ShipInput() :
_fingerDown(false)
{
    
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void ShipInput::dispose() {
#ifndef CU_TOUCH_SCREEN
    if (_mouse == nullptr) {
        return;
    }
    _mouse->removePressListener(LISTENER_KEY);
    _mouse->removeReleaseListener(LISTENER_KEY);
#else
    if (_touch == nullptr) {
        return;
    }
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
 * @return true if the controller was initialized successfully
 */
bool ShipInput::init() {
    bool success = true;
    
// Only process keyboard on desktop
#ifndef CU_TOUCH_SCREEN
    _mouse = Input::get<Mouse>();
    _mouse->addPressListener(LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        this->mousePressedCB( event, clicks, focus);
    });
    _mouse->addReleaseListener(LISTENER_KEY, [=](const MouseEvent& event, Uint8 clicks, bool focus) {
        this->mouseReleasedCB( event, clicks, focus);
    });
#else
    _touch = Input::get<Touchscreen>();
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
void ShipInput::update(float dt) {
#ifdef CU_MOBILE
    // TODO: use touchDown() with finger id
    _fingerDown = _touch->touchCount() != 0;
#endif
    
    // fingerdown for the mouse controls if used to signal the user has finished a mouse movement
    if (_fingerDown) {
        Vec2 finishTouch = _swipeStart - _swipeEnd;
        
        _position = _swipeStart;
//        cout << "Finger Pos - (" << _position.x << ", " << _position.y << ")\n";
        _velocity = finishTouch;
        
        _swipeStart.setZero();
        _swipeEnd.setZero();
        _fingerDown = false;
    } else {
        _position = Vec2::ZERO;
        _velocity = Vec2::ZERO;
    }
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void ShipInput::clear() {
    
    _position = Vec2::ZERO;
    _velocity = Vec2::ZERO;
    
    _swipeStart = Vec2::ZERO;
    _swipeEnd = Vec2::ZERO;
}

#pragma mark -
#pragma mark Touch Callbacks
/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void ShipInput::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    _swipeStart.set(previous);
    _swipeEnd.set(event.position);
}

/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void ShipInput::touchEndedCB(const TouchEvent& event, bool focus) {
    _fingerDown = false;
}

/**
 * Callback for a mouse pressed event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void ShipInput::mousePressedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    _swipeStart.set(event.position);
}

/**
 * Callback for a mouse released event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void ShipInput::mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus) {
    _swipeEnd.set(event.position);
    
    _fingerDown = true;
}

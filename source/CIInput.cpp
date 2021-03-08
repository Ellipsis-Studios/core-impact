//
//  CIInput.h
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
#pragma mark Input
/**
 * Creates a new input controller.
 *
 * This constructor does NOT do any initialzation.  It simply allocates the
 * object. This makes it safe to use this class without a pointer.
 */
InputController::InputController() :
_fingerDown(false)
{
    
}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
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
    clearTouchInstance(_touchInstance);
    
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
#ifdef CU_MOBILE
    // TODO: use touchDown() with finger id
#endif
    if (_fingerDown) {
        _prevPosition = _position;
        _prevVelocity = _velocity;
    }
}

/**
 * Clears any buffered inputs so that we may start fresh.
 */
void InputController::clear() {
    _position = Vec2::ZERO;
    _velocity = Vec2::ZERO;
    _prevPosition = Vec2::ZERO;
    _prevVelocity = Vec2::ZERO;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void InputController::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
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
    Vec2 pos = event.position;
    if (_touchInstance.touchids.empty()) {
        _touchInstance.position = pos;
        _touchInstance.timestamp.mark();
        _touchInstance.touchids.insert(event.touch);

        _fingerDown = true;
        _position = touch2Screen(pos);
    }
}

/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    if (_touchInstance.touchids.find(event.touch) != _touchInstance.touchids.end()) {
        Vec2 pos = event.position;
        _velocity = touch2Screen(pos - previous);
        _position = touch2Screen(pos);
    }
}

/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    if (_touchInstance.touchids.find(event.touch) != _touchInstance.touchids.end()) {
        _touchInstance.touchids.clear();
        _fingerDown = false;
        _position = Vec2::ZERO;
        _velocity = Vec2::ZERO;
    }
}

/**
 * Callback for a mouse pressed event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::mousePressedCB(const MouseEvent& event, Uint8 clicks, bool focus) {
    _fingerDown = true;
    _position = touch2Screen(event.position);
}

/**
 * Callback for a mouse moved event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::mouseMovedCB(const MouseEvent& event, const Vec2 previous, bool focus) {
    if (_fingerDown) {
        Vec2 pos = event.position;
        _velocity = touch2Screen(pos - previous);
        _position = touch2Screen(pos);
    }
}

/**
 * Callback for a mouse released event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::mouseReleasedCB(const MouseEvent& event, Uint8 clicks, bool focus) {
    _fingerDown = false;
    _position = Vec2::ZERO;
    _velocity = Vec2::ZERO;
}

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
#ifndef __SD_INPUT_H__
#define __SD_INPUT_H__
#include <cugl/cugl.h>

/**
 * This class represents player input in the ship demo.
 *
 * This input handler uses the CUGL input API.  It uses the polling API for
 * keyboard, but the callback API for touch.  This demonstrates a mix of ways
 * to handle input, and the reason for hiding it behind an abstraction like
 * this class.
 *
 * Unlike CUGL input devices, this class is not a singleton.  It must be 
 * allocated before use.  However, you will notice that we do not do any
 * input initialization in the constructor.  This allows us to allocate this 
 * controller as a field without using pointers. We simply add the class to the 
 * header file of its owner, and delay initialization (via the method init()) 
 * until later. This is one of the main reasons we like to avoid initialization 
 * in the constructor.
 */
class ShipInput {
private:
    
    /** Postion and velocity of a completed swipe. To be used in CollisionController to check if a dot was swiped. */
    cugl::Vec2 _position;
    cugl::Vec2 _velocity;
    
    // TOUCH SUPPORT
    /** The swipe location for the current gesture */
    cugl::Vec2 _swipeStart;
    cugl::Vec2 _swipeEnd;
    
    /** Flag that tells whether the user's finger is currently down or not */
    bool _fingerDown;
    
    cugl::Touchscreen* _touch;
    cugl::Mouse* _mouse;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    ShipInput(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~ShipInput() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Initializes the input control
     *
     * This method works like a proper constructor, initializing the input
     * controller, allocating memory and attaching listeners.
     *
     * @return true if the controller was initialized successfully
     */
    bool init();
    
    
#pragma mark -
#pragma mark Input Detection
    
    /**
     * Processes the currently cached inputs.
     *
     * This method is used to to poll the current input state.  This will poll 
     * the keyboad and accelerometer.
     * 
     * This method also gathers the delta difference in the touches. Depending 
     * on the OS, we may see multiple updates of the same touch in a single 
     * animation frame, so we need to accumulate all of the data together.
     */
    void update(float dt);
    
    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();

    
#pragma mark -
#pragma mark Input Results
    /**
     * Returns the position of a user swipe.
     *
     * @return position of a user swipe
     */
    cugl::Vec2 getPosition() {
        return _position;
    }
    
    /**
     * Returns the velocity of a user swipe
     *
     * @return velocity of a user swipe
     */
    cugl::Vec2 getVelocity() {
        return _velocity;
    }
    
    /**
     * Returns whether the user's finger is down or not
     *
     * @return whether the user's finger is down or not
     */
    bool fingerDown() {
        return _fingerDown;
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
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
    
    /**
     * Callback for a touch ended event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
    
    /**
     * Callback for a mouse pressed event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void mousePressedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
    
    /**
     * Callback for a mouse released event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
};

#endif /* __SD_INPUT_H__ */

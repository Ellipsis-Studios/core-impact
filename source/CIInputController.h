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
#ifndef __CI_INPUT_CONTROLLER_H__
#define __CI_INPUT_CONTROLLER_H__
#include <cugl/cugl.h>
#include <map>
#include "CITouchInstance.h" 

/**
 * This class represents player input
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
class InputController {
private:
    
    /** Pointers to the touchscreen and mouse */
    cugl::Touchscreen* _touch;
    cugl::Mouse* _mouse;
    
    /** All active touch instances */
    std::map<Uint64, TouchInstance> _touchInstances;
    
    /** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
    cugl::Rect _sbounds;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new input controller.
     *
     * This constructor does NOT do any initialzation.  It simply allocates the
     * object. This makes it safe to use this class without a pointer.
     */
    InputController(); // Don't initialize.  Allow stack based
    
    /**
     * Disposes of this input controller, releasing all listeners.
     */
    ~InputController() { dispose(); }
    
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
     * @param bounds    the scene graph bounds
     * @return true if the controller was initialized successfully
     */
    bool init(const cugl::Rect bounds);
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;
    
    
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
     * Returns a pointer to the active touch instances
     *
     * @return a pointer to the touch instances
     */
    std::map<Uint64, TouchInstance>* getTouchInstances() {
        return &_touchInstances;
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
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);
    
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
     * Callback for a mouse moved event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void mouseMovedCB(const cugl::MouseEvent& event, const cugl::Vec2 previous, bool focus);
    
    /**
     * Callback for a mouse released event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void mouseReleasedCB(const cugl::MouseEvent& event, Uint8 clicks, bool focus);
    
    /**
     * Process the start of a touch or click
     *
     * @param pos The screen coordinates of the touch or click
     * @param id The id of the touch or click to start
     */
    void processBegan(cugl::Vec2 pos, Uint64 id);
    
    /**
     * Process movement during a touch or click
     *
     * @param pos The screen coordinates of the touch or click
     * @param prev The screen coordinates of the previous touch or click
     * @param id The id of the touch of click to move
     */
    void processMoved(cugl::Vec2 pos, cugl::Vec2 prev, Uint64 id);
    
    /**
     * Process the end of a touch or click
     *
     * @param id The id of the touch of click to end
     */
    void processEnded(Uint64 id);
};

#endif /* __CI_INPUT_CONTROLLER_H__ */

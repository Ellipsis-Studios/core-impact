//
//  CINetworkMessageManager.h
//  CoreImpact
//
// This class will be used for send a receiving messages on the network.
//
//  Created by William Long on 3/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_NETWORK_MESSAGE_MANAGER_H__
#define __CI_NETWORK_MESSAGE_MANAGER_H__

#include "CIGameUpdateManager.h"

class NetworkMessageManager {
private:
    /** Pointer to the game update manager class */
    std::shared_ptr<GameUpdateManager> _gameUpdateManager;
    
    /** The timestamp of the next message to send. */
    int _timestamp;
    
    // TODO: need a reference to cugl-net network connection
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new network message manager
     */
    NetworkMessageManager() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this network message manager.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~NetworkMessageManager() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this network message manager.
     */
    void dispose();
    
    /**
     * Initializes the network message manager
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @return true if the game update manager is initialized properly, false otherwise.
     */
    bool init();
    
    /**
     * Returns a newly allocated network message manager.
     *
     * @return a newly allocated game update manager.
     */
    static std::shared_ptr<NetworkMessageManager> alloc() {
        std::shared_ptr<NetworkMessageManager> result = std::make_shared<NetworkMessageManager>();
        return (result->init() ? result : nullptr);
    }

#pragma mark Interactions
    /**
     * Sends messages from the game update manager to other players over the network.
     */
    void sendMessages();
    
    /**
     * Receives messages sent over the network and adds them to the queue in game update manager.
     */
    void receiveMessages();
    
};

#endif /* __CI_NETWORK_MESSAGE_MANAGER_H__ */


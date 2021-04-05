//
//  CIJoinMenu.h
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_JOIN_MENU_H__
#define __CI_JOIN_MENU_H__
#include <cugl/cugl.h>
#include  "CIMenuStatus.h"


class JoinMenu {
private:
    /** The asset manager for menu screens. */
    std::shared_ptr<cugl::AssetManager> _assets;

    // JOIN GAME Screen Layer
    std::shared_ptr<cugl::scene2::SceneNode> _layer;
    /** Game room id input */
    std::shared_ptr<cugl::scene2::TextField> _roomIdInput;
    /** Button to join game with specified id */
    std::shared_ptr<cugl::scene2::Button> _roomJoinBtn;
    
    /** Stores the game code for joining as client*/
    string _joinGame;
    
    bool _pressedJoin;
        
public:
#pragma mark -
#pragma mark Constructors
    JoinMenu() : _pressedJoin(false) {}
    
    ~JoinMenu() { dispose(); }
    
    void dispose();

    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    static std::shared_ptr<JoinMenu> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<JoinMenu> result = std::make_shared<JoinMenu>();
        return (result->init(assets) ? result : nullptr);
    }
    
    void setDisplay(bool value);

    const std::shared_ptr<cugl::scene2::SceneNode>& getLayer() const {
        return _layer;
    }
    
    std::string getRoomId() {
        return _joinGame;
    }
    
    bool getPressedJoin() {
        return _pressedJoin;
    }
    
    void update(float timestep) {
        if (_roomIdInput->isActive()) {
            _joinGame = _roomIdInput->getText();
        }
    }
    
    void resetPress() {
        _pressedJoin = false;
    }

};

#endif /* __CI_JOIN_MENU_H__ */

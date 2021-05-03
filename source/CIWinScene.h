//
//  CIWinScene.h
//  CoreImpact
//
//  Class containing UI elements that get displayed after a game has ended.
//
//  Created by William Long on 4/17/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_WIN_SCENE_H__
#define __CI_WIN_SCENE_H__

#include <cugl/cugl.h>

class WinScene {
private:
    /** Label that displays the outcome of the game. */
    std::shared_ptr<cugl::scene2::Label> _gameOutcomeLabel;
    /** Button that brings a player back to the home screen. */
    std::shared_ptr<cugl::scene2::Button> _backToHomeButton;
    /** Button that starts a new game. */
    std::shared_ptr<cugl::scene2::Button> _newGameButton;
    
    /** Flag variable to indicated whether to bring the user back to the home screen. */
    bool _goBackToHome;
    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new win scene
     *
     * This constructor does not allocate any objects or start the game.
     * This allows us to use the object without a heap pointer.
     */
    WinScene() {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
    */
    ~WinScene() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this win scene.
     */
    void dispose();

    /**
     * Initializes a new win scene.
     *
     * @param assets    The (loaded) assets for this win scene
     * @param dimen      The size of the screen
     *
     * @return true if initialization was successful, false otherwise
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen);


    /**
     * Returns a newly allocated Win Scene
     *
     * @param assets    The (loaded) assets for this win scene
     * @param dimen      The size of the screen
     *
     * @return a newly allocated main menu
     */
    static std::shared_ptr<WinScene> alloc(const std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen) {
        std::shared_ptr<WinScene> result = std::make_shared<WinScene>();
        return (result->init(assets, dimen) ? result : nullptr);
    }

#pragma mark -
#pragma mark Menu Monitoring
    // TODO: switch this to use player names
    /**
     * Sets the winner of the game.
     *
     * @param winnerPlayerId The player id of the player who won the game
     * @param playerId               The player id of the player playing on this device
     */
    void setWinner(int winnerPlayerId, int playerId, std::string winningPlayer);
    
    /**
     * Sets whether the win scene is currently active and visible.
     *
     * @param onDisplay     Whether the win scene is currently active and visible
     */
    void setDisplay(bool onDisplay);
    
    /**
     * Returns whether the win screen display is active.
     *
     * @return whether the win screen display is active
     */
    bool displayActive() {
        return _gameOutcomeLabel->isVisible();
    }
    
    /**
     * Returns whether the user has clicked the go back to home button.
     *
     * @return whether the user has clicked the go back to home button
     */
    bool goBackToHome() {
        return _goBackToHome;
    }
};

#endif /* __CI_WIN_SCENE_H__ */

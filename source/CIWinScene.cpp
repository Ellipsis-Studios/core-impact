//
//  CIWinScene.cpp
//  CoreImpact
//
//  Class containing UI elements that get displayed after a game has ended.
//
//  Created by William Long on 4/17/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIWinScene.h"

/**
 * Disposes of all (non-static) resources allocated to this win scene.
 */
void WinScene::dispose() {
    if (_backToHomeButton != nullptr && _backToHomeButton->isActive()) {
        _backToHomeButton->deactivate();
        _newGameButton->deactivate();
    }
    
    _gameOutcomeLabel = nullptr;
    _backToHomeButton = nullptr;
    _newGameButton = nullptr;
    _goBackToHome = false;
}

/**
 * Initializes a new win scene.
 *
 * @param assets    The (loaded) assets for this win scene
 *
 * @return true if initialization was successful, false otherwise
 */
bool WinScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    _gameOutcomeLabel = std::dynamic_pointer_cast<cugl::scene2::Label>(assets->get<cugl::scene2::SceneNode>("game_win_gameOutcomeLabel"));
    _backToHomeButton = std::dynamic_pointer_cast<cugl::scene2::Button>(assets->get<cugl::scene2::SceneNode>("game_win_backToHomeButton"));
    _backToHomeButton->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _goBackToHome = true;
        }
    });
    
    // TODO: change this listener
    _newGameButton = std::dynamic_pointer_cast<cugl::scene2::Button>(assets->get<cugl::scene2::SceneNode>("game_win_newGameButton"));
    _newGameButton->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _goBackToHome = true;
        }
    });
    _goBackToHome = false;
    return true;
}

/**
 * Sets the winner of the game.
 *
 * @param winnerPlayerId The player id of the player who won the game
 * @param playerId               The player id of the player playing on this device
 */
void WinScene::setWinner(int winnerPlayerId, int playerId) {
    if (winnerPlayerId == playerId) {
        _gameOutcomeLabel->setText("Congratulations! You won the game!");
    } else {
        _gameOutcomeLabel->setText("Sorry! Player " + std::to_string(playerId) + " won the game.");
    }
}

/**
 * Sets whether the win scene is currently active and visible.
 *
 * @param onDisplay     Whether the win scene is currently active and visible
 */
void WinScene::setDisplay(bool onDisplay) {
    _gameOutcomeLabel->setVisible(onDisplay);
    _backToHomeButton->setVisible(onDisplay);
    _newGameButton->setVisible(onDisplay);
    
    if (onDisplay) {
        _backToHomeButton->activate();
        _newGameButton->activate();
    } else {
        _backToHomeButton->deactivate();
        _newGameButton->deactivate();
    }
}

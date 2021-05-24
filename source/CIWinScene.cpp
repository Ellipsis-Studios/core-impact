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
    } else if (_backToHomeButton != nullptr) {
        _backToHomeButton->clearListeners();
        _newGameButton->clearListeners();
    }
    
    _gameOutcomeLabel = nullptr;
    _backToHomeButton = nullptr;
    _newGameButton = nullptr;
    _flareExplosion = nullptr;
    _goBackToHome = false;
}

/**
 * Initializes a new win scene.
 *
 * @param assets    The (loaded) assets for this win scene
 * @param dimen      The size of the screen
 *
 * @return true if initialization was successful, false otherwise
 */
bool WinScene::init(const std::shared_ptr<cugl::AssetManager>& assets, cugl::Size dimen) {
    auto win = assets->get<cugl::scene2::SceneNode>("win");
    _layer = win;
    win->setContentSize(dimen);
    win->doLayout();
    
    _gameOutcomeLabel = std::dynamic_pointer_cast<cugl::scene2::Label>(assets->get<cugl::scene2::SceneNode>("win_gameOutcomeLabel"));
    _backToHomeButton = std::dynamic_pointer_cast<cugl::scene2::Button>(assets->get<cugl::scene2::SceneNode>("win_backToHomeButton"));
    _flareExplosion = std::dynamic_pointer_cast<cugl::scene2::SceneNode>(assets->get<cugl::scene2::SceneNode>("win_flare"));
    _backToHomeButton->addListener([&](const std::string& name, bool down) {
        if (!down) {
            _goBackToHome = true;
        }
    });
    
    // TODO: change this listener
    _newGameButton = std::dynamic_pointer_cast<cugl::scene2::Button>(assets->get<cugl::scene2::SceneNode>("win_newGameButton"));
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
 * @param winningPlayer    The name of the player who won
 */
void WinScene::setWinner(int winnerPlayerId, int playerId, std::string winningPlayer) {
    if (winnerPlayerId == playerId) {
        _gameOutcomeLabel->setText("Congratulations! You won the game!");
    } else if (winnerPlayerId == -2) {
        _gameOutcomeLabel->setText("Host disconnected! Please start a new game!");
    } else if (winnerPlayerId == -3) {
        _gameOutcomeLabel->setText("Disconnected from game! Please check network connection!");
    } else {
        _gameOutcomeLabel->setText("Sorry! " + winningPlayer + " won the game!");
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

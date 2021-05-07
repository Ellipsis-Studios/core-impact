//
//  CIJoinMenu.cpp
//  CoreImpact
//
//  Created by Richard Yoon on 4/5/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CIJoinMenu.h"

using namespace cugl;

#pragma mark -
#pragma mark Constructors

/**
 * Disposes of all (non-static) resources allocated to this menu.
 */
void JoinMenu::dispose() {
    if (_numberpad1 != nullptr && _numberpad1->isActive()) {
        _numberpad1->deactivate();
        _numberpad2->deactivate();
        _numberpad3->deactivate();
        _numberpad4->deactivate();
        _numberpad5->deactivate();
        _numberpad6->deactivate();
        _numberpad7->deactivate();
        _numberpad8->deactivate();
        _numberpad9->deactivate();
        _numberpaddel->deactivate();
        _numberpad0->deactivate();
        _numberpadjoin->deactivate();
    }
    _numberpad1 = nullptr;
    _numberpad2 = nullptr;
    _numberpad3 = nullptr;
    _numberpad4 = nullptr;
    _numberpad5 = nullptr;
    _numberpad6 = nullptr;
    _numberpad7 = nullptr;
    _numberpad8 = nullptr;
    _numberpad9 = nullptr;
    _numberpaddel = nullptr;
    _numberpad0 = nullptr;
    _numberpadjoin = nullptr;

    _roomIdLabelNode1 = nullptr;
    _roomIdLabelNode2 = nullptr;
    _roomIdLabelNode3 = nullptr;
    _roomIdLabelNode4 = nullptr;
    _roomIdLabelNode5 = nullptr;

    _roomIdLabel1 = nullptr;
    _roomIdLabel2 = nullptr;
    _roomIdLabel3 = nullptr;
    _roomIdLabel4 = nullptr;
    _roomIdLabel5 = nullptr;

    _layer = nullptr;

    _roomIds.clear();
    _roomInd = 0;

    _nextState = MenuState::JoinRoom;
}

/**
 * Initializes a new join game menu with the state pointer.
 *
 * @param assets    The (loaded) assets for this join game menu
 * @param gameSettings      The settings for the current game
 *
 * @return true if initialization was successful, false otherwise
 */
bool JoinMenu::init(const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<GameSettings>& gameSettings) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    // Lock the scene to a reasonable resolution
    dimen *= CONSTANTS::SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }

    _layer = assets->get<scene2::SceneNode>("join");
    _layer->setContentSize(dimen);
    _layer->doLayout();

    _gameSettings = gameSettings;

    _roomIds.clear();
    _roomIds.push_back("");
    _roomIds.push_back("");
    _roomIds.push_back("");
    _roomIds.push_back("");
    _roomIds.push_back("");

    _roomInd = 0;

    // Room Id Labels
    _roomIdLabelNode1 = assets->get<scene2::SceneNode>("join_roomid1");
    _roomIdLabelNode2 = assets->get<scene2::SceneNode>("join_roomid2");
    _roomIdLabelNode3 = assets->get<scene2::SceneNode>("join_roomid3");
    _roomIdLabelNode4 = assets->get<scene2::SceneNode>("join_roomid4");
    _roomIdLabelNode5 = assets->get<scene2::SceneNode>("join_roomid5");

    _roomIdLabel1 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("join_roomid1_label"));
    _roomIdLabel2 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("join_roomid2_label"));
    _roomIdLabel3 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("join_roomid3_label"));
    _roomIdLabel4 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("join_roomid4_label"));
    _roomIdLabel5 = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>("join_roomid5_label"));

    // Number Pad
    _numberpad1 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad1"));
    _numberpad1->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "1";
                _roomInd++;
            }
        }
        });
    _numberpad2 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad2"));
    _numberpad2->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "2";
                _roomInd++;
            }
        }
        });
    _numberpad3 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad3"));
    _numberpad3->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "3";
                _roomInd++;
            }
        }
        });
    _numberpad4 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad4"));
    _numberpad4->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "4";
                _roomInd++;
            }
        }
        });
    _numberpad5 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad5"));
    _numberpad5->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "5";
                _roomInd++;
            }
        }
        });
    _numberpad6 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad6"));
    _numberpad6->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "6";
                _roomInd++;
            }
        }
        });
    _numberpad7 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad7"));
    _numberpad7->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "7";
                _roomInd++;
            }
        }
        });
    _numberpad8 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad8"));
    _numberpad8->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "8";
                _roomInd++;
            }
        }
        });
    _numberpad9 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad9"));
    _numberpad9->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "9";
                _roomInd++;
            }
        }
        });
    _numberpad0 = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpad0"));
    _numberpad0->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd < 5) {
                _roomIds[_roomInd] = "0";
                _roomInd++;
            }
        }
        });

    _numberpaddel = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpaddel"));
    _numberpaddel->addListener([&](const std::string& name, bool down) {
        if (!down) {
            if (_roomInd > 0) {
                _roomIds[(_roomInd - 1)] = "";
                _roomInd--;
            }
        }
        });
    _numberpadjoin = std::dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("join_numpadjoin"));
    _numberpadjoin->addListener([&](const std::string& name, bool down) {
        if (!down) {
            std::string nroomidstr;
            for (const string& d : _roomIds) {
                nroomidstr.append(d);
            }
            if (_gameSettings != nullptr) {
                _gameSettings->setGameId(nroomidstr);
            }
            //_nextState = MenuState::JoinToLobby;
            _nextState = MenuState::MenuToJoin;
        }
        });

    _nextState = MenuState::JoinRoom;
    return true;
}

#pragma mark -
#pragma mark Menu Monitoring
/**
 * Sets whether the join game menu is currently active and visible.
 *
 * @param onDisplay     Whether the join game menu is currently active and visible
 */
void JoinMenu::setDisplay(bool onDisplay) {
    if (_layer != nullptr) {
        _roomIdLabelNode1->setVisible(onDisplay);
        _roomIdLabelNode2->setVisible(onDisplay);
        _roomIdLabelNode3->setVisible(onDisplay);
        _roomIdLabelNode4->setVisible(onDisplay);
        _roomIdLabelNode5->setVisible(onDisplay);

        _numberpad1->setVisible(onDisplay);
        _numberpad2->setVisible(onDisplay);
        _numberpad3->setVisible(onDisplay);
        _numberpad4->setVisible(onDisplay);
        _numberpad5->setVisible(onDisplay);
        _numberpad6->setVisible(onDisplay);
        _numberpad7->setVisible(onDisplay);
        _numberpad8->setVisible(onDisplay);
        _numberpad9->setVisible(onDisplay);
        _numberpaddel->setVisible(onDisplay);
        _numberpad0->setVisible(onDisplay);
        _numberpadjoin->setVisible(onDisplay);

        _layer->setVisible(onDisplay);

        if (onDisplay) {
            _numberpad1->activate();
            _numberpad2->activate();
            _numberpad3->activate();
            _numberpad4->activate();
            _numberpad5->activate();
            _numberpad6->activate();
            _numberpad7->activate();
            _numberpad8->activate();
            _numberpad9->activate();
            _numberpaddel->activate();
            _numberpad0->activate();
            _numberpadjoin->activate();
        }
        else {
            _numberpad1->deactivate();
            _numberpad2->deactivate();
            _numberpad3->deactivate();
            _numberpad4->deactivate();
            _numberpad5->deactivate();
            _numberpad6->deactivate();
            _numberpad7->deactivate();
            _numberpad8->deactivate();
            _numberpad9->deactivate();
            _numberpaddel->deactivate();
            _numberpad0->deactivate();
            _numberpadjoin->deactivate();
        }
    }
}

/**
 * The method called to update this menu.
 *
 * The menu screen is only visible during the JoinRoom state. The
 * screen is put on display on states transitioning into JoinRoom.
 * Screen is taken down once menu state exits JoinRoom.
 */
void JoinMenu::update(MenuState& state) {
    if (_layer == nullptr) {
        return;
    }
    // handle JoinRoom menu
    switch (state)
    {
        case MenuState::NameToJoin:
        case MenuState::MainToJoin:
            // handle transitioning into JoinRoom
            setDisplay(true);
            state = MenuState::JoinRoom;
            _nextState = MenuState::JoinRoom;
            break;
        case MenuState::JoinRoom:
            // handle roomid input and transition out of JoinRoom
            _roomIdLabel1->setText(_roomIds[0]);
            _roomIdLabel2->setText(_roomIds[1]);
            _roomIdLabel3->setText(_roomIds[2]);
            _roomIdLabel4->setText(_roomIds[3]);
            _roomIdLabel5->setText(_roomIds[4]);

            // handle join button activation
            if (_roomInd == 5) {
                _numberpadjoin->setColor(cugl::Color4f::WHITE);
                _numberpadjoin->activate();
            }
            else {
                _numberpadjoin->setColor(cugl::Color4f::GRAY);
                _numberpadjoin->deactivate();
            }
            if (_numberpad1 != nullptr && !_numberpad1->isActive()) {
                _roomIds.clear();
                _roomInd = 0;
                _roomIds.push_back("");
                _roomIds.push_back("");
                _roomIds.push_back("");
                _roomIds.push_back("");
                _roomIds.push_back("");

                _numberpad1->activate();
                _numberpad2->activate();
                _numberpad3->activate();
                _numberpad4->activate();
                _numberpad5->activate();
                _numberpad6->activate();
                _numberpad7->activate();
                _numberpad8->activate();
                _numberpad9->activate();
                _numberpaddel->activate();
                _numberpad0->activate();
            }
            state = _nextState;
            break;
        case MenuState::MenuToJoin:
        case MenuState::JoiningGame:
            _numberpad1->deactivate();
            _numberpad2->deactivate();
            _numberpad3->deactivate();
            _numberpad4->deactivate();
            _numberpad5->deactivate();
            _numberpad6->deactivate();
            _numberpad7->deactivate();
            _numberpad8->deactivate();
            _numberpad9->deactivate();
            _numberpaddel->deactivate();
            _numberpad0->deactivate();
            _numberpadjoin->deactivate();
            break;
        default:
            // hide menu screen 
            if (_layer != nullptr && _layer->isVisible()) {
                _roomIds.clear();
                _roomInd = 0;
                _roomIds.push_back("");
                _roomIds.push_back("");
                _roomIds.push_back("");
                _roomIds.push_back("");
                _roomIds.push_back("");
                setDisplay(false);
            }
            break;
    }
}

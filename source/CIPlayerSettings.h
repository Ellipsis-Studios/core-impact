//
//  CIPlayerSettings.h
//  CoreImpact
//
//  This model class stores player settings that users can change from the menu scene
//
//  Created by Richard Yoon on 4/18/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLAYER_SETTINGS_H__
#define __CI_PLAYER_SETTINGS_H__
#include <cugl/cugl.h>
#include "CIGameConstants.h"

/** 
 * This model class represents the player settings that user can change from menu scene and save to local files
 * for subsequent game instances.
 */
class PlayerSettings {
private:
    /** Value for the player name */
    std::string _playerName;
    /** Value for the game audio volume */
    float _volume;
    /** Whether game music is turned on/off */
    bool _musicOn;
    /** Whether player is new */
    bool _isNew;

public:
#pragma mark Constructors
    /**
     * Initializes a new player settings class with default settings
     *
     * @return true if the initialization was successful
     */
    bool init() {
        _playerName = CONSTANTS::DEFAULT_PLAYER_NAME;
        _volume = CONSTANTS::DEFAULT_VOLUME;
        _musicOn = CONSTANTS::DEFAULT_MUSIC_ON;
        _isNew = false;
        return true;
    }

    /**
     * Returns a newly allocated player settings class with default settings
     *
     * @return a newly allocated player settings class.
     */
    static std::shared_ptr<PlayerSettings> alloc() {
        std::shared_ptr<PlayerSettings> result = std::make_shared<PlayerSettings>();
        return (result->init() ? result : nullptr);
    }

#pragma mark Properties
    /** 
     * Appends the player setting values to the given json value.
     */
    void appendSettings(std::shared_ptr<cugl::JsonValue>& playerSettings) {
        playerSettings->appendValue("PlayerName", _playerName);
        playerSettings->appendValue("Volume", _volume);
        playerSettings->appendValue("MusicOn", _musicOn);
    }

    /**
     * Sets the values of player settings from a json value.
     */
    void setPlayerSettings(const std::shared_ptr<cugl::JsonValue>& playerSettings) {
        _playerName = CONSTANTS::DEFAULT_PLAYER_NAME;
        _volume = CONSTANTS::DEFAULT_VOLUME;
        _musicOn = CONSTANTS::DEFAULT_MUSIC_ON;

        if (playerSettings != nullptr) {
            string pname = playerSettings->getString("PlayerName", "Player Name");
            if (!pname.empty()) {
                _playerName = pname;
            }
            _volume = playerSettings->getFloat("Volume", 0.5f);
            _musicOn = playerSettings->getBool("MusicOn", true);
        }
    }

    /** 
     * Returns the player name value.
     */
    std::string getPlayerName() const {
        return _playerName;
    }

    /** 
     * Sets the player name setting value.
     */
    void setPlayerName(std::string playerName) {
        _playerName = playerName;
    }

    /** 
     * Returns the volume setting value.
     */
    float getVolume() const {
        return _volume;
    }

    /**
     * Sets the volume setting value.
     */
    void setVolume(float volume) {
        _volume = volume;
    }

    /** 
     * Returns the music toggle setting value.
     */
    bool getMusicOn() const {
        return _musicOn;
    }

    /**
     * Sets the music toggle setting value.
     */
    void setMusicOn(bool musicOn) {
        _musicOn = musicOn;
    }

    /**
     * Sets whether the player is new.
     */
    void setIsNew(bool isNew) {
        _isNew = isNew;
    }

    /**
     * Returns whether the player is new.
     */
    bool getIsNew() const {
        return _isNew;
    }

    /**
     * Returns whether the player is new or player name is invalid.
     * 
     * Used to check player name when transitioning from main menu screen to
     * the join or create menu screens. 
     */
    bool getSkipNameMenu() const {
        return !(_isNew || _playerName.empty() || _playerName.size() > 12);
    }

};

#endif /* __CI_PLAYER_SETTINGS_H__ */

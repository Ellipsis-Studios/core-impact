//
//  CIGameSettings.h
//  CoreImpact
//
//  This model class stores game settings that users can change from the menu scene
//
//  Created by William Long on 4/15/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_GAME_SETTINGS_H__
#define __CI_GAME_SETTINGS_H__
#include <cugl/cugl.h>
#include "CIGameConstants.h"

/** 
 * This model class represents the game settings that user can change from game lobby menu screen.
 */
class GameSettings {
private:
    /** The game id of the current game */
    std::string _gameId;
    
    /** The stardust spawning rate */
    float _spawnRate;
    
    /** The strength of gravity */
    float _gravStrength;
    
    /** The number of colors in the game */
    uint8_t _colorCount;
    
    /** The amount of planet mass needed to win the game */
    uint16_t _planetMassToWin;
    
public:
#pragma mark Constructors
    /**
     * Initializes a new game settings class with default settings
     *
     * @return true if the initialization was successful
     */
    bool init() {
        reset();
        return true;
    }

    /**
     * Returns a newly allocated game settings class with default settings
     *
     * @return a newly allocated game settings class.
     */
    static std::shared_ptr<GameSettings> alloc() {
        std::shared_ptr<GameSettings> result = std::make_shared<GameSettings>();
        return (result->init() ? result : nullptr);
    }

#pragma mark Properties
    /**
     * Resets the setting values to default. 
     */
    void reset() {
        using namespace constants::gamesettings;

        _gameId = DEFAULT_GAME_ID;
        _spawnRate = DEFAULT_SPAWN_RATE;
        _gravStrength = DEFAULT_GRAV_STRENGTH;
        _colorCount = DEFAULT_COLOR_COUNT;
        _planetMassToWin = DEFAULT_WIN_MASS;
    }

    /** 
     * Returns the game room id value.
     */
    std::string getGameId() const {
        return _gameId;
    }
    
    /** 
     * Sets the game room id value.
     */
    void setGameId(std::string gameId) {
        _gameId = gameId;
    }
    
    /** 
     * Returns the stardust spawn rate value 
     */
    float getSpawnRate() const {
        return _spawnRate;
    }
    
    /**
     * Sets the stardust spawn rate value.
     */
    void setSpawnRate(float spawnRate) {
        _spawnRate = spawnRate;
    }
    
    /** 
     * Returns the planet gravity strength value.
     */
    float getGravStrength() const {
        return _gravStrength;
    }
    
    /** 
     * Sets the planet gravity strength value.
     */
    void setGravStrength(float gravStrength) {
        _gravStrength = gravStrength;
    }
    
    /** 
     * Returns the stardust color count value.
     */
    uint8_t getColorCount() const {
        return _colorCount;
    }
    
    /**
     * Sets the stardust color count value.
     */
    void setColorCount(uint8_t colorCount) {
        _colorCount = colorCount;
    }
    
    /**
     * Returns the planet mass required to win the game.
     */
    uint16_t getPlanetMassToWin() const {
        return _planetMassToWin;
    }
    
    /** 
     * Sets the planet mass required to win the game.
     */
    void setPlanetMassToWin(uint16_t planetMassToWin) {
        _planetMassToWin = planetMassToWin;
    }
    
};

#endif /* __CI_GAME_SETTINGS_H__ */

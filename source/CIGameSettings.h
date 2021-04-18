//
//  CIGameSettings.h
//  CoreImpact
//
// This model class stores game settings that users can change from the menu scene
//
//  Created by William Long on 4/15/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_GAME_SETTINGS_H__
#define __CI_GAME_SETTINGS_H__

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
        _gameId = "";
        _spawnRate = 1.0f;
        _gravStrength = 1.0f;
        _colorCount = 4;
        _planetMassToWin = 200;
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
    std::string getGameId() {
        return _gameId;
    }
    
    void setGameId(std::string gameId) {
        _gameId = gameId;
    }
    
    float getSpawnRate() {
        return _spawnRate;
    }
    
    void setSpawnRate(float spawnRate) {
        _spawnRate = spawnRate;
    }
    
    float getGravStrength() {
        return _gravStrength;
    }
    
    void setGravStrength(float gravStrength) {
        _gravStrength = gravStrength;
    }
    
    uint8_t getColorCount() {
        return _colorCount;
    }
    
    void setColorCount(uint8_t colorCount) {
        _colorCount = colorCount;
    }
    
    uint16_t getPlanetMassToWin() {
        return _planetMassToWin;
    }
    
    void setPlanetMassToWin(uint16_t planetMassToWin) {
        _planetMassToWin = planetMassToWin;
    }
    
};

#endif /* __CI_GAME_SETTINGS_H__ */

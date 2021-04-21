//
//  CIOpponentNode.h
//  CoreImpact
//
//  This class is used to draw the opponent's progress to the screen.
//
//  Created by Brandon Stein on 4/2/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_OPPONENT_NODE_H__
#define __CI_OPPONENT_NODE_H__
#include <cugl/cugl.h>
#include <cmath>
#include "CIColor.h"
#include "CILocation.h"

#define PROGRESS_ROWS           14
#define PROGRESS_COLS           15
#define PROGRESS_NORMAL_END    119
#define PROGRESS_NORMAL_START    0
#define PROGRESS_FLASH_END     209
#define PROGRESS_FLASH_START   120

/** The x offset of the nametag from the corner of the screen */
#define NAMETAG_X_OFFSET 45
/** The y offset of the nametag from the corner of the screen */
#define NAMETAG_Y_OFFSET 80

class OpponentNode : public cugl::scene2::AnimationNode {
private:
    /** Graphic asset used to display progress */
    std::shared_ptr<cugl::Texture> _texture;
    /** Graphic asset used to display the fog power up */
    std::shared_ptr<cugl::Texture> _fogTexture;
    /** Progress of the opponent towards winning, between 0 and 1 */
    float _progress;
    /** The maximum width of the progress bar */
    float _maxwidth;
    /** The maximum height of the progress bar */
    float _maxheight;
    /** The corner that this opponent node is in */
    CILocation::Value _location;
    /** The Label used to display the player name of this oppoent */
    std::shared_ptr<cugl::scene2::Label> _nameLabel;
    
    /** The amount of time since last animation frame change */
    float _timeElapsed;
    
    /** The amount of time the full fog texture has be on the screen */
    float _fogTimeOnScreen;
    /** How many frames the fog animation has been going for. */
    int _fogAnimationProgress;
    /** Whether or not the fog power up is still on going */
    bool _fogOngoing;
    /** Bezier easing function for the fog animation */
    std::shared_ptr<cugl::EasingBezier> _fogAnimationEasingFunction;
    
    /**
     * Helper function to get the x and y reflection of the progress bar
     *
     * @param location The location of this opponent node
     */
    cugl::Vec2 getReflectFromLocation(CILocation::Value location) {
        switch (location) {
            case CILocation::Value::TOP_LEFT:
                return cugl::Vec2(1, -1);
                break;
            case CILocation::Value::TOP_RIGHT:
                return cugl::Vec2(-1, -1);
                break;
            case CILocation::Value::BOTTOM_LEFT:
                return cugl::Vec2(1, 1);
                break;
            case CILocation::Value::BOTTOM_RIGHT:
                return cugl::Vec2(-1, 1);
                break;
            case CILocation::Value::ON_SCREEN: //this case should not occur
                return cugl::Vec2(0, 0);
        }
    }
    
    /**
     * Helper function to get rotation for the fog asset.
     *
     * @param location The location of this oppoent node
     */
    float getFogRotationFromLocation(CILocation::Value location) {
        switch (location) {
            case CILocation::Value::TOP_LEFT:
                return -M_PI / 2;
                break;
            case CILocation::Value::TOP_RIGHT:
                return -M_PI;
                break;
            case CILocation::Value::BOTTOM_LEFT:
                return 0;
                break;
            case CILocation::Value::BOTTOM_RIGHT:
                return M_PI / 2;
                break;
            case CILocation::Value::ON_SCREEN: //this case should not occur
                return 0;
        }
    }
    
public:
    /**
     * Creates an opponent node with default values.
     */
    OpponentNode() : AnimationNode(), _progress(0) {}
    
    /**
     * Disposes the opponent node, releasing all resources.
     */
    ~OpponentNode() { dispose(); }
    
    /**
     * Disposes the opponent node, releasing all resources.
     */
    virtual void dispose() override;
    
    /**
     * Returns the newly allocated opponent node
     *
     * @param texture   The pointer to the shared progress texture
     * @param maxwidth The max width of the progress bar
     * @param maxheight The max height of the progress bar
     *
     * @return a newly allocated opponent node
     */
    static std::shared_ptr<OpponentNode> alloc(const std::shared_ptr<cugl::Texture>& texture,
                                               float maxwidth, float maxheight) {
        std::shared_ptr<OpponentNode> node = std::make_shared<OpponentNode>();
        return (node->AnimationNode::initWithFilmstrip(texture, PROGRESS_ROWS, PROGRESS_COLS) && node->init(texture, maxwidth, maxheight) ? node : nullptr);
    }
    
    /**
     * Initializes a new opponent node with the pointers.
     *
     * @param texture   The pointer to the shared progress texture
     * @param maxwidth The max width of the progress bar
     * @param maxheight The max height of the progress bar
     *
     * @return bool true if new node initialized successfully else false
     */
    bool init(const std::shared_ptr<cugl::Texture>& texture, float maxwidth, float maxheight) {
        _texture = texture;
        _maxwidth = maxwidth;
        _maxheight = maxheight;
        _timeElapsed = 0;
        _fogTimeOnScreen = 0;
        _fogAnimationProgress = 0;
        _fogOngoing = false;
        return true;
    }
    
    /**
     * Draws the progress bar to the game scene.
     */
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    /**
     * Sets the fog texture
     *
     * @param fogTexture The texture for the fog powerup
     */
    void setFogTexture(const std::shared_ptr<cugl::Texture>& fogTexture) {
        _fogTexture = fogTexture;
        cugl::Vec2 fogOrigin = _fogTexture->getSize() / 2;;
        _fogAnimationEasingFunction = cugl::EasingBezier::alloc(-fogOrigin, fogOrigin);
    }
    
    /**
     * Applies the fog power up.
     */
    void applyFogPower() {
        if (!_fogOngoing) {
            _fogTimeOnScreen = 0;
            _fogAnimationProgress = 0;
            _fogOngoing = true;
        }
    }
    
    /**
     * Starts the animation of the progress bar flashing
     */
    void startHitAnimation() {
        if (getFrame() < PROGRESS_FLASH_START) {
            setFrame(PROGRESS_FLASH_START);
        }
    }
    
    /**
     * Set the progress and color of the opponent node.
     *
     * @param progress The progress of the opponent towards winning
     * @param color The color of the opponent's planet
     */
    void setProgress(float progress, CIColor::Value color) {
        _progress = progress;
        setColor(CIColor::getColor4(color));
    }
    
    /**
     * Set the location of this opponent node
     *
     * @param location The location of this opponent node
     */
    void setLocation(CILocation::Value location) {
        _location = location;
    }
    
    /**
     * Set the player name associated with this opponent node
     *
     * @param name The name to display
     * @param font The font to use for the name
     */
    void setName(std::string name, std::shared_ptr<cugl::Font> font) {
        if (_nameLabel == nullptr) {
            _nameLabel = cugl::scene2::Label::alloc(name, font);
            cugl::Vec2 pos = getReflectFromLocation(_location) * cugl::Vec2(NAMETAG_X_OFFSET, NAMETAG_Y_OFFSET);
            _nameLabel->setPosition(pos);
            _nameLabel->setRelativeColor(false);
            _nameLabel->setForeground(cugl::Color4::WHITE);
            addChild(_nameLabel);
        } else {
            _nameLabel->setText(name);
        }
    }
    
    /**
     * Updates the animations for this opponent node.
     *
     * @param timestep The amount of time since the last animation frame
     */
    void update(float timestep);
};

#endif /* __CI_OPPONENT_NODE_H__ */

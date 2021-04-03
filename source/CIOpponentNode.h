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
#include "CIColor.h"
#include "CILocation.h"

class OpponentNode : public cugl::scene2::SceneNode {
private:
    /** Graphic asset used to display progress */
    std::shared_ptr<cugl::Texture> _texture;
    /** Progress of the opponent towards winning, between 0 and 1 */
    float _progress;
    /** The maximum width of the progress bar */
    float _maxwidth;
    /** The maximum height of the progress bar */
    float _maxheight;
    /** The corner that this opponent node is in */
    Location _location;
    
    cugl::Vec2 getReflectFromLocation(Location location) {
        switch (location) {
            case Location::TOP_LEFT:
                return cugl::Vec2(1, -1);
                break;
            case Location::TOP_RIGHT:
                return cugl::Vec2(-1, -1);
                break;
            case Location::BOTTOM_LEFT:
                return cugl::Vec2(1, 1);
                break;
            case Location::BOTTOM_RIGHT:
                return cugl::Vec2(-1, 1);
                break;
            case Location::ON_SCREEN: //this case should not occur
                return cugl::Vec2(0, 0);
        }
    }
    
public:
    /**
     * Creates an opponent node with default values.
     */
    OpponentNode() : SceneNode(), _progress(0) {} 
    
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
        return (node->init(texture, maxwidth, maxheight) ? node : nullptr);
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
        return SceneNode::init();
    }
    
    /**
     * Draws the progress bar to the game scene.
     */
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
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
    
    void setLocation(Location location) {
        _location = location;
    }
    
};

#endif /* __CI_OPPONENT_NODE_H__ */

//
//  CIPlanetProgressNode.h
//  CoreImpact
//
//  Created by William Long on 4/19/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLANET_PROGRESS_NODE_H__
#define __CI_PLANET_PROGRESS_NODE_H__
#include <cugl/cugl.h>
#include "CIPlanetLayer.h"

#define PROGRESS_ARC_ROWS   5
#define PROGRESS_ARC_COLS   13
#define PROGRESS_ARC_END    60
#define PROGRESS_ARC_START  0

#define LOCKIN_ROWS         14
#define LOCKIN_COLS         13
#define LOCKIN_END          173
#define LOCKIN_START        0

class PlanetProgressNode : public cugl::scene2::AnimationNode {
private:
    /** A layer of the player's planet. */
    PlanetLayer _planetLayer;
    
    /** Texture to display planet progress. */
    std::shared_ptr<cugl::Texture> _progressTexture;
    
    /** The amount of time since last animation frame change */
    float _timeElapsed;
    
    int _currFrame;
    
    /** The number for planet layer associated with this node. Numbering starts at 0 */
    int _layerNum;
    
public:
    /**
     * Creates a planet progress node with default values.
     */
    PlanetProgressNode() : AnimationNode() {}

    /**
     * Disposes the planet progress node, releasing all resources.
     */
    ~PlanetProgressNode() { dispose(); }
    
    /**
     * Disposes the planet progress node, releasing all resources.
     */
    virtual void dispose() override;
    
    /**
     * Returns the newly allocated Planet Progress Node
     *
     * @param progressTexture     Pointer to the texture to display planet progress
     *
     * @return a newly allocated Planet Progress Node
     */
    static std::shared_ptr<PlanetProgressNode> alloc(const std::shared_ptr<cugl::Texture>& progressTexture) {
        std::shared_ptr<PlanetProgressNode> node = std::make_shared<PlanetProgressNode>();
        return (node->AnimationNode::initWithFilmstrip(progressTexture, LOCKIN_ROWS, LOCKIN_COLS) && node->init(progressTexture) ? node : nullptr);
    }
    
    /** Initializes a new planet progress node.
     *
     * @param progressTexture     Pointer to the texture to display planet progress
     *
     * @return bool true if new node initialized successfully else false
     */
    bool init(const std::shared_ptr<cugl::Texture>& progressTexture) {
        _progressTexture = progressTexture;
        _timeElapsed = 0;
        _currFrame = LOCKIN_END;
        setScale(0.15f);
        return true;
    }
    
    /**
     * Sets the planet layer for this progress node
     *
     * @param layer     The planet layer associated with this progress node
     */
    void setLayer(PlanetLayer layer) {
        _planetLayer = layer;
    }
    
    void setLayerNum(int layerNum) {
        _layerNum = layerNum;
    }
    
    /**
     * Draws the planet progress to the game scene.
     */
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    /**
     * Updates the frame of the planet progress animation
     */
    void update(float timestep);
};

#endif /* __CI_PLANET_PROGRESS_NODE_H__ */
//
//  CIPlanetNode.h
//  CoreImpact
//
//  This class is used to draw the planet to the screen.
//  The root is used for the texture of the core, and all other layers
//  are added as children of this node
//
//  Created by William Long on 3/8/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_PLANET_NODE_H__
#define __CI_PLANET_NODE_H__
#include <cugl/cugl.h>
#include "CIColor.h"
#include "CIPlanetLayer.h"

#define PLANET_RING_TEXTURE_INNER_SIZE 140

/** Number of rows and cols in inner ring film strip */
#define INNER_RING_ROWS   10
#define INNER_RING_COLS   10
#define INNER_RING_END    90
#define INNER_RING_START  0

class PlanetNode : public cugl::scene2::PolygonNode {
private:
    class LayerNode {
    public:
        /** The node representing the inner ring of the layer */
        std::shared_ptr<cugl::scene2::AnimationNode> innerRing;
      
        /** The node representing the outer ring of the layer */
        std::shared_ptr<cugl::scene2::PolygonNode> outerRing;
        
    };
    
    /** The scaling factor of the core */
    float _coreScale;
    /** The scaling factor of the outermost layer */
    float _layerScale;
    /** The amount of time since last animation frame change */
    float _timeElapsed;
    
    /** The layers of this planet */
    std::vector<PlanetLayer>* _layers;
    /** The nodes representing the layers of this planet */
    std::vector<LayerNode> _layerNodes;
  
    void advanceFrame(LayerNode* node);
    
protected:
    /** The texture of an innner ring */
    std::shared_ptr<cugl::Texture> _ringTexture;
    /** The texture on the outside of an unlocked ring */
    std::shared_ptr<cugl::Texture> _unlockedTexture;
    /** The texture on the outside of a locked ring */
    std::shared_ptr<cugl::Texture> _lockedTexture;
    
public:
    PlanetNode() : PolygonNode(), _timeElapsed(0) {}
    
    ~PlanetNode() { dispose(); }

    void update(float timestep);
  
    static std::shared_ptr<PlanetNode> alloc(const std::shared_ptr<cugl::Texture>& core,
                                             const std::shared_ptr<cugl::Texture>& ring,
                                             const std::shared_ptr<cugl::Texture>& unlocked,
                                             const std::shared_ptr<cugl::Texture>& locked) {
        std::shared_ptr<PlanetNode> node = std::make_shared<PlanetNode>();
        node->_ringTexture = ring;
        node->_unlockedTexture = unlocked;
        node->_lockedTexture = locked;
        return (node->initWithTexture(core) ? node : nullptr);
    }
    
    void draw(const std::shared_ptr<cugl::SpriteBatch>& batch,
              const cugl::Mat4& transform, cugl::Color4 tint) override;
    
    void setLayers(std::vector<PlanetLayer>* layers);
    
    void setRadius(float r) {
        _layerScale = (INNER_RING_COLS * 2 * r) / (_ringTexture->getWidth());
        for (int ii = (int)(_layerNodes.size())-1; ii >= 0; ii--) {
            LayerNode* node = &_layerNodes[ii];
            if (node->innerRing != nullptr) {
                if (ii == 0) {
                    _coreScale = (INNER_RING_COLS * r * PLANET_RING_TEXTURE_INNER_SIZE) / (_ringTexture->getWidth() * getTexture()->getWidth());
                    setScale(_coreScale);
                }
                node->innerRing->setScale(_layerScale/_coreScale);
                node->outerRing->setScale(_layerScale/_coreScale);
                break;
            }
        }
    }
};


#endif /* __CI_PLANET_NODE_H__ */

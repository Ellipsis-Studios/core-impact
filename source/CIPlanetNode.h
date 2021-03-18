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

#define PLANET_RING_SCALE     0.28f

class PlanetNode : public cugl::scene2::PolygonNode {
private:
    class LayerNode {
    public:
        /** The node representing the inner ring of the layer */
        std::shared_ptr<cugl::scene2::PolygonNode> innerRing;
        /** The node representing the outer ring of the layer */
        std::shared_ptr<cugl::scene2::PolygonNode> outerRing;
    };
    
    /** Radius of the planet in pixels */
    float _radius;
    
    /** The layers of this planet */
    std::vector<PlanetLayer>* _layers;
    /** The nodes representing the layers of this planet */
    std::vector<LayerNode> _layerNodes;
    
protected:
    /** The texture of an innner ring */
    std::shared_ptr<cugl::Texture> _ringTexture;
    /** The texture on the outside of an unlocked ring */
    std::shared_ptr<cugl::Texture> _unlockedTexture;
    /** The texture on the outside of a locked ring */
    std::shared_ptr<cugl::Texture> _lockedTexture;
    
public:
    PlanetNode() : PolygonNode() {}
    
    ~PlanetNode() { dispose(); }

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
        _radius = r;
        float scale = _radius * 0.02f;
        setScale(scale);
        for (int ii = 0; ii < _layerNodes.size(); ii++) {
            LayerNode node = _layerNodes[ii];
            if (node.innerRing != nullptr) {
                node.innerRing->setScale(PLANET_RING_SCALE);
                node.outerRing->setScale(PLANET_RING_SCALE);
            }
        }
    }
};

#endif /* __CI_PLANET_NODE_H__ */

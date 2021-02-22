//
//  CUSpriteBatch.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides one-stop shopping for basic 2d graphics.  Despite the
//  name, it is also capable of drawing solid shapes, as well as wireframes.
//  It also has support for color gradients and (rotational) scissor masks.
//
//  While it is possible to swap out the shader for this class, the shader is
//  very peculiar in how it uses uniforms.  You should study SpriteShader.frag
//  and SpriteShader.vert before making any shader changes to this class.
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 2/10/20
#include <cugl/math/cu_math.h>
#include <cugl/util/CUDebug.h>
#include <cugl/render/CUSpriteBatch.h>
#include <cugl/render/CUVertexBuffer.h>
#include <cugl/render/CUTexture.h>
#include <cugl/render/CUShader.h>
#include <cugl/render/CUGradient.h>
#include <cugl/render/CUScissor.h>

/**
 * Default fragment shader
 *
 * This trick uses C++11 raw string literals to put the shader in a separate
 * file without having to guarantee its presence in the asset directory.
 * However, to work properly, the #include statement below MUST be on its
 * own separate line.
 */
const std::string oglShaderFrag =
#include "shaders/SpriteShader.frag"
;

/**
 * Default vertex shader
 *
 * This trick uses C++11 raw string literals to put the shader in a separate
 * file without having to guarantee its presence in the asset directory.
 * However, to work properly, the #include statement below MUST be on its
 * own separate line.
 */
const std::string oglShaderVert =
#include "shaders/SpriteShader.vert"
;

using namespace cugl;


#pragma mark Context

/** The drawing type for a textured mesh */
#define TYPE_TEXTURE    1
/** The drawing type for a gradient mesh */
#define TYPE_GRADIENT   2
/** The drawing type for a scissored mesh */
#define TYPE_SCISSOR    4
/** The drawing type for a (simple) texture blur */
#define TYPE_GAUSSBLUR  8

/** The drawing command has changed */
#define DIRTY_COMMAND       1
/** The blending equation has changed */
#define DIRTY_EQUATION      2
/** The blending factors have changed */
#define DIRTY_BLENDFACTOR   4
/** The depth test has changed */
#define DIRTY_DEPTHTEST     8
/** The drawing type has changed */
#define DIRTY_DRAWTYPE      16
/** The perspective matrix has changed */
#define DIRTY_PERSPECTIVE   32
/** The texture has changed */
#define DIRTY_TEXTURE       64
/** The block offset has changed */
#define DIRTY_UNIBLOCK      128
/** The blur step has changed */
#define DIRTY_BLURSTEP      256
/** All values have changed */
#define DIRTY_ALL_VALS      511

/**
 * Creates a context of the default uniforms.
 */
SpriteBatch::Context::Context() {
    first = 0;
    last  = 0;
    command = GL_TRIANGLES;
    blendEquation = GL_FUNC_ADD;
    srcFactor = GL_SRC_ALPHA;
    dstFactor = GL_ONE_MINUS_SRC_ALPHA;
    depthFunc = GL_ALWAYS;
    perspective = std::make_shared<Mat4>();
    perspective->setIdentity();
    texture  = nullptr;
    blurstep = 0;
    blockptr = -1;
    type = 0;
}

/**
 * Creates a copy of the given uniforms
 *
 * @param copy  The uniforms to copy
 */
SpriteBatch::Context::Context(Context* copy) {
    first = copy->first;
    last  = copy->last;
    type  = copy->type;
    command = copy->command;
    srcFactor = copy->srcFactor;
    dstFactor = copy->dstFactor;
    depthFunc = copy->depthFunc;
    blendEquation = copy->blendEquation;
    perspective = copy->perspective;
    texture  = copy->texture;
    blockptr = copy->blockptr;
    blurstep = copy->blurstep;
    dirty = 0;
}

/**
 * Disposes this collection of uniforms
 */
SpriteBatch::Context::~Context() {
    first = 0;
    last  = 0;
    command = GL_FALSE;
    blendEquation = GL_FALSE;
    srcFactor = GL_FALSE;
    dstFactor = GL_FALSE;
    depthFunc = GL_ALWAYS;
    perspective = nullptr;
    texture  = nullptr;
    blockptr = -1;
    type = 0;
}

#pragma mark -
#pragma mark Constructors
/**
 * Creates a degenerate sprite batch with no buffers.
 *
 * You must initialize the buffer before using it.
 */
SpriteBatch::SpriteBatch() :
_initialized(false),
_active(false),
_inflight(false),
_vertData(nullptr),
_indxData(nullptr),
_color(Color4f::WHITE),
_context(nullptr),
_depth(0),
_vertMax(0),
_vertSize(0),
_indxMax(0),
_indxSize(0),
_vertTotal(0),
_callTotal(0) {
    _shader = nullptr;
    _vertbuff = nullptr;
    _unifbuff = nullptr;
    _gradient = nullptr;
    _scissor  = nullptr;
}

/**
 * Deletes the vertex buffers and resets all attributes.
 *
 * You must reinitialize the sprite batch to use it.
 */
void SpriteBatch::dispose() {
    if (_vertData) {
        delete[] _vertData; _vertData = nullptr;
    }
    if (_indxData) {
        delete[] _indxData; _indxData = nullptr;
    }
    if (_context != nullptr) {
        delete _context; _context = nullptr;
    }
    _shader = nullptr;
    _vertbuff = nullptr;
    _unifbuff = nullptr;
    _gradient = nullptr;
    _scissor  = nullptr;
    
    _vertMax  = 0;
    _vertSize = 0;
    _indxMax  = 0;
    _indxSize = 0;
    _depth = 0;
    _color = Color4f::WHITE;
    
    _vertTotal = 0;
    _callTotal = 0;
    
    _initialized = false;
    _inflight = false;
    _active = false;
}

/**
 * Initializes a sprite batch with the default vertex capacity.
 *
 * The default vertex capacity is 8192 vertices and 8192*3 = 24576 indices.
 * If the mesh exceeds these values, the sprite batch will flush before
 * before continuing to draw. Similarly uniform buffer is initialized with
 * 512 buffer positions. This means that the uniform buffer is comparable
 * in memory size to the vertices, but only allows 512 gradient or scissor
 * mask context switches before the sprite batch must flush. If you wish to
 * increase (or decrease) the capacity, use the alternate initializer.
 *
 * The sprite batch begins with the default blank texture, and color white.
 * The perspective matrix is the identity.
 *
 * @return true if initialization was successful.
 */
bool SpriteBatch::init() {
    return init(DEFAULT_CAPACITY,Shader::alloc(SHADER(oglShaderVert),SHADER(oglShaderFrag)));
}

/** 
 * Initializes a sprite batch with the default vertex capacity.
 *
 * The index capacity will be 3 times the vertex capacity. The maximum
 * number of possible indices is the maximum size_t, so the vertex size
 * must be a third that.  In addition, the sprite batch will allocate
 * 1/16 of the vertex capacity for uniform blocks (for gradients and
 * scissor masks). This means that the uniform buffer is comparable
 * in memory size to the vertices while still allowing a reasonably
 * high rate of change for quads and regularly shaped sprites.
 *
 * If the mesh exceeds the capacity, the sprite batch will flush before
 * before continuing to draw. You should tune your system to have the
 * appropriate capacity.  To small a capacity will cause the system to
 * thrash.  However, too large a capacity could stall on memory transfers.
 *
 * The sprite batch begins with no active texture, and the color white.
 * The perspective matrix is the identity.
 *
 * @return true if initialization was successful.
 */
bool SpriteBatch::init(unsigned int capacity) {
    return init(capacity,Shader::alloc(SHADER(oglShaderVert),SHADER(oglShaderFrag)));
}

/**
 * Initializes a sprite batch with the default vertex capacity and given shader
 *
 * The index capacity will be 3 times the vertex capacity. The maximum
 * number of possible indices is the maximum size_t, so the vertex size
 * must be a third that.  In addition, the sprite batch will allocate
 * 1/16 of the vertex capacity for uniform blocks (for gradients and
 * scissor masks). This means that the uniform buffer is comparable
 * in memory size to the vertices while still allowing a reasonably
 * high rate of change for quads and regularly shaped sprites.
 *
 * If the mesh exceeds the capacity, the sprite batch will flush before
 * before continuing to draw. You should tune your system to have the
 * appropriate capacity.  To small a capacity will cause the system to
 * thrash.  However, too large a capacity could stall on memory transfers.
 *
 * The sprite batch begins with no active texture, and the color white.
 * The perspective matrix is the identity.
 *
 * See the class description for the properties of a valid shader.
 *
 * @param shader The shader to use for this spritebatch
 *
 * @return true if initialization was successful.
 */
bool SpriteBatch::init(unsigned int capacity, const std::shared_ptr<Shader>& shader) {
    if (_initialized) {
        CUAssertLog(false, "SpriteBatch is already initialized");
        return false; // If asserts are turned off.
    } else if (shader == nullptr) {
        CUAssertLog(false, "SpriteBatch shader cannot be null");
        return false; // If asserts are turned off.
    }
    
    _shader = shader;
    
    _vertbuff = VertexBuffer::alloc(sizeof(SpriteVertex3));
    _vertbuff->setupAttribute("aPosition", 3, GL_FLOAT, GL_FALSE, 0);
    _vertbuff->setupAttribute("aColor",    4, GL_FLOAT, GL_TRUE,
                            offsetof(cugl::SpriteVertex3,color));
    _vertbuff->setupAttribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
                            offsetof(cugl::SpriteVertex3,texcoord));
    _vertbuff->attach(_shader);
    
    // Set up data arrays;
    _vertMax = capacity;
    _vertData = new SpriteVertex3[_vertMax];
    _indxMax = capacity*3;
    _indxData = new GLuint[_indxMax];
    
    // Create uniform buffer (this has its own backing array)
    _unifbuff = UniformBuffer::alloc(40*sizeof(float),capacity/16);
    
    // Layout std140 format
    _unifbuff->setOffset("scMatrix", 0);
    _unifbuff->setOffset("scExtent", 48);
    _unifbuff->setOffset("scScale",  56);
    _unifbuff->setOffset("gdMatrix", 64);
    _unifbuff->setOffset("gdInner",  112);
    _unifbuff->setOffset("gdOuter",  128);
    _unifbuff->setOffset("gdExtent", 144);
    _unifbuff->setOffset("gdRadius", 152);
    _unifbuff->setOffset("gdFeathr", 156);

    _shader->setUniformBlock("uContext",_unifbuff);
    
    _context = new Context();
    _context->dirty = DIRTY_ALL_VALS;
    return true;
}


#pragma mark -
#pragma mark Attributes
/**
 * Sets the active color of this sprite batch 
 *
 * All subsequent shapes and outlines drawn by this sprite batch will be
 * tinted by this color.  This color is white by default.
 *
 * @param color The active color for this sprite batch
 */
void SpriteBatch::setColor(const Color4f color) {
    if (_color == color) {
        return;
    }
    _color = color;
    if (_gradient != nullptr) {
        if (_inflight) { record(); }
        _gradient->setTintColor(color);
        _context->dirty = _context->dirty | DIRTY_UNIBLOCK;
    }
}

/**
 * Sets the shader for this sprite batch
 *
 * This value may NOT be changed during a drawing pass.
 *
 * @param color The active color for this sprite batch
 */
void SpriteBatch::setShader(const std::shared_ptr<Shader>& shader) {
    CUAssertLog(_active, "Attempt to reassign shader while drawing is active");
    CUAssertLog(shader != nullptr, "Shader cannot be null");
    _vertbuff->detach();
    _shader = shader;
    _vertbuff->attach(_shader);
    _shader->setUniformBlock("uContext", _unifbuff);
}


/**
 * Sets the active perspective matrix of this sprite batch
 *
 * The perspective matrix is the combined modelview-projection from the
 * camera. By default, this is the identity matrix. Changing this value
 * will cause the sprite batch to flush.
 *
 * @param perspective   The active perspective matrix for this sprite batch
 */
void SpriteBatch::setPerspective(const Mat4& perspective) {
    if (_context->perspective.get() != &perspective) {
        if (_inflight) { record(); }
        auto matrix = std::make_shared<Mat4>(perspective);
        _context->perspective = matrix;
        _context->dirty = _context->dirty | DIRTY_PERSPECTIVE;
    }
}


/**
 * Sets the active texture of this sprite batch
 *
 * All subsequent shapes and outlines drawn by this sprite batch will use
 * this texture.  If the value is nullptr, all shapes and outlines will be
 * draw with a solid color instead.  This value is nullptr by default.
 *
 * Changing this value will cause the sprite batch to flush.  However, a
 * subtexture will not cause a pipeline flush.  This is an important
 * argument for using texture atlases.
 *
 * @param color The active texture for this sprite batch
 */
void SpriteBatch::setTexture(const std::shared_ptr<Texture>& texture) {
    if (texture == _context->texture) {
        return;
    }

    if (_inflight) { record(); }
    if (texture == nullptr) {
        // Active texture is not null
        _context->dirty = _context->dirty | DIRTY_DRAWTYPE;
        _context->texture = nullptr;
        _context->type = _context->type & ~TYPE_TEXTURE;
    } else if (_context->texture == nullptr) {
        // Texture is not null
        _context->dirty = _context->dirty | DIRTY_DRAWTYPE | DIRTY_TEXTURE;
        _context->texture = texture;
        _context->type = _context->type | TYPE_TEXTURE;
    } else {
        // Both must be not nullptr
        if (_context->texture->getBuffer() != texture->getBuffer()) {
            _context->dirty = _context->dirty | DIRTY_TEXTURE;
        }
        _context->texture = texture;
        if (_context->texture->getBindPoint()) {
            _context->texture->setBindPoint(0);
        }
    }
}

/**
 * Returns the active gradient of this sprite batch
 *
 * Gradients may be used in the place of (and together with) colors.
 * Gradients are applied textures, and use the first two attrbutes of
 * the color attribute in {@link Vertex3} as texture coordinates. The
 * last two coordinates (z and w) can used for minor feather adjustments
 * in linear gradients (for smooth stroke effects).
 *
 * Setting a gradient value does not guarantee that it will be used.
 * Gradients can be turned on or off by the {@link #useGradient} method.
 * By default, the gradient will not be used (as it is slower than
 * solid colors).
 *
 * All gradients are tinted by the active color. Unless you explicitly
 * want this tinting, you should set the active color to white before
 * drawing with an active gradient.
 *
 * This method returns a copy of the internal gradient. Changes to this
 * object have no effect on the sprite batch.
 *
 * @return The active gradient for this sprite batch
 */
std::shared_ptr<Gradient> SpriteBatch::getGradient() const {
    if (_gradient != nullptr) {
        return Gradient::alloc(_gradient);
    }
    return nullptr;
}

/**
 * Sets the active gradient of this sprite batch
 *
 * Gradients may be used in the place of (and together with) colors.
 * Gradients are applied textures, and use the first two attrbutes of
 * the color attribute in {@link Vertex3} as texture coordinates. The
 * last two coordinates (z and w) can used for minor feather adjustments
 * in linear gradients (for smooth stroke effects).
 *
 * If this value is nullptr, then no gradient is active. In that case,
 * the color vertex attribute will be interpretted as normal (e.g. a
 * traditional color vector).  This value is nullptr by default.
 *
 * All gradients are tinted by the active color. Unless you explicitly
 * want this tinting, you should set the active color to white before
 * drawing with an active gradient.
 *
 * This method acquires a copy of the gradient. Changes to the original
 * gradient after calling this method have no effect.
 *
 * @param gradient   The active gradient for this sprite batch
 */
void SpriteBatch::setGradient(const std::shared_ptr<Gradient>& gradient) {
    if (gradient == _gradient) {
        return;
    }
    
    if (_inflight) { record(); }
    if (gradient == nullptr) {
        // Active gradient is not null
        _context->dirty = _context->dirty | DIRTY_UNIBLOCK | DIRTY_DRAWTYPE;
        _context->type = _context->type & ~TYPE_GRADIENT;
        _gradient = nullptr;
    } else {
        _context->dirty = _context->dirty | DIRTY_UNIBLOCK | DIRTY_DRAWTYPE;
        _context->type = _context->type | TYPE_GRADIENT;
        _gradient = Gradient::alloc(gradient);
        _gradient->setTintColor(_color);
    }
}

/**
 * Returns the active scissor mask of this sprite batch
 *
 * Scissor masks may be combined with all types of drawing (colors,
 * textures, and gradients).  They are specified in the same coordinate
 * system as {@link getPerspective}.
 *
 * If this value is nullptr, then no scissor mask is active. This value
 * is nullptr by default.
 *
 * This method returns a copy of the internal scissor. Changes to this
 * object have no effect on the sprite batch.
 *
 * @return The active scissor mask for this sprite batch
 */
std::shared_ptr<Scissor> SpriteBatch::getScissor() const {
    if (_scissor != nullptr) {
        return Scissor::alloc(_scissor);
    }
    return nullptr;
}

/**
 * Sets the active scissor mask of this sprite batch
 *
 * Scissor masks may be combined with all types of drawing (colors,
 * textures, and gradients).  They are specified in the same coordinate
 * system as {@link getPerspective}. 
 *
 * If this value is nullptr, then no scissor mask is active. This value
 * is nullptr by default.
 *
 * This method acquires a copy of the scissor. Changes to the original
 * scissor mask after calling this method have no effect.
 *
 * @param scissor   The active scissor mask for this sprite batch
 */
void SpriteBatch::setScissor(const std::shared_ptr<Scissor>& scissor) {
    if (scissor == _scissor) {
        return;
    }
    
    if (_inflight) { record(); }
    if (scissor == nullptr) {
        // Active gradient is not null
        _context->dirty = _context->dirty | DIRTY_UNIBLOCK | DIRTY_DRAWTYPE;
        _context->type = _context->type & ~TYPE_SCISSOR;
        _scissor = nullptr;
    } else {
        _context->dirty = _context->dirty | DIRTY_UNIBLOCK | DIRTY_DRAWTYPE;
        _context->type = _context->type | TYPE_SCISSOR;
        _scissor = Scissor::alloc(scissor);
    }
}

/**
 * Sets the blending function for this sprite batch
 *
 * The enums are the standard ones supported by OpenGL.  See
 *
 *      https://www.opengl.org/sdk/docs/man/html/glBlendFunc.xhtml
 *
 * However, this setter does not do any error checking to verify that
 * the enums are valid.  By default, srcFactor is GL_SRC_ALPHA while
 * dstFactor is GL_ONE_MINUS_SRC_ALPHA. This corresponds to non-premultiplied
 * alpha blending.
 *
 * Changing this value will cause the sprite batch to flush.
 *
 * @param srcFactor Specifies how the source blending factors are computed
 * @param dstFactor Specifies how the destination blending factors are computed.
 */
void SpriteBatch::setBlendFunc(GLenum srcFactor, GLenum dstFactor) {
    if (_context->srcFactor != srcFactor || _context->dstFactor != dstFactor) {
        if (_inflight) { record(); }
        _context->srcFactor = srcFactor;
        _context->dstFactor = dstFactor;
        _context->dirty = _context->dirty | DIRTY_BLENDFACTOR;
    }
}

/**
 * Sets the blending equation for this sprite batch
 *
 * The enum must be a standard ones supported by OpenGL.  See
 *
 *      https://www.opengl.org/sdk/docs/man/html/glBlendEquation.xhtml
 *
 * However, this setter does not do any error checking to verify that
 * the input is valid.  By default, the equation is GL_FUNC_ADD.
 *
 * Changing this value will cause the sprite batch to flush.
 *
 * @param equation  Specifies how source and destination colors are combined
 */
void SpriteBatch::setBlendEquation(GLenum equation) {
    if (_context->blendEquation != equation) {
        if (_inflight) { record(); }
        _context->blendEquation = equation;
        _context->dirty = _context->dirty | DIRTY_EQUATION;
    }
}

/**
 * Returns the current drawing command.
 *
 * The value must be one of GL_TRIANGLES or GL_LINES.  Changing this value
 * during a drawing pass will flush the buffer.
 *
 * @return the current drawing command.
 */
void SpriteBatch::setCommand(GLenum command) {
    if (_context->command != command) {
        if (_inflight) { record(); }
        _context->command = command;
        _context->dirty = _context->dirty | DIRTY_COMMAND;
    }
}


/**
 * Sets the depth testing function for this sprite batch 
 *
 * The enum must be a standard ones supported by OpenGL. See
 *
 *      https://www.opengl.org/sdk/docs/man/html/glDepthFunc.xhtml
 *
 * However, this setter does not do any error checking to verify that
 * the input is valid.
 *
 * To disable depth testing, set the function to GL_ALWAYS. GL_ALWAYS is
 * the initial default value.
 *
 * @param function  Specifies how to accept fragments by depth value
 */
void SpriteBatch::setDepthFunc(GLenum function) {
    if (_context->depthFunc != function) {
        if (_inflight) { record(); }
        _context->depthFunc = function;
        _context->dirty = _context->dirty | DIRTY_DEPTHTEST;
    }
}

/**
 * Sets the blur step in pixels (0 if there is no blurring).
 *
 * This sprite batch supports a simple 9-step blur. The blur samples
 * from the center pixel and 8 other pixels around it in a box. The
 * blur step is the number of pixels away to sample. So a 1-step
 * blur samples from the immediate neighbor pixels. On most textures
 * a 5-step blur has very noticeable affects.
 *
 * This is not a full-featured Gaussian blur. In particular, large
 * step values will start to produce a pixellation effect. But it
 * can produce acceptable blur effects with little cost to performance.
 * It is especially ideal for font-blur effects on font atlases.
 *
 * Setting this value to 0 will disable texture blurring.  This
 * value is 0 by default.
 *
 * @param step  The blur step in pixels
 */
void SpriteBatch::setBlurStep(GLuint step) {
    if (_context->blurstep == step) {
        return;
    }
    
    if (_inflight) { record(); }
    if (step == 0) {
        // Active gradient is not null
        _context->dirty = _context->dirty | DIRTY_BLURSTEP | DIRTY_DRAWTYPE;
        _context->type = _context->type & ~TYPE_GAUSSBLUR;
    } else if (_context->blurstep == 0){
        _context->dirty = _context->dirty | DIRTY_BLURSTEP | DIRTY_DRAWTYPE;
        _context->type = _context->type | TYPE_GAUSSBLUR;
    } else {
        _context->dirty = _context->dirty | DIRTY_BLURSTEP;
    }
    _context->blurstep = step;
}


#pragma mark -
#pragma mark Rendering
/**
 * Starts drawing with the current perspective matrix.
 *
 * This call will disable depth buffer writing. It enables blending and
 * texturing. You must call end() to complete drawing.
 *
 * Calling this method will reset the vertex and OpenGL call counters to 0.
 */
void SpriteBatch::begin() {
    glDisable(GL_CULL_FACE);
    glDepthMask(true);
    glEnable(GL_BLEND);

    // DO NOT CLEAR.  This responsibility lies elsewhere
    _shader->bind();
    _vertbuff->bind();
    _unifbuff->bind(false);
    _unifbuff->deactivate();
    _active = true;
    _callTotal = 0;
    _vertTotal = 0;
}

/**
 * Completes the drawing pass for this sprite batch, flushing the buffer.
 *
 * This method enables depth writes and disables blending and texturing. It
 * Must always be called after a call to {@link #begin()}.
 */
void SpriteBatch::end() {
    CUAssertLog(_active,"SpriteBatch is not active");
    flush();
    _shader->unbind();
    _active = false;
}


/**
 * Flushes the current mesh without completing the drawing pass.
 *
 * This method is called whenever you change any attribute other than color
 * mid-pass. It prevents the attribute change from retoactively affecting
 * previuosly drawn shapes.
 */
void SpriteBatch::flush() {
    if (_indxSize == 0 || _vertSize == 0) {
        return;
    } else if (_context->first != _indxSize) {
        record();
    }
    
    // Load all the vertex data at once
    _vertbuff->loadVertexData(_vertData, _vertSize);
    _vertbuff->loadIndexData(_indxData, _indxSize);
    _unifbuff->activate();
    _unifbuff->flush();
    
    // Chunk the uniforms
    std::shared_ptr<Texture> previous = _context->texture;
    for(auto it = _history.begin(); it != _history.end(); ++it) {
        Context* next = *it;
        if (next->dirty & DIRTY_EQUATION) {
            glBlendEquation(next->blendEquation);
        }
        if (next->dirty & DIRTY_BLENDFACTOR) {
            glBlendFunc(next->srcFactor, next->dstFactor);
        }
        if (next->dirty & DIRTY_BLENDFACTOR) {
            glBlendFunc(next->srcFactor, next->dstFactor);
        }
        if (next->dirty & DIRTY_DEPTHTEST) {
            if (next->depthFunc == GL_ALWAYS) {
                glDisable(GL_DEPTH_TEST);
            } else {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(next->depthFunc);
            }
        }
        if (next->dirty & DIRTY_DRAWTYPE) {
             _shader->setUniform1i("uType", next->type);
        }
        if (next->dirty & DIRTY_PERSPECTIVE) {
            _shader->setUniformMat4("uPerspective",*(next->perspective.get()));
        }
        if (next->dirty & DIRTY_TEXTURE) {
            previous = next->texture;
            if (previous != nullptr) {
                previous->bind();
            }
        }
        if (next->dirty & DIRTY_UNIBLOCK) {
            _unifbuff->setBlock(next->blockptr);
        }
        if (next->dirty & DIRTY_BLURSTEP) {
            blurTexture(next->texture,next->blurstep);
        }
        GLuint amt = next->last-next->first;
        _vertbuff->draw(next->command, amt, next->first);
        _callTotal++;
    }
    
    _unifbuff->deactivate();
    
    // Increment the counters
    _vertTotal += _indxSize;
    
    _vertSize = _indxSize = 0;
    unwind();
    _context->first = 0;
    _context->last  = 0;
    _context->blockptr = -1;
}


#pragma mark -
#pragma mark Solid Shapes
/**
 * Draws the given rectangle filled with the current color and texture.
 *
 * The texture will fill the entire rectangle with texture coordinate 
 * (0,1) at the bottom left corner identified by rect,origin. To draw only
 * part of a texture, use a subtexture to fill the rectangle with the
 * region [minS,maxS]x[min,maxT]. Alternatively, you can use a {@link Poly2}
 * for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to draw
 */
void SpriteBatch::fill(const Rect rect) {
    setCommand(GL_TRIANGLES);
    prepare(rect);
}

/**
 * Draws the given rectangle filled with the current color and texture.
 *
 * The texture will fill the entire rectangle with texture coordinate
 * (0,1) at the bottom left corner identified by rect,origin. To draw only
 * part of a texture, use a subtexture to fill the rectangle with the
 * region [minS,maxS]x[min,maxT]. Alternatively, you can use a {@link Poly2}
 * for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to draw
 * @param offset    The rectangle offset
 */
void SpriteBatch::fill(const Rect rect, const Vec2 offset) {
    Mat4 transform;
    transform.translate((Vec3)offset);
    setCommand(GL_TRIANGLES);
    prepare(rect,transform);
}

/**
 * Draws the given rectangle filled with the current color and texture.
 *
 * The rectangle will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin, 
 * which is specified relative to the origin of the rectangle (not world 
 * coordinates).  So to spin about the center, the origin should be width/2, 
 * height/2 of the rectangle.
 *
 * The texture will fill the entire rectangle before being transformed. 
 * Texture coordinate (0,1) will at the bottom left corner identified by 
 * rect,origin. To draw only part of a texture, use a subtexture to fill 
 * the rectangle with the region [minS,maxS]x[min,maxT]. Alternatively, you 
 * can use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to draw
 * @param origin    The rotational offset in the rectangle
 * @param scale     The amount to scale the rectangle
 * @param angle     The amount to rotate the rectangle
 * @param offset    The rectangle offset in world coordinates
 */
void SpriteBatch::fill(const Rect rect, const Vec2 origin, const Vec2 scale,
                       float angle, const Vec2 offset) {
    Mat4 transform;
    transform.translate(-origin.x,-origin.y,0);
    transform.scale(scale);
    transform.rotateZ(angle);
    transform.translate((Vec3)offset);

    setCommand(GL_TRIANGLES);
    prepare(rect,transform);
}

/**
 * Draws the given rectangle filled with the current color and texture.
 *
 * The rectangle will transformed by the given matrix. The transform will 
 * be applied assuming the given origin, which is specified relative to 
 * the origin of the rectangle (not world coordinates).  So to apply the 
 * transform to the center of the rectangle, the origin should be width/2, 
 * height/2 of the rectangle.
 *
 * The texture will fill the entire rectangle with texture coordinate
 * (0,1) at the bottom left corner identified by rect,origin. To draw only
 * part of a texture, use a subtexture to fill the rectangle with the
 * region [minS,maxS]x[min,maxT]. Alternatively, you can use a {@link Poly2}
 * for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to draw
 * @param origin    The rotational offset in the rectangle
 * @param transform The coordinate transform
 */
void SpriteBatch::fill(const Rect rect, const Vec2 origin, const Mat4& transform) {
    Mat4 matrix;
    matrix.translate(-origin.x,-origin.y,0);
    matrix *= transform;
    setCommand(GL_TRIANGLES);
    prepare(rect,matrix);
}

/**
 * Draws the given polygon filled with the current color and texture.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation 
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A 
 * vertical coordinate has texture coordinate 1-y/texture.height. As a 
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.  
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the 
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to draw
 */
void SpriteBatch::fill(const Poly2& poly) {
    CUAssertLog(poly.getGeometry() == Geometry::SOLID, "Polygon is not triangulated");
    setCommand(GL_TRIANGLES);
    prepare(poly);
}

/**
 * Draws the given polygon filled with the current color and texture.
 *
 * The polygon will be offset by the given position.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to draw
 * @param offset    The polygon offset
 */
void SpriteBatch::fill(const Poly2& poly, const Vec2 offset) {
    CUAssertLog(poly.getGeometry() == Geometry::SOLID, "Polygon is not triangulated");
    setCommand(GL_TRIANGLES);
    prepare(poly,offset);
}

/**
 * Draws the given polygon filled with the current color and texture.
 *
 * The polygon will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin,
 * which is specified relative to the origin of the polygon (not world 
 * coordinates). Hence this origin is essentially the pixel coordinate 
 * of the texture (see below) to assign as the rotational center.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to draw
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the polygon
 * @param angle     The amount to rotate the polygon
 * @param offset    The polygon offset in world coordinates
 */
void SpriteBatch::fill(const Poly2& poly, const Vec2 origin, const Vec2 scale,
                       float angle, const Vec2 offset) {
    CUAssertLog(poly.getGeometry() == Geometry::SOLID, "Polygon is not triangulated");
    Mat4 transform;
    Mat4::createTranslation(-origin.x,-origin.y,0,&transform);
    transform.scale(scale);
    transform.rotateZ(angle);
    transform.translate((Vec3)offset);
    setCommand(GL_TRIANGLES);
    prepare(poly,transform);
}

/**
 * Draws the given polygon filled with the current color and texture.
 *
 * The polygon will transformed by the given matrix. The transform will
 * be applied assuming the given origin, which is specified relative to the 
 * origin of the polygon (not world coordinates). Hence this origin is 
 * essentially the pixel coordinate of the texture (see below) to 
 * assign as the origin of this transform.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to draw
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::fill(const Poly2& poly, const Vec2 origin, const Mat4& transform) {
    CUAssertLog(poly.getGeometry() == Geometry::SOLID, "Polygon is not triangulated");
    Mat4 matrix;
    matrix.translate(-origin.x,-origin.y,0);
    matrix *= transform;
    setCommand(GL_TRIANGLES);
    prepare(poly,matrix);
}

/**
 * Fills the given mesh with the current texture and/or gradient.
 *
 * This method provides more fine tuned control over texture coordinates
 * that the other fill methods.  The texture no longer needs to be
 * drawn uniformly over the shape. The transform will be applied to the
 * vertex positions directly in world space. If depth testing is on, all
 * vertices will be the current depth.
 *
 * The triangulation will be determined by the mesh indices. If necessary,
 * these can be generated via one of the triangulation factories
 * {@link SimpleTriangulator} or {@link DelaunayTriangulator}.
 *
 * The mesh vertices use their own color values.  However, if tint is true,
 * these values will be tinted (i.e. multiplied) by the current active color.
 *
 * @param mesh      The sprite mesh
 * @param transform The coordinate transform
 * @param tint      Whether to tint with the active color
 */
void SpriteBatch::fill(const Mesh<SpriteVertex2>& mesh, const Mat4& transform, bool tint) {
    CUAssertLog(mesh.command == GL_TRIANGLES, "The mesh is not triangulated properly.");
    setCommand(GL_TRIANGLES);
    prepare(mesh,transform,tint);
}

/**
 * Fills the given mesh with the current texture and/or gradient.
 *
 * This method provides more fine tuned control over texture coordinates
 * that the other fill methods.  The texture no longer needs to be
 * drawn uniformly over the shape. The transform will be applied to the
 * vertex positions directly in world space. This method will use the
 * depth of each vertex and ignore the sprite batch depth
 *
 * The triangulation will be determined by the mesh indices. If necessary,
 * these can be generated via one of the triangulation factories
 * {@link SimpleTriangulator} or {@link DelaunayTriangulator}.
 *
 * The mesh vertices use their own color values.  However, if tint is true,
 * these values will be tinted (i.e. multiplied) by the current active color.
 *
 * @param mesh      The sprite mesh
 * @param transform The coordinate transform
 * @param tint      Whether to tint with the active color
 */
void SpriteBatch::fill(const Mesh<SpriteVertex3>& mesh, const Mat4& transform, bool tint) {
    CUAssertLog(mesh.command == GL_TRIANGLES, "The mesh is not triangulated properly.");
    setCommand(GL_TRIANGLES);
    prepare(mesh,transform,tint);
}


#pragma mark -
#pragma mark Outlines
/**
 * Outlines the given rectangle with the current color and texture.
 *
 * The drawing will be a wireframe of a rectangle.  The wireframe will
 * be textured with Texture coordinate (0,1) at the bottom left corner 
 * identified by rect,origin. The remaining edges will correspond to the
 * edges of the texture. To draw only part of a texture, use a subtexture
 * to outline the edges with [minS,maxS]x[min,maxT]. Alternatively, you can 
 * use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to outline
 */
void SpriteBatch::outline(const Rect rect) {
    setCommand(GL_LINES);
    prepare(rect);
}

/**
 * Outlines the given rectangle with the current color and texture.
 *
 * The drawing will be a wireframe of a rectangle.  The wireframe will
 * be textured with Texture coordinate (0,1) at the bottom left corner
 * identified by rect,origin. The remaining edges will correspond to the
 * edges of the texture. To draw only part of a texture, use a subtexture
 * to outline the edges with [minS,maxS]x[min,maxT]. Alternatively, you can
 * use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to outline
 * @param offset    The polygon offset
 */
void SpriteBatch::outline(const Rect rect, const Vec2 offset) {
    Mat4 transform;
    transform.translate((Vec3)offset);
    setCommand(GL_LINES);
    prepare(rect,transform);
}

/**
 * Outlines the given rectangle with the current color and texture.
 *
 * The rectangle will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin, 
 * which is specified relative to the origin of the rectangle (not world 
 * coordinates).  So to spin about the center, the origin should be width/2, 
 * height/2 of the rectangle.
 *
 * The drawing will be a wireframe of a rectangle.  The wireframe will
 * be textured with Texture coordinate (0,1) at the bottom left corner
 * identified by rect,origin. The remaining edges will correspond to the
 * edges of the texture. To draw only part of a texture, use a subtexture
 * to outline the edges with [minS,maxS]x[min,maxT]. Alternatively, you can
 * use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to outline
 * @param origin    The rotational offset in the rectangle
 * @param scale     The amount to scale the rectangle
 * @param angle     The amount to rotate the rectangle
 * @param offset    The rectangle offset in world coordinates
 */
void SpriteBatch::outline(const Rect rect, const Vec2 origin, const Vec2 scale,
                          float angle, const Vec2 offset) {
    Mat4 transform;
    Mat4::createTranslation(-origin.x,-origin.y,0,&transform);
    transform.scale(scale);
    transform.rotateZ(angle);
    transform.translate((Vec3)offset);
    setCommand(GL_LINES);
    prepare(rect,transform);
}

/**
 * Outlines the given rectangle with the current color and texture.
 *
 * The rectangle will transformed by the given matrix. The transform will 
 * be applied assuming the given origin, which is specified relative to 
 * the origin of the rectangle (not world coordinates).  So to apply the 
 * transform to the center of the rectangle, the origin should be width/2, 
 * height/2 of the rectangle.
 *
 * The drawing will be a wireframe of a rectangle.  The wireframe will
 * be textured with Texture coordinate (0,1) at the bottom left corner
 * identified by rect,origin. The remaining edges will correspond to the
 * edges of the texture. To draw only part of a texture, use a subtexture
 * to outline the edges with [minS,maxS]x[min,maxT]. Alternatively, you can
 * use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param rect      The rectangle to outline
 * @param origin    The rotational offset in the rectangle
 * @param transform The coordinate transform
 */
void SpriteBatch::outline(const Rect rect, const Vec2 origin, const Mat4& transform) {
    Mat4 matrix;
    matrix.translate(-origin.x,-origin.y,0);
    matrix *= transform;
    setCommand(GL_LINES);
    prepare(rect,matrix);
}

/**
 * Outlines the given polygon with the current color and texture.
 *
 * The polygon path will be determined by the indices in poly. The indices
 * should be a multiple of two, preferably generated by the factories
 * {@link PathOutliner} or {@link CubicSplineApproximator}.
 *
 * The drawing will be a wireframe of a polygon, but the lines are textured.
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply outlines the rectangle.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to outline
 */
void SpriteBatch::outline(const Poly2& poly) {
    CUAssertLog(poly.getGeometry() == Geometry::PATH, "Polygon is not a traversal");
    setCommand(GL_LINES);
    prepare(poly);
}

/**
 * Outlines the given polygon with the current color and texture.
 *
 * The polygon will be offset by the given position.
 *
 * The polygon path will be determined by the indices in poly. The indices
 * should be a multiple of two, preferably generated by the factories
 * {@link PathOutliner} or {@link CubicSplineApproximator}.
 *
 * The drawing will be a wireframe of a polygon, but the lines are textured.
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply outlines the rectangle.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to outline
 * @param offset    The polygon offset
 */
void SpriteBatch::outline(const Poly2& poly, const Vec2 offset) {
    CUAssertLog(poly.getGeometry() == Geometry::PATH, "Polygon is not a traversal");
    setCommand(GL_LINES);
    prepare(poly,offset);
}

/**
 * Outlines the given polygon with the current color and texture.
 *
 * The polygon will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin,
 * which is specified relative to the origin of the polygon (not world 
 * coordinates). Hence this origin is essentially the pixel coordinate 
 * of the texture (see below) to assign as the rotational center.
 *
 * The polygon path will be determined by the indices in poly. The indices
 * should be a multiple of two, preferably generated by the factories
 * {@link PathOutliner} or {@link CubicSplineApproximator}.
 *
 * The drawing will be a wireframe of a polygon, but the lines are textured.
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply outlines the rectangle.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture file, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to outline
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the polygon
 * @param angle     The amount to rotate the polygon
 * @param offset    The polygon offset in world coordinates
 */
void SpriteBatch::outline(const Poly2& poly, const Vec2 origin, const Vec2 scale,
                          float angle, const Vec2 offset) {
    CUAssertLog(poly.getGeometry() == Geometry::PATH, "Polygon is not a traversal");
    Mat4 transform;
    Mat4::createTranslation(-origin.x,-origin.y,0,&transform);
    transform.scale(scale);
    transform.rotateZ(angle);
    transform.translate((Vec3)offset);
    setCommand(GL_LINES);
    prepare(poly,transform);
}

/**
 * Outlines the given polygon with the current color and texture.
 *
 * The polygon will transformed by the given matrix. The transform will
 * be applied assuming the given origin, which is specified relative to the 
 * origin of the polygon (not world coordinates). Hence this origin is 
 * essentially the pixel coordinate of the texture (see below) to 
 * assign as the origin of this transform.
 *
 * The polygon path will be determined by the indices in poly. The indices
 * should be a multiple of two, preferably generated by the factories
 * {@link PathOutliner} or {@link CubicSplineApproximator}.
 *
 * The drawing will be a wireframe of a polygon, but the lines are textured.
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply outlines the rectangle.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param poly      The polygon to outline
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::outline(const Poly2& poly, const Vec2 origin, const Mat4& transform) {
    CUAssertLog(poly.getGeometry() == Geometry::PATH, "Polygon is not a traversal");
    Mat4 matrix;
    matrix.translate(-origin.x,-origin.y,0);
    matrix *= transform;
    setCommand(GL_LINES);
    prepare(poly,matrix);
}

/**
 * Outlines the given mesh with the current texture and/or gradient.
 *
 * This method provides more fine tuned control over texture coordinates
 * that the other fill methods.  The texture no longer needs to be
 * drawn uniformly over the shape. The transform will be applied to the
 * vertex positions directly in world space. If depth testing is on, all
 * vertices will be the current depth.
 *
 * The triangulation will be determined by the mesh indices. If necessary,
 * these can be generated via one of the triangulation factories
 * {@link SimpleTriangulator} or {@link DelaunayTriangulator}.
 *
 * The mesh vertices use their own color values.  However, if tint is true,
 * these values will be tinted (i.e. multiplied) by the current active color.
 *
 * @param mesh      The sprite mesh
 * @param transform The coordinate transform
 * @param tint      Whether to tint with the active color
 */
void SpriteBatch::outline(const Mesh<SpriteVertex2>& mesh, const Mat4& transform, bool tint) {
    setCommand(GL_LINES);
    prepare(mesh,transform,tint);
}

/**
 * Outlines the given mesh with the current texture and/or gradient.
 *
 * This method provides more fine tuned control over texture coordinates
 * that the other fill methods.  The texture no longer needs to be
 * drawn uniformly over the shape. The transform will be applied to the
 * vertex positions directly in world space. This method will use the
 * depth of each vertex and ignore the sprite batch depth
 *
 * The triangulation will be determined by the mesh indices. If necessary,
 * these can be generated via one of the triangulation factories
 * {@link SimpleTriangulator} or {@link DelaunayTriangulator}.
 *
 * The mesh vertices use their own color values.  However, if tint is true,
 * these values will be tinted (i.e. multiplied) by the current active color.
 *
 * @param mesh      The sprite mesh
 * @param transform The coordinate transform
 * @param tint      Whether to tint with the active color
 */
void SpriteBatch::outline(const Mesh<SpriteVertex3>& mesh, const Mat4& transform, bool tint) {
    setCommand(GL_LINES);
    prepare(mesh,transform,tint);
}


#pragma mark -
#pragma mark Convenience Methods
/**
 * Draws the texture (without tint) at the given position
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws a rectangle of the size of the texture, with bottom left
 * corner at the given position.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param position  The bottom left corner of the texture
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Vec2 position) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(Rect(position.x,position.y, (float)texture->getWidth(), (float)texture->getHeight()));
}

/**
 * Draws the tinted texture at the given position
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws a rectangle of the size of the texture, with bottom left
 * corner at the given position.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param position  The bottom left corner of the texture
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color, const Vec2 position) {
    setTexture(texture);
    setColor(color);
    fill(Rect(position.x,position.y, (float)texture->getWidth(), (float)texture->getHeight()));
}

/**
 * Draws the tinted texture at the given position
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).  
 * It then draws a rectangle of the size of the texture, with bottom left 
 * corner at the given position.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param position  The bottom left corner of the texture
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Rect bounds) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(bounds);
}

/**
 * Draws the texture (without tint) inside the given bounds
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the specified rectangle filled with the texture.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param bounds    The rectangle to texture
 */
 void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color, const Rect bounds) {
     setTexture(texture);
     setColor(color);
     fill(bounds);
 }

/**
 * Draws the texture (without tint) transformed by the given parameters
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws a texture-sized rectangle centered at the given origin, and
 * transformed by the given parameters.
 *
 * The rectangle will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin,
 * which is specified in texture pixel coordinates (e.g from the bottom
 * left corner).
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the texture
 * @param angle     The amount to rotate the texture
 * @param offset    The texture offset in world coordinates
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture,
                       const Vec2 origin, const Vec2 scale, float angle, const Vec2 offset) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(Rect(0,0, (float)texture->getWidth(), (float)texture->getHeight()), origin, scale, angle, offset);
}

/**
 * Draws the tinted texture transformed by the given parameters
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws a texture-sized rectangle centered at the given origin, and
 * transformed by the given parameters.
 *
 * The rectangle will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin,
 * which is specified in texture pixel coordinates (e.g from the bottom
 * left corner).
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the texture
 * @param angle     The amount to rotate the texture
 * @param offset    The texture offset in world coordinates
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color,
                       const Vec2 origin, const Vec2 scale, float angle, const Vec2 offset) {
    setTexture(texture);
    setColor(color);
    fill(Rect(0,0, (float)texture->getWidth(), (float)texture->getHeight()), origin, scale, angle, offset);
}

/**
 * Draws the texture (without tint) transformed by the given parameters
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It fills the specified rectangle with the texture.
 *
 * The rectangle will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin, 
 * which is specified relative to the origin of the rectangle (not world 
 * coordinates).  So to spin about the center, the origin should be width/2, 
 * height/2 of the rectangle.
 *
 * The texture will fill the entire rectangle before being transformed. 
 * Texture coordinate (0,1) will at the bottom left corner identified by 
 * rect,origin. To draw only part of a texture, use a subtexture to fill 
 * the rectangle with the region [minS,maxS]x[min,maxT]. Alternatively, you 
 * can use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param bounds    The rectangle to texture
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the texture
 * @param angle     The amount to rotate the texture
 * @param offset    The rectangle offset in world coordinates
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Rect bounds,
                       const Vec2 origin, const Vec2 scale, float angle, const Vec2 offset) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(bounds, origin, scale, angle, offset);
}

/**
 * Draws the tinted texture transformed by the given parameters
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It fills the specified rectangle with the texture.
 *
 * The rectangle will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin, 
 * which is specified relative to the origin of the rectangle (not world 
 * coordinates).  So to spin about the center, the origin should be width/2, 
 * height/2 of the rectangle.
 *
 * The texture will fill the entire rectangle before being transformed. 
 * Texture coordinate (0,1) will at the bottom left corner identified by 
 * rect,origin. To draw only part of a texture, use a subtexture to fill 
 * the rectangle with the region [minS,maxS]x[min,maxT]. Alternatively, you 
 * can use a {@link Poly2} for more fine-tuned control.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param bounds    The rectangle to texture
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the texture
 * @param angle     The amount to rotate the texture
 * @param offset    The rectangle offset in world coordinates
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color, const Rect bounds,
                       const Vec2 origin, const Vec2 scale, float angle, const Vec2 offset) {
    setTexture(texture);
    setColor(color);
    fill(bounds, origin, scale, angle, offset);
}

/**
 * Draws the texture (without tint) transformed by the matrix
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws a texture-sized rectangle centered at the given origin, and
 * transformed by the given matrix.
 *
 * The transform will be applied assuming the given image origin, which is
 * specified in texture pixel coordinates (e.g from the bottom left corner).
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Vec2 origin, const Mat4& transform) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(Rect(0,0, (float)texture->getWidth(), (float)texture->getHeight()), origin, transform);
}

/**
 * Draws the tinted texture transformed by the matrix
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws a texture-sized rectangle centered at the given origin, and
 * transformed by the given matrix.
 *
 * The transform will be applied assuming the given image origin, which is
 * specified in texture pixel coordinates (e.g from the bottom left corner).
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color,
                       const Vec2 origin, const Mat4& transform)  {
    setTexture(texture);
    setColor(color);
    fill(Rect(0,0, (float)texture->getWidth(), (float)texture->getHeight()), origin, transform);
}
    
/**
 * Draws the texture (without tint) transformed by the matrix
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It fills the specified rectangle with the texture, transformed by the 
 * given matrix.
 *
 * The transform will be applied assuming the given image origin, which is
 * specified in texture pixel coordinates (e.g from the bottom left corner).
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param bounds    The rectangle to texture
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Rect bounds,
                       const Vec2 origin, const Mat4& transform) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(bounds, origin, transform);
}

/**
 * Draws the tinted texture transformed by the matrix
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It fills the specified rectangle with the texture, transformed by the 
 * given matrix.
 *
 * The transform will be applied assuming the given image origin, which is
 * specified in texture pixel coordinates (e.g from the bottom left corner).
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param bounds    The rectangle to texture
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color,
                       const Rect bounds, const Vec2 origin, const Mat4& transform) {
    setTexture(texture);
    setColor(color);
    fill(bounds, origin, transform);
}

/**
 * Draws the textured polygon (without tint) at the given position
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the polygon, offset by the given value.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param poly      The polygon to texture
 * @param offset    The polygon offset
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture,
                       const Poly2& poly, const Vec2 offset) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(poly, offset);
}

/**
 * Draws the tinted, textured polygon at the given position
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the polygon, offset by the given value.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param poly      The polygon to texture
 * @param offset    The polygon offset
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color,
                       const Poly2& poly, const Vec2 offset) {
    setTexture(texture);
    setColor(color);
    fill(poly, offset);
}

/**
 * Draws the textured polygon (without tint) transformed by the given parameters
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the polygon, transformed by the given parameters.
 *
 * The polygon will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin,
 * which is specified relative to the origin of the polygon (not world 
 * coordinates). Hence this origin is essentially the pixel coordinate 
 * of the texture (see below) to assign as the rotational center.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param poly      The polygon to texture
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the polygon
 * @param angle     The amount to rotate the polygon
 * @param offset    The polygon offset in world coordinates
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Poly2& poly,
                       const Vec2 origin, const Vec2 scale, float angle,
                       const Vec2 offset) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(poly, origin, scale, angle, offset);
}

/**
 * Draws the tinted, textured polygon transformed by the given parameters
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the polygon, translated by the given parameters.
 *
 * The polygon will be scaled first, then rotated, and finally offset
 * by the given position. Rotation is measured in radians and is counter
 * clockwise from the x-axis.  Rotation will be about the provided origin,
 * which is specified relative to the origin of the polygon (not world 
 * coordinates). Hence this origin is essentially the pixel coordinate 
 * of the texture (see below) to assign as the rotational center.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param poly      The polygon to texture
 * @param origin    The image origin in pixel space
 * @param scale     The amount to scale the polygon
 * @param angle     The amount to rotate the polygon
 * @param offset    The polygon offset in world coordinates
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color,
                       const Poly2& poly, const Vec2 origin, const Vec2 scale,
                       float angle, const Vec2 offset) {
    setTexture(texture);
    setColor(color);
    fill(poly, origin, scale, angle, offset);
}

/**
 * Draws the textured polygon (without tint) transformed by the given matrix
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the polygon, translated by the given matrix.
 *
 * The polygon will transformed by the given matrix. The transform will
 * be applied assuming the given origin, which is specified relative to the 
 * origin of the polygon (not world coordinates). Hence this origin is 
 * essentially the pixel coordinate of the texture (see below) to 
 * assign as the origin of this transform.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param poly      The polygon to texture
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture,
                       const Poly2& poly, const Vec2 origin, const Mat4& transform) {
    setTexture(texture);
    setColor(Color4::WHITE);
    fill(poly, origin, transform);
}

/**
 * Draws the tinted, textured polygon transformed by the given matrix
 *
 * This is a convenience method that calls the appropriate fill method.
 * It sets both the texture and color (removing the previous active values).
 * It then draws the polygon, translated by the given matrix.
 *
 * The polygon will transformed by the given matrix. The transform will
 * be applied assuming the given origin, which is specified relative to the 
 * origin of the polygon (not world coordinates). Hence this origin is 
 * essentially the pixel coordinate of the texture (see below) to 
 * assign as the origin of this transform.
 *
 * The polygon tesselation will be determined by the indices in poly. If
 * the polygon has not been triangulated (by one of the triangulation
 * factories {@link SimpleTriangulator} or {@link DelaunayTriangulator},
 * it may not draw properly.
 *
 * The vertex coordinates will be determined by polygon vertex position.
 * A horizontal position x has texture coordinate x/texture.width. A
 * vertical coordinate has texture coordinate 1-y/texture.height. As a
 * result, a rectangular polygon that has the same dimensions as the texture
 * is the same as simply drawing the texture.
 *
 * One way to think of the polygon is as a "cookie cutter".  Treat the
 * polygon coordinates as pixel coordinates in the texture filed, and use
 * that to determine how the texture fills the polygon. This may make the
 * polygon larger than you like in order to get the appropriate texturing.
 * You should use one of the transform methods to fix this.
 *
 * If depth testing is on, all vertices will be the current depth.
 *
 * @param texture   The new active texture
 * @param color     The new active color
 * @param poly      The polygon to texture
 * @param origin    The image origin in pixel space
 * @param transform The coordinate transform
 */
void SpriteBatch::draw(const std::shared_ptr<Texture>& texture, const Color4f color,
                       const Poly2& poly, const Vec2 origin, const Mat4& transform) {
    setTexture(texture);
    setColor(color);
    fill(poly, origin, transform);
}


#pragma mark -
#pragma mark Internal Helpers
/**
 * Records the current set of uniforms, freezing them.
 *
 * This method must be called whenever {@link prepare} is called for
 * a new set of uniforms.  It ensures that the vertices batched so far
 * will use the correct set of uniforms.
 */
void SpriteBatch::record() {
    Context* next = new Context(_context);
    _context->last = _indxSize;
    next->first = _indxSize;
    _history.push_back(_context);
    _context = next;
    _inflight = false;
}

/**
 * Deletes the recorded uniforms.
 *
 * This method is called upon flushing or cleanup.
 */
void SpriteBatch::unwind() {
    for(auto it = _history.begin(); it != _history.end(); ++it) {
        delete *it;
    }
    _history.clear();
}

/**
 * Sets the active uniform block to agree with the gradient and stroke.
 *
 * This method is called upon vertex preparation.
 *
 * @param context   The current uniform context
 * @param tint      Whether to tint the gradient
 */
void SpriteBatch::setUniformBlock(Context* context, bool tint) {
    if (!(_context->dirty & DIRTY_UNIBLOCK)) {
        return;
    }
    if (_context->blockptr+1 >= _unifbuff->getBlockCount()) {
        flush();
    }
    float data[40];
    if (_scissor != nullptr) {
        _scissor->getData(data);
    } else {
        std::memset(data,0,16*sizeof(float));
    }
    if (_gradient != nullptr) {
        _gradient->setTintStatus(tint);
        _gradient->getData(data+16);
    } else {
        std::memset(data+16,0,24*sizeof(float));
    }
    _context->blockptr++;
    _unifbuff->setUniformfv(_context->blockptr,0,40,data);
}

/**
 * Updates the shader with the current blur offsets
 *
 * Blur offsets depend upon the texture size. This method converts the
 * blur step into an offset in texture coordinates. It supports
 * non-square textures.
 *
 * If there is no active texture, the blur offset will be 0.
 *
 * @param texture   The texture to blur
 * @param step      The blur step in pixels
 */
void SpriteBatch::blurTexture(const std::shared_ptr<Texture>& texture, GLuint step) {
    if (texture == nullptr) {
        _shader->setUniform2f("uBlur", 0, 0);
        return;
    }
    Size size = texture->getSize();
    size.width  = step/size.width;
    size.height = step/size.height;
    _shader->setUniform2f("uBlur",size.width,size.height);
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given rectangle to the vertex buffer, but does not
 * draw it yet.  You must call {@link #flush} or {@link #end} to draw the 
 * rectangle. This method will automatically flush if the maximum number
 * of vertices is reached.
 *
 * If depth testing is on, all vertices will use the current sprite
 * batch depth.
 *
 * @param rect  The rectangle to add to the buffer
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Rect rect) {
    if (_vertSize+4 >= _vertMax ||  _indxSize+8 >= _indxMax) {
        flush();
    }
    
    Texture* texture = _context->texture.get();
    float tsmax, tsmin;
    float ttmax, ttmin;
    
    if (texture != nullptr) {
        tsmax = texture->getMaxS();
        tsmin = texture->getMinS();
        ttmax = texture->getMaxT();
        ttmin = texture->getMinT();
    } else {
        tsmax = 1.0f; tsmin = 0.0f;
        ttmax = 1.0f; ttmin = 0.0f;
    }
    
    setUniformBlock(_context,true);
    Poly2 poly(rect, _context->command == GL_TRIANGLES);
    unsigned int vstart = _vertSize;
    int ii = 0;
    for(auto it = poly.vertices().begin(); it != poly.vertices().end(); ++it) {
        Vec3 point = Vec3((*it),_depth);
        _vertData[vstart+ii].position = point;
        
        point.x = (point.x-rect.origin.x)/rect.size.width;
        point.y = 1-(point.y-rect.origin.y)/rect.size.height;
        _vertData[vstart+ii].texcoord.x = point.x*tsmax+(1-point.x)*tsmin;
        _vertData[vstart+ii].texcoord.y = point.y*ttmax+(1-point.y)*ttmin;
        _vertData[vstart+ii].color = (_gradient == nullptr) ? (Vec4)_color : Vec4(_vertData[vstart+ii].texcoord,0,0);
        
        ii++;
    }
    
    int jj = 0;
    unsigned int istart = _indxSize;
    for(auto it = poly.indices().begin(); it != poly.indices().end(); ++it) {
        _indxData[istart+jj] = vstart+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given rectangle to the vertex buffer, but does not
 * draw it yet.  You must call {@link #flush} or {@link #end} to draw the 
 * rectangle. This method will automatically flush if the maximum number
 * of vertices is reached.
 *
 * All vertices will be uniformly transformed by the transform matrix.
 * If depth testing is on, all vertices will use the current sprite
 * batch depth.
 *
 * @param rect  The rectangle to add to the buffer
 * @param mat  	The transform to apply to the vertices
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Rect rect, const Mat4& mat) {
    if (_vertSize+4 > _vertMax ||  _indxSize+8 > _indxMax) {
        flush();
    }

    Texture* texture = _context->texture.get();
    float tsmax, tsmin;
    float ttmax, ttmin;
    
    if (texture != nullptr) {
        tsmax = texture->getMaxS();
        tsmin = texture->getMinS();
        ttmax = texture->getMaxT();
        ttmin = texture->getMinT();
    } else {
        tsmax = 1.0f; tsmin = 0.0f;
        ttmax = 1.0f; ttmin = 0.0f;
    }
    
    setUniformBlock(_context,true);
    Poly2 poly(rect, _context->command == GL_TRIANGLES);
    unsigned int vstart = _vertSize;
    int ii = 0;
    for(auto it = poly.vertices().begin(); it != poly.vertices().end(); ++it) {
        Vec3 point = Vec3((*it),_depth);
        _vertData[vstart+ii].position = point*mat;
        
        point.x = (point.x-rect.origin.x)/rect.size.width;
        point.y = 1-(point.y-rect.origin.y)/rect.size.height;
        _vertData[vstart+ii].texcoord.x = point.x*tsmax+(1-point.x)*tsmin;
        _vertData[vstart+ii].texcoord.y = point.y*ttmax+(1-point.y)*ttmin;
        _vertData[vstart+ii].color = (_gradient == nullptr) ? (Vec4)_color : Vec4(_vertData[vstart+ii].texcoord,0,0);
        
        ii++;
    }
    
    int jj = 0;
    unsigned int istart = _indxSize;
    for(auto it = poly.indices().begin(); it != poly.indices().end(); ++it) {
        _indxData[istart+jj] = vstart+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given polygon to the vertex buffer, but does not
 * draw it yet.  You must call {@link #flush} or {@link #end} to draw the 
 * polygon. This method will automatically flush if the maximum number
 * of vertices is reached.
 *
 * If depth testing is on, all vertices will use the current sprite
 * batch depth.
 *
 * @param poly  The polygon to add to the buffer
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Poly2& poly) {
    CUAssertLog(_context->command == GL_TRIANGLES ?
                 poly.indices().size() % 3 == 0 :
                 poly.indices().size() % 2 == 0,
                "Polynomial has the wrong number of indices: %d", (int)poly.indices().size());
    if (poly.vertices().size() >= _vertMax || poly.indices().size()  >= _indxMax) {
        return chunkify(poly,Mat4::IDENTITY);
    } else if (_vertSize+poly.vertices().size() > _vertMax ||
               _indxSize+poly.indices().size()  > _indxMax) {
        flush();
    }

    Texture* texture = _context->texture.get();
    float twidth, theight;
    float tsmax, tsmin;
    float ttmax, ttmin;
    
    if (texture != nullptr) {
        twidth  = texture->getWidth();
        theight = texture->getHeight();
        tsmax = texture->getMaxS();
        tsmin = texture->getMinS();
        ttmax = texture->getMaxT();
        ttmin = texture->getMinT();
    } else {
        twidth  = poly.getBounds().size.width;
        theight = poly.getBounds().size.height;
        tsmax = 1.0f; tsmin = 0.0f;
        ttmax = 1.0f; ttmin = 0.0f;
    }
    
    setUniformBlock(_context,true);
    unsigned int vstart = _vertSize;
    int ii = 0;
    for(auto it = poly.vertices().begin(); it != poly.vertices().end(); ++it) {
        Vec3 point = Vec3((*it),_depth);
        _vertData[vstart+ii].position = point;
        
        point.x /= twidth;
        point.y = 1-point.y/theight;
        _vertData[vstart+ii].texcoord.x = point.x*tsmax+(1-point.x)*tsmin;
        _vertData[vstart+ii].texcoord.y = point.y*ttmax+(1-point.y)*ttmin;
        _vertData[vstart+ii].color = (_gradient == nullptr) ? (Vec4)_color : Vec4(_vertData[vstart+ii].texcoord,0,0);
        
        ii++;
    }
    
    int jj = 0;
    unsigned int istart = _indxSize;
    for(auto it = poly.indices().begin(); it != poly.indices().end(); ++it) {
        _indxData[istart+jj] = vstart+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given polygon to the vertex buffer, but does not
 * draw it yet.  You must call {@link #flush} or {@link #end} to draw the 
 * polygon. This method will automatically flush if the maximum number
 * of vertices is reached.
 *
 * All vertices will be uniformly offset by the given vector. If depth
 * testing is on, all vertices will use the current sprite batch depth.

 *
 * @param poly  The polygon to add to the buffer
 * @param off  	The offset to apply to the vertices
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Poly2& poly, const Vec2 off) {
    CUAssertLog(_context->command == GL_TRIANGLES ?
                 poly.indices().size() % 3 == 0 :
                 poly.indices().size() % 2 == 0,
                "Polynomial has the wrong number of indices: %d", (int)poly.indices().size());
    if (poly.vertices().size() >= _vertMax || poly.indices().size()  >= _indxMax) {
        Mat4 matrix;
        matrix.translate((Vec3)off);
        return chunkify(poly,matrix);
    } else if (_vertSize+poly.vertices().size() > _vertMax ||
               _indxSize+poly.indices().size()  > _indxMax) {
        flush();
    }

    Texture* texture = _context->texture.get();
    float twidth, theight;
    float tsmax, tsmin;
    float ttmax, ttmin;
    
    if (texture != nullptr) {
        twidth  = texture->getWidth();
        theight = texture->getHeight();
        tsmax = texture->getMaxS();
        tsmin = texture->getMinS();
        ttmax = texture->getMaxT();
        ttmin = texture->getMinT();
    } else {
        twidth  = poly.getBounds().size.width;
        theight = poly.getBounds().size.height;
        tsmax = 1.0f; tsmin = 0.0f;
        ttmax = 1.0f; ttmin = 0.0f;
    }
    
    setUniformBlock(_context,true);
    unsigned int vstart = _vertSize;
    int ii = 0;
    Vec3 off1 = Vec3(off);
    for(auto it = poly.vertices().begin(); it != poly.vertices().end(); ++it) {
        Vec3 point = Vec3((*it),_depth);
        _vertData[vstart+ii].position = point+off1;
        
        point.x /= twidth;
        point.y = 1-point.y/theight;
        _vertData[vstart+ii].texcoord.x = point.x*tsmax+(1-point.x)*tsmin;
        _vertData[vstart+ii].texcoord.y = point.y*ttmax+(1-point.y)*ttmin;
        _vertData[vstart+ii].color = (_gradient == nullptr) ? (Vec4)_color : Vec4(_vertData[vstart+ii].texcoord,0,0);
        
        ii++;
    }
    
    int jj = 0;
    unsigned int istart = _indxSize;
    for(auto it = poly.indices().begin(); it != poly.indices().end(); ++it) {
        _indxData[istart+jj] = vstart+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given polygon to the vertex buffer, but does not
 * draw it yet.  You must call {@link #flush} or {@link #end} to draw the 
 * polygon. This method will automatically flush if the maximum number
 * of vertices is reached.
 *
 * All vertices will be uniformly transformed by the transform matrix.
 * If depth testing is on, all vertices will use the current sprite
 * batch depth.
 *
 * @param poly  The polygon to add to the buffer
 * @param mat  	The transform to apply to the vertices
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Poly2& poly, const Mat4& mat) {
    CUAssertLog(_context->command == GL_TRIANGLES ?
                 poly.indices().size() % 3 == 0 :
                 poly.indices().size() % 2 == 0,
                "Polynomial has the wrong number of indices: %d", (int)poly.indices().size());
    if (poly.vertices().size() >= _vertMax || poly.indices().size()  >= _indxMax) {
        return chunkify(poly,mat);
    } else if (_vertSize+poly.vertices().size() > _vertMax ||
               _indxSize+poly.indices().size()  > _indxMax) {
        flush();
    }

    Texture* texture = _context->texture.get();
    float twidth, theight;
    float tsmax, tsmin;
    float ttmax, ttmin;
    
    if (texture != nullptr) {
        twidth  = texture->getWidth();
        theight = texture->getHeight();
        tsmax = texture->getMaxS();
        tsmin = texture->getMinS();
        ttmax = texture->getMaxT();
        ttmin = texture->getMinT();
    } else {
        twidth  = poly.getBounds().size.width;
        theight = poly.getBounds().size.height;
        tsmax = 1.0f; tsmin = 0.0f;
        ttmax = 1.0f; ttmin = 0.0f;
    }
    
    setUniformBlock(_context,true);
    unsigned int vstart = _vertSize;
    int ii = 0;
    for(auto it = poly.vertices().begin(); it != poly.vertices().end(); ++it) {
        Vec3 point = Vec3((*it),_depth);
        _vertData[vstart+ii].position = point*mat;
        
        point.x /= twidth;
        point.y = 1-point.y/theight;
        _vertData[vstart+ii].texcoord.x = point.x*tsmax+(1-point.x)*tsmin;
        _vertData[vstart+ii].texcoord.y = point.y*ttmax+(1-point.y)*ttmin;
        _vertData[vstart+ii].color = (_gradient == nullptr) ? (Vec4)_color : Vec4(_vertData[vstart+ii].texcoord,0,0);
        ii++;
    }
    
    int jj = 0;
    unsigned int istart = _indxSize;
    for(auto it = poly.indices().begin(); it != poly.indices().end(); ++it) {
        _indxData[istart+jj] = vstart+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method is an alternate version of {@link #prepare} for the same
 * arguments.  It runs slower (e.g. the compiler cannot easily optimize
 * the loops) but it is guaranteed to work on any size polygon.  This
 * is important for avoiding memory corruption.  Unlike the perpare methods,
 * this method is guaranteed to flush, draining the vertex buffer.
 *
 * All vertices will be uniformly transformed by the transform matrix.
 * If depth testing is on, all vertices will use the current sprite
 * batch depth.
 *
 * @param poly  The polygon to add to the buffer
 * @param mat   The transform to apply to the vertices
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::chunkify(const Poly2& poly, const Mat4& mat) {
    Texture* texture = _context->texture.get();
    std::unordered_map<Uint32, Uint32> offsets;
    const std::vector<cugl::Vec2> vertices  = poly.vertices();
    const std::vector<Uint32> indices = poly.indices();
    
    setUniformBlock(_context,true);
    int chunksize = _context->command == GL_TRIANGLES ? 3 : 2;
    unsigned int start = _indxSize;

    float twidth, theight;
    float tsmax, tsmin;
    float ttmax, ttmin;
    
    if (texture != nullptr) {
        twidth  = texture->getWidth();
        theight = texture->getHeight();
        tsmax = texture->getMaxS();
        tsmin = texture->getMinS();
        ttmax = texture->getMaxT();
        ttmin = texture->getMinT();
    } else {
        twidth  = poly.getBounds().size.width;
        theight = poly.getBounds().size.height;
        tsmax = 1.0f; tsmin = 0.0f;
        ttmax = 1.0f; ttmin = 0.0f;
    }

    for(int ii = 0;  ii < indices.size(); ii += chunksize) {
        if (_indxSize+chunksize > _indxMax || _vertSize+chunksize > _vertMax) {
            flush();
            offsets.clear();
        }
        
        for(int jj = 0; jj < chunksize; jj++) {
            auto search = offsets.find(indices[ii+jj]);
            if (search != offsets.end()) {
                _indxData[_indxSize] = search->second;
            } else {
                Vec3 point = Vec3(vertices[indices[ii+jj]],_depth);
                _indxData[_indxSize] = _vertSize;
                _vertData[_vertSize].position = point*mat;
                
                point.x /= twidth;
                point.y = 1-point.y/theight;
                _vertData[_vertSize].texcoord.x = point.x*tsmax+(1-point.x)*tsmin;
                _vertData[_vertSize].texcoord.y = point.y*ttmax+(1-point.y)*ttmin;
                _vertData[_vertSize].color = (_gradient == nullptr) ? (Vec4)_color : Vec4(_vertData[_vertSize].texcoord,0,0);
                offsets[indices[ii+jj]] = _vertSize;
                _vertSize++;
            }
            _indxSize++;
        }
    }

    _inflight = true;
    return (unsigned int)indices.size()+start;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given mesh (both vertices and indices) to the
 * vertex buffer, but does not draw it.  You must call {@link #flush}
 * or {@link #end} to draw the complete mesh. This method will automatically
 * flush if the maximum number of vertices (or uniform blocks) is reached.
 *
 * If depth testing is on, all vertices will use the current sprite
 * batch depth.
 *
 * @param mesh  The mesh to add to the buffer
 * @param mat   The transform to apply to the vertices
 * @param tint  Whether to tint with the active color
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Mesh<SpriteVertex2>& mesh, const Mat4& mat, bool tint) {
    CUAssertLog(mesh.isSliceable(), "Sprite batches only support sliceable meshes");
    if (mesh.vertices.size() >= _vertMax || mesh.indices.size() >= _indxMax) {
        return chunkify(mesh, mat, tint);
    } else if(_vertSize+mesh.vertices.size() > _vertMax || _indxSize+mesh.indices.size() > _indxMax) {
        flush();
    }
    
    setUniformBlock(_context,tint);
    int ii = 0;
    for(auto it = mesh.vertices.begin(); it != mesh.vertices.end(); ++it) {
        _vertData[_vertSize+ii].position = Vec3(it->position,_depth);
        _vertData[_vertSize+ii].color = it->color;
        _vertData[_vertSize+ii].texcoord = it->texcoord;
        _vertData[_vertSize+ii].position *= mat;
        if (tint && _gradient == nullptr) {
            _vertData[_vertSize+ii].color *= _color;
        }
        ii++;
    }
    
    int jj = 0;
    for(auto it = mesh.indices.begin(); it != mesh.indices.end(); ++it) {
        _indxData[_indxSize+jj] = _vertSize+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method is an alternate version of {@link #prepare} for the same
 * arguments.  It runs slower (e.g. the compiler cannot easily optimize
 * the loops) but it is guaranteed to work on any size array.  This
 * is important for avoiding memory corruption.  Unlike the perpare methods,
 * this method is guaranteed to flush, draining the vertex buffer.
 *
 * @param mesh  The mesh to add to the buffer
 * @param mat   The transform to apply to the vertices
 * @param tint  Whether to tint with the active color
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::chunkify(const Mesh<SpriteVertex2>& mesh, const Mat4& mat, bool tint) {
    std::unordered_map<Uint32, Uint32> offsets;
    
    setUniformBlock(_context,tint);
    int chunksize = _context->command == GL_TRIANGLES ? 3 : 2;
    unsigned int start = _indxSize;
    
    for(int ii = 0;  ii < mesh.indices.size(); ii += chunksize) {
        if (_indxSize+chunksize > _indxMax || _vertSize+chunksize > _vertMax) {
            flush();
            offsets.clear();
        }
        
        for(int jj = 0; jj < chunksize; jj++) {
            auto search = offsets.find(mesh.indices[ii+jj]);
            if (search != offsets.end()) {
                _indxData[_indxSize] = search->second;
            } else {
                _indxData[_indxSize] = _vertSize;
                _vertData[_vertSize].position = Vec3(mesh.vertices[ii+jj].position,_depth);
                _vertData[_vertSize].color = mesh.vertices[ii+jj].color;
                _vertData[_vertSize].texcoord = mesh.vertices[ii+jj].texcoord;
                _vertData[_vertSize].position *= mat;
                if (tint && _gradient == nullptr) {
                    _vertData[_vertSize].color *= _color;
                }
                _vertSize++;
            }
            _indxSize++;
        }
    }

    _inflight = true;
    return (unsigned int)(mesh.indices.size()+start);
}

/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method adds the given mesh (both vertices and indices) to the
 * vertex buffer, but does not draw it.  You must call {@link #flush}
 * or {@link #end} to draw the complete mesh. This method will automatically
 * flush if the maximum number of vertices (or uniform blocks) is reached.
 *
 * @param mesh  The mesh to add to the buffer
 * @param mat   The transform to apply to the vertices
 * @param tint  Whether to tint with the active color
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::prepare(const Mesh<SpriteVertex3>& mesh, const Mat4& mat, bool tint) {
    CUAssertLog(mesh.isSliceable(), "Sprite batches only support sliceable meshes");
    if (mesh.vertices.size() >= _vertMax || mesh.indices.size() >= _indxMax) {
        return chunkify(mesh, mat, tint);
    } else if(_vertSize+mesh.vertices.size() > _vertMax || _indxSize+mesh.indices.size() > _indxMax) {
        flush();
    }
    
    setUniformBlock(_context,tint);
    int ii = 0;
    for(auto it = mesh.vertices.begin(); it != mesh.vertices.end(); ++it) {
        _vertData[_vertSize+ii] = *it;
        _vertData[_vertSize+ii].position *= mat;
        if (tint && _gradient == nullptr) {
            _vertData[_vertSize+ii].color *= _color;
        }
        ii++;
    }
    
    int jj = 0;
    for(auto it = mesh.indices.begin(); it != mesh.indices.end(); ++it) {
        _indxData[_indxSize+jj] = _vertSize+(*it);
        jj++;
    }
    
    _vertSize += ii;
    _indxSize += jj;
    _inflight = true;
    return ii;
}
/**
 * Returns the number of vertices added to the drawing buffer.
 *
 * This method is an alternate version of {@link #prepare} for the same
 * arguments.  It runs slower (e.g. the compiler cannot easily optimize
 * the loops) but it is guaranteed to work on any size array.  This
 * is important for avoiding memory corruption.  Unlike the perpare methods,
 * this method is guaranteed to flush, draining the vertex buffer.
 *
 * @param mesh  The mesh to add to the buffer
 * @param mat   The transform to apply to the vertices
 * @param tint  Whether to tint with the active color
 *
 * @return the number of vertices added to the drawing buffer.
 */
unsigned int SpriteBatch::chunkify(const Mesh<SpriteVertex3>& mesh, const Mat4& mat, bool tint) {
    std::unordered_map<Uint32, Uint32> offsets;
    
    setUniformBlock(_context,tint);
    int chunksize = _context->command == GL_TRIANGLES ? 3 : 2;
    unsigned int start = _indxSize;
    
    for(int ii = 0;  ii < mesh.indices.size(); ii += chunksize) {
        if (_indxSize+chunksize > _indxMax || _vertSize+chunksize > _vertMax) {
            flush();
            offsets.clear();
        }
        
        for(int jj = 0; jj < chunksize; jj++) {
            auto search = offsets.find(mesh.indices[ii+jj]);
            if (search != offsets.end()) {
                _indxData[_indxSize] = search->second;
            } else {
                _indxData[_indxSize] = _vertSize;
                _vertData[_vertSize] = mesh.vertices[ii+jj];
                _vertData[_vertSize].position *= mat;
                if (tint && _gradient == nullptr) {
                    _vertData[_vertSize].color *= _color;
                }
                _vertSize++;
            }
            _indxSize++;
        }
    }

    _inflight = true;
    return (unsigned int)(mesh.indices.size()+start);
}


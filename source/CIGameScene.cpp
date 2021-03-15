//
//  CIGameScene.cpp
//  CoreImpact
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It is a relativeluy simple class as we are not
//  worried about collisions.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  Author: Walker White
//  Version: 1/10/17
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "CIGameScene.h"
#include "CICollisionController.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

/** The parallax for each layer */
#define PARALLAX_AMT 0.1f

/** Maximum number of stardusts allowed on screen at a time. */
#define MAX_STARDUST 512

#pragma mark -
#pragma mark Constructors

/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
	// Initialize the scene to a locked width
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
	if (assets == nullptr) {
		return false;
	}
	else if (!Scene2::init(dimen)) {
		return false;
	}

	// Start up the input handler
	_assets = assets;
	_input.init(getBounds());

	// Acquire the scene built by the asset loader and resize it the scene
	auto scene = _assets->get<scene2::SceneNode>("game");
	scene->setContentSize(dimen);
	scene->doLayout(); // Repositions the HUD

	// Get the scene components.
	_allSpace = _assets->get<scene2::SceneNode>("game_field");
	_farSpace = _assets->get<scene2::SceneNode>("game_field_far");
	_nearSpace = _assets->get<scene2::SceneNode>("game_field_near");
	_massHUD = std::dynamic_pointer_cast<scene2::Label>(_assets->get<scene2::SceneNode>("game_hud"));

	addChild(scene);

	// Create the planet model
	_planet = PlanetModel::alloc(dimen.width / 2, dimen.height / 2, CIColor::blue, 3);
	auto planetTexture = _assets->get<Texture>("planet1");
	_planet->setTexture(planetTexture);

	addChild(_planet->getPlanetNode());

	// TEMP
	std::shared_ptr<StardustNode> _stardustNode = StardustNode::alloc();

	_stardustContainer = StardustQueue::alloc(MAX_STARDUST, _stardustNode);
	_stardustContainer->setTexture(_assets->get<Texture>("photon"));
	_stardustContainer->addStardust(dimen);
	_stardustContainer->addStardust(dimen);
	_stardustContainer->addStardust(dimen);
	_stardustContainer->addStardust(dimen);

	_stardustNode->setQueue(&(_stardustContainer->getQueue()));

	//_stardustContainer = StardustQueue::alloc(MAX_STARDUST);
	//_stardustContainer->setTexture(_assets->get<Texture>("photon"));

	//std::shared_ptr<StardustNode> _stardustNode = _stardustContainer->getStardustNode();

	//_stardustContainer->addStardust(dimen);
	//_stardustContainer->addStardust(dimen);
	//_stardustContainer->addStardust(dimen);
	//_stardustContainer->addStardust(dimen);

	//try {
	//	addChild(scene);
	//}
	//catch (...) {
	//	std::cout << "FAILED 1234" << "\n";

	//	CULog("FAILED 1");
	//}

	// TEMP
	try {
		addChild(_stardustNode, 1);

		//assert(false);
	}
	catch (...) {
		std::cout << "FAILED 22222\n";
		CULog("FAILED 2");
	}

	//try {
	//	addChild(_planet->getPlanetNode());
	//}
	//catch (...) {
	//	CUAssertLog(false, "FAILED 3");
	//}

	return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
	if (_active) {
		removeAllChildren();
		_input.dispose();
		_allSpace = nullptr;
		_farSpace = nullptr;
		_nearSpace = nullptr;
		_planet = nullptr;
		_active = false;
	}
}

#pragma mark -
#pragma mark Gameplay Handling

/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
	// Reset the planet and input
	_input.clear();

	// Reset the parallax
	Vec2 position = _farSpace->getPosition();
	_farSpace->setAnchor(Vec2::ANCHOR_CENTER);
	_farSpace->setPosition(position);
	_farSpace->setAngle(0.0f);
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width;
	_input.update(timestep);

	_stardustContainer->update();

	if (rand() % 60 == 0) {
		_stardustContainer->addStardust(dimen);
	}

	//    _massHUD->setText(to_string(_planet->getMass()));
	_massHUD->setText(to_string(_planet->getMass()) + "; " + CIColor::getStringValue(_planet->getColor()));

	if (_input.getDidLongPress()) {
		// lock in
		collisions::checkForCollision(true, _input.getPosition(), _planet);
		_input.setPressedPlanet(false);
	}
	else if (_input.fingerDown()) {
		bool isPressingPlanet = collisions::checkForCollision(false, _input.getPosition(), _planet);
		_input.setPressedPlanet(isPressingPlanet);
	}

	collisions::checkForCollision(_planet, _stardustContainer);
	collisions::checkForCollision(_input.getPosition(), _stardustContainer);
	collisions::checkInBounds(_stardustContainer, dimen);
}
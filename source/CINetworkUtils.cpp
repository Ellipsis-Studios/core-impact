//
//  CINetworkUtils.cpp
//  CoreImpact
//
// This class contains functions to help out with networking stuff such as encoding/decoding bytes.
//
//  Created by William Long on 3/21/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#include "CINetworkUtils.h"
#include "CILocation.h"

/**
 * Decodes 12 bytes into a 12 character string.
 */
string NetworkUtils::decodeString(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6,
    uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10, uint8_t c11, uint8_t c12) {
    std::vector<uint8_t> data{ c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12 };
    std::string s(data.begin(), data.end());
    //return cugl::marshall(s);
    return s;
}

/**
 * Encodes the string s into 12 bytes and appends those bytes onto out.
 */
void NetworkUtils::encodeString(string s, std::vector<uint8_t>& out) {
    s.resize(12);
    std::vector<uint8_t> data(s.begin(), s.end());
    for (int i = 0; i < data.size(); i++) {
        out.push_back(data[i]);
    }
}

/**
 * Decodes 4 bytes into a float.
 */
float NetworkUtils::decodeFloat(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4) {
    float f;
    std::vector<uint8_t> data{m1, m2, m3, m4};
    // make sure the vector is the right size
    if (data.size() != sizeof(f))
        throw std::runtime_error{"Size of data in vector and float do not match"};
    // copy the bytes into the float
    std::memcpy(&f, data.data(), sizeof(f));
    return cugl::marshall(f);
}

/**
 * Encodes the float f into 4 bytes and appends those bytes onto out.
 */
void NetworkUtils::encodeFloat(float f, std::vector<uint8_t>& out) {
    float marshalled = cugl::marshall(f);
    std::vector<uint8_t> data(sizeof(marshalled));
    std::memcpy(data.data(), &marshalled, sizeof(marshalled));
    for (int i = 0; i < data.size(); i++) {
        out.push_back(data[i]);
    }
}

/**
 * Decodes 4 bytes into an int.
 */
int NetworkUtils::decodeInt(uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4) {
    int i;
    std::vector<uint8_t> data{i1, i2, i3, i4};
    // make sure the vector is the right size
    if (data.size() != sizeof(i))
        throw std::runtime_error{"Size of data in vector and int do not match"};
    // copy the bytes into the float
    std::memcpy(&i, data.data(), sizeof(i));
    return cugl::marshall(i);
}

/**
 * Encodes x into 4 bytes and appends those bytes onto out.
 */
void NetworkUtils::encodeInt(int x, std::vector<uint8_t>& out) {
    Sint32 marshalled = cugl::marshall(x);
    std::vector<uint8_t> data(sizeof(marshalled));
    std::memcpy(data.data(), &marshalled, sizeof(marshalled));
    for (int i = 0; i < data.size(); i++) {
        out.push_back(data[i]);
    }
}

/**
 * Gets the stardust location given our player id and the player id of the opponent.
 */
CILocation::Value NetworkUtils::getLocation(int playerID, int opponentPlayerID) {
    int location = (opponentPlayerID < playerID ? opponentPlayerID + 1 : opponentPlayerID);
    return CILocation::Value(location);
}

/**
 * Returns an opponents player id given this player's id and a location
 */
int NetworkUtils::getOpponentPlayerID(int playerID, CILocation::Value location) {
    return (location <= playerID ? location - 1 : location);
}

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

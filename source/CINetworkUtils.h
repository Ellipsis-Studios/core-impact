//
//  CINetworkUtils.h
//  CoreImpact
//
// This class contains functions to help out with networking stuff such as encoding/decoding bytes.
//
//  Created by William Long on 3/21/21.
//  Copyright © 2021 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef __CI_NETWORK_UTILS_H__
#define __CI_NETWORK_UTILS_H__

#include <cugl/cugl.h>

class NetworkUtils {
private:
    /** IP of the NAT punchthrough server */
    static constexpr auto SERVER_ADDRESS = "34.74.68.73";
    
    /** Port of the NAT punchthrough server */
    static constexpr uint16_t SERVER_PORT = 61111;
    
public:
    /**
     * The types of network messages that can be sent.
     */
    enum MessageType {
        StartGame = 1,
        StardustSent = 2,
        PlanetUpdate = 3,
        AttemptToWin = 4,
        WonGame = 5
    };
    
    /**
     * Decodes 4 bytes into a float.
     */
    static float decodeFloat(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4);
    
    /**
     * Encodes the float f into 4 bytes and appends those bytes onto out.
     */
    static void encodeFloat(float f, std::vector<uint8_t>& out);
    
    /**
     * Decodes 4 bytes into an int.
     */
    static int decodeInt(uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4);
    
    /**
     * Encodes x into 4 bytes and appends those bytes onto out.
     */
    static void encodeInt(int x, std::vector<uint8_t>& out);
    
    /**
     * Returns the connection config objects to connect to NAT Punchthrough Server.
     */
    static cugl::CUNetworkConnection::ConnectionConfig getConnectionConfig() {
        return {
            SERVER_ADDRESS,
            SERVER_PORT,
            4,
            0
        };
    }
};

#endif /* __CI_NETWORK_UTILS_H__ */

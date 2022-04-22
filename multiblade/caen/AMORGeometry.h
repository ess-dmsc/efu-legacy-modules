/* Copyright (C) 2017-2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief AMOR mixed 1D/2D mode geometry
/// Check out the ICD for explanation
//===----------------------------------------------------------------------===//

#pragma once

#include <common/debug/Trace.h>
#include <cinttypes>
#include <stdio.h>

// #undef TRC_LEVEL
// #define TRC_LEVEL TRC_L_DEB

class AMORGeometry {
public:
  const int MonitorChannel{63};
  const int MonitorCassette{0};
  const int Cassette2D{10};


  bool is1DDetector(int Cassette) {
    return (Cassette < Cassette2D);
  }

  bool isValidChannel(uint16_t Channel) {
    return (Channel <= 63);
  }

  bool isMonitor(int Cassette, uint16_t Channel) {
    return ((Cassette == MonitorCassette) and (Channel == MonitorChannel));
  }

  bool isXCoord(int Cassette, uint16_t Channel) {
    // Cassettes 0 - 9 are y coordinates
    if (Cassette < Cassette2D) {
      return false;
    }

    // Cassette 10 is either x- or y-
    return (Channel >= 32);
  }

  bool isYCoord(int Cassette, uint16_t Channel) {
    // Cassettes 0 - 9 are y coordinates
    if (Cassette < Cassette2D) {
      return true;
    }

    // Cassette 10 is either x- or y-
    return (Channel <= 31);
  }


  int getXCoord(int Cassette, uint16_t Channel) {
    if (Cassette < Cassette2D) {
      return 0;
    }

    return Channel - 32;
  }

int getYCoord(int Cassette, uint16_t Channel) {
    if (not isYCoord(Cassette, Channel)) {
      XTRACE(EVENT, DEB, "Not Y channel - Cassette %d, Channel; %d",
             Cassette, Channel);
      return -1;
    }

    int Wire = Channel + 1;
    if (is1DDetector(Cassette) and (not (Cassette & 0x01))) {
        Wire = Channel - 31;
    }

    int res = (Cassette * 32) + 32 - Wire;
    XTRACE(EVENT, DEB, "Cassette %d, Channel %d -> Wire %d, y %d",
            Cassette, Channel, Wire, res);

    return res;
  }

  uint8_t getPlane(int Cassette, uint16_t Channel) {
    return isXCoord(Cassette, Channel) ^ 0x1; // x == 0, y == 1
  }

};

// Copyright (C) 2020 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Multiblade application counters (for Grafana and low level debug)
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cinttypes>

struct Counters {
    // Input Counters - accessed in input thread
    int64_t RxPackets;
    int64_t RxBytes;
    int64_t RxIdle;
    int64_t FifoPushErrors;
    int64_t PaddingFor64ByteAlignment[4]; // cppcheck-suppress unusedStructMember

    // Processing Counters - accessed in processing thread
    int64_t FifoSeqErrors;
    int64_t PacketsErrorVersion;
    int64_t PacketsSeqErrors;
    int64_t ReadoutsErrorBytes;
    int64_t PacketBadDigitizer;
    int64_t ReadoutsCount;
    int64_t ReadoutsGood;
    int64_t ReadoutsInvalidAdc;
    int64_t ReadoutsInvalidChannel;
    int64_t ReadoutsInvalidPlane;
    int64_t ReadoutsTOFLarge;
    int64_t ReadoutsTimerWraps;
    int64_t Readouts1DY;
    int64_t Readouts2DX;
    int64_t Readouts2DY;
    int64_t ReadoutsDiscardStrips;
    int64_t ReadoutsDiscard1D;
    int64_t MonitorCount;
    int64_t MonitorTxBytes;
    int64_t ProcessingIdle;
    int64_t Events;
    int64_t Events2D;
    int64_t Events1D;
    int64_t Events1DDiscard;
    int64_t EventsNoCoincidence;
    int64_t EventsWireMultTwoPlus;
    int64_t EventsWireMultSingle;
    int64_t EventsMatchedClusters;
    int64_t EventsInvalidStripGap;
    int64_t EventsWireGapCount;
    int64_t EventsWireGapInvalid;
    int64_t EventsMaxTofNS;
    int64_t GeometryErrors;
    int64_t TxBytes;

    // Kafka stats below are common to all detectors
    int64_t kafka_produce_fails;
    int64_t kafka_ev_errors;
    int64_t kafka_ev_others;
    int64_t kafka_dr_errors;
    int64_t kafka_dr_noerrors;
  } __attribute__((aligned(64)));

// Copyright (C) 2020 - 2022 European Spallation Source, ERIC. See LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Separating Multigrid processing from pipeline main loop
///
//===----------------------------------------------------------------------===//

#include <common/debug/Log.h>
#include <common/debug/Trace.h>
#include <common/time/TimeString.h>
#include <multiblade/MBCaenInstrument.h>

// #undef TRC_LEVEL
// #define TRC_LEVEL TRC_L_DEB

namespace Multiblade {

/// \brief load configuration and calibration files
MBCaenInstrument::MBCaenInstrument(struct Counters & counters,
    BaseSettings & EFUSettings,
    CAENSettings &moduleSettings)
      : counters(counters)
      , ModuleSettings(moduleSettings) {


    // Setup Instrument according to configuration file
    config = Config(ModuleSettings.ConfigFile);

    if (!moduleSettings.FilePrefix.empty()) {
      dumpfile = ReadoutFile::create(moduleSettings.FilePrefix + "-" + timeString());
    }

    ncass = 11;

    essgeom = ESSGeometry(32, 352, 1, 1);
    topic = "amor_detector";

    builders = std::vector<EventBuilder>(ncass);
    for (EventBuilder & builder : builders) {
      builder.setTimeBox(2010);
    }

    // Kafka producers and flatbuffer serialisers
    // Monitor producer
    Producer monitorprod(EFUSettings.KafkaBroker, monitor);
    auto ProduceHist = [&monitorprod](auto DataBuffer, auto Timestamp) {
      monitorprod.produce(DataBuffer, Timestamp);
    };
    histfb.set_callback(ProduceHist);
    histograms = Hists(std::max(ncass * nwires, ncass * nstrips), 65535);
    histfb = HistogramSerializer(histograms.needed_buffer_size(), "multiblade");
}


// Moved from MBCaenBase to better support unit testing
bool MBCaenInstrument::parsePacket(char * data, int length,  EV42Serializer & ev42ser) {

  int res = parser.parse(data, length);

  counters.ReadoutsErrorBytes += parser.Stats.error_bytes;
  counters.ReadoutsErrorVersion += parser.Stats.error_version;
  counters.ReadoutsSeqErrors += parser.Stats.seq_errors;

  if (res < 0) {
    return false;
  }

  XTRACE(DATA, DEB, "Received %d readouts from digitizer %d",
         parser.MBHeader->numElements, parser.MBHeader->digitizerID);

  counters.ReadoutsCount += parser.MBHeader->numElements;

  uint64_t efu_time = 1000000000LU * (uint64_t)time(NULL); // ns since 1970
  ev42ser.pulseTime(efu_time);

  if (dumpfile) {
    dumpfile->push(parser.readouts);
  }

  int DigitiserIndex = config.Mappings->digitiserIndex(parser.MBHeader->digitizerID);
  if (DigitiserIndex < 0) {
    XTRACE(DATA, WAR, "Invalid digitizerId: %d", parser.MBHeader->digitizerID);
    counters.PacketBadDigitizer++;
    return false;
  }

  FixJumpsAndSort(DigitiserIndex, parser.readouts);

  for (int cassette = 0; cassette <= 10; cassette++) {
    builders[cassette].flush();
  }

  return true;
}


int MBCaenInstrument::getCassette(int DigitiserIndex, uint8_t Channel) {
  if (DigitiserIndex == 5) {
    return 10;
  }

  if (Channel > 31) {
    return DigitiserIndex * 2;
  } else {
    return DigitiserIndex * 2 + 1;
  }
}


// New EF algorithm - Needed to sort readouts in time
bool compareByTime(const Readout &a, const Readout &b) {
  return a.local_time < b.local_time;
}

// New EF algorithm - buffers data according to time and sorts before
// processing
void MBCaenInstrument::FixJumpsAndSort(int DigitiserIndex, std::vector<Readout> &vec) {
  int64_t Gap{43'000'000};
  int64_t PrevTime{0xffffffffff};
  std::vector<Readout> temp;

  // Assume time gap detectino and data sorting can be done
  // per digitizer and not per cassette.
  for (auto &Readout : vec) {
    int64_t Time = (uint64_t)(Readout.local_time * config.TimeTickNS);

    if ((PrevTime - Time) < Gap) {
      temp.push_back(Readout);
    } else {
      XTRACE(CLUSTER, DEB, "Wrap: %4d, Time: %lld, PrevTime: %lld, diff %lld",
             counters.ReadoutsTimerWraps, Time, PrevTime, (PrevTime - Time));
      counters.ReadoutsTimerWraps++;
      std::sort(temp.begin(), temp.end(), compareByTime);
      LoadAndProcessReadouts(DigitiserIndex, temp);

      temp.clear();
      temp.push_back(Readout);
    }
    PrevTime = Time;
  }
  LoadAndProcessReadouts(DigitiserIndex, temp);
}

// Here readouts from the same digitizer can end up in different
// builders - one per cassette
void MBCaenInstrument::LoadAndProcessReadouts(int DigitiserIndex, std::vector<Readout> &vec) {
  for (Readout &dp : vec) {
    int Cassette = getCassette(DigitiserIndex, dp.channel);
    assert(Cassette <= 10);
    assert(Cassette >= 0);

    XTRACE(DATA, DEB, "time %u, channel %u, adc %u",
           dp.local_time, dp.channel, dp.adc);

    assert(dp.local_time * config.TimeTickNS < 0xffffffff);
    uint64_t Time = (uint64_t)(dp.local_time * config.TimeTickNS);

    if (amorgeom.isMonitor(Cassette, dp.channel)) {
      handleMonitorReadout(Time);
    } else {
      handleDetectorReadout(Cassette, dp, Time);
    }
  }

  for (auto & builder : builders) {
    builder.flush();
  }
}


void MBCaenInstrument::handleMonitorReadout(uint64_t Time) {
  XTRACE(DATA, DEB, "monitor data: Time %" PRIu64 "", Time);
  // Monitor coordinate, adc and placearbitrarily set to 0
  // we only care about the timestamp
  MonitorHits.push_back({Time, 0, 0, 0});
  counters.MonitorCount++;
}

void MBCaenInstrument::handleDetectorReadout(int Cassette, Readout & dp, uint64_t Time) {
  if (not amorgeom.isValidChannel(dp.channel)) {
    counters.ReadoutsInvalidChannel++;
    return;
  }

  if (dp.adc > config.MaxValidADC) {
    XTRACE(DATA, INF, "Bad ADC: Cassette %2u, Channel %2u, ADC %u",
       Cassette, dp.channel, dp.adc);
    counters.ReadoutsInvalidAdc++;
    return;
  }

  uint8_t plane = amorgeom.getPlane(Cassette, dp.channel);
  int coord;
  if (plane == 0) {
    coord = amorgeom.getXCoord(Cassette, dp.channel);
    //printf("Cassette %d, Channel: %u - x-coord %d\n", Cassette, dp.channel, coord);
  } else  if (plane == 1) {
    coord = amorgeom.getYCoord(Cassette, dp.channel);
    //printf("Cassette %d, Channel: %u - y-coord %d\n", Cassette, dp.channel, coord);
    assert(coord < 352);
  } else {
    counters.ReadoutsInvalidPlane++;
    return;
  }

  counters.ReadoutsGood++;

  if (amorgeom.is1DDetector(Cassette)) {
    counters.Readouts1D++;
  } else {
    counters.Readouts2D++;
  }

  /// \todo might be the place to skip strip channels in 1D mode
  builders[Cassette].insert({Time, (uint16_t)coord, dp.adc, plane});
}


} // namespace

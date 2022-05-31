// Copyright (C) 2018-2020 European Spallation Source, see LICENSE file
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Multi-Blade prototype detector base plugin interface definition
///
//===----------------------------------------------------------------------===//


#include <cinttypes>
#include <common/detector/EFUArgs.h>
#include <common/kafka/EV42Serializer.h>
#include <common/kafka/Producer.h>
#include <common/debug/Trace.h>
#include <common/time/TimeString.h>

#include <unistd.h>

#include <common/RuntimeStat.h>
#include <common/system/Socket.h>
#include <common/memory/SPSCFifo.h>
#include <common/time/TimeString.h>
#include <common/time/TSCTimer.h>
#include <common/time/Timer.h>
#include <multiblade/MBCaenBase.h>
#include <multiblade/MBCaenInstrument.h>
#include <unistd.h>

// #undef TRC_LEVEL
// #define TRC_LEVEL TRC_L_DEB

namespace Multiblade {

const char *classname = "Multiblade detector with CAEN readout";

CAENBase::CAENBase(BaseSettings const &settings, struct CAENSettings &LocalMBCAENSettings)
    : Detector("MBCAEN", settings), MBCAENSettings(LocalMBCAENSettings) {

  Stats.setPrefix(EFUSettings.GraphitePrefix, EFUSettings.GraphiteRegion);

  XTRACE(INIT, ALW, "Adding stats");
  // clang-format off
  Stats.create("receive.packets", Counters.RxPackets);
  Stats.create("receive.bytes", Counters.RxBytes);
  Stats.create("receive.idle", Counters.RxIdle);
  Stats.create("receive.dropped", Counters.FifoPushErrors);
  Stats.create("receive.fifo_seq_errors", Counters.FifoSeqErrors);

  Stats.create("headers.packet_bad_header", Counters.PacketBadDigitizer);
  Stats.create("headers.error_version", Counters.ReadoutsErrorVersion);
  Stats.create("headers.seq_errors", Counters.ReadoutsSeqErrors);

  Stats.create("readouts.count", Counters.ReadoutsCount);
  Stats.create("readouts.2D_x", Counters.Readouts2DX);
  Stats.create("readouts.2D_y", Counters.Readouts2DY);
  Stats.create("readouts.1D_y", Counters.Readouts1DY);
  Stats.create("readouts.discard.1D", Counters.ReadoutsDiscard1D);
  Stats.create("readouts.discard.strips", Counters.ReadoutsDiscardStrips);
  Stats.create("readouts.count_valid", Counters.ReadoutsGood);
  Stats.create("readouts.invalid_ch", Counters.ReadoutsInvalidChannel);
  Stats.create("readouts.invalid_adc", Counters.ReadoutsInvalidAdc);
  Stats.create("readouts.invalid_plane", Counters.ReadoutsInvalidPlane);
  Stats.create("readouts.large_tof", Counters.ReadoutsTOFLarge);
  Stats.create("readouts.timer_wraps", Counters.ReadoutsTimerWraps);
  Stats.create("readouts.error_bytes", Counters.ReadoutsErrorBytes);

  Stats.create("monitor.count", Counters.MonitorCount);
  Stats.create("monitor.txbytes", Counters.MonitorTxBytes);

  Stats.create("thread.processing_idle", Counters.ProcessingIdle);

  Stats.create("events.count", Counters.Events);
  Stats.create("events.2D", Counters.Events2D);
  Stats.create("events.1D", Counters.Events1D);
  Stats.create("events.1DDiscard", Counters.Events1DDiscard);
  Stats.create("events.geometry_errors", Counters.GeometryErrors);
  Stats.create("events.no_coincidence", Counters.EventsNoCoincidence);
  Stats.create("events.wiremult_single", Counters.EventsWireMultSingle);
  Stats.create("events.wiremult_twoplus", Counters.EventsWireMultTwoPlus);
  Stats.create("events.matched_clusters", Counters.EventsMatchedClusters);
  Stats.create("events.strip_gaps", Counters.EventsInvalidStripGap);
  Stats.create("events.wiregap.count", Counters.EventsWireGapCount);
  Stats.create("events.wiregap.invalid", Counters.EventsWireGapInvalid);
  Stats.create("events.max_tof_ns", Counters.EventsMaxTofNS);

  Stats.create("transmit.bytes", Counters.TxBytes);

  /// \todo below stats are common to all detectors and could/should be moved
  Stats.create("kafka.produce_fails", Counters.kafka_produce_fails);
  Stats.create("kafka.ev_errors", Counters.kafka_ev_errors);
  Stats.create("kafka.ev_others", Counters.kafka_ev_others);
  Stats.create("kafka.dr_errors", Counters.kafka_dr_errors);
  Stats.create("kafka.dr_others", Counters.kafka_dr_noerrors);

  Stats.create("memory.hitvec_storage.alloc_count", HitVectorStorage::Pool->Stats.AllocCount);
  Stats.create("memory.hitvec_storage.alloc_bytes", HitVectorStorage::Pool->Stats.AllocBytes);
  Stats.create("memory.hitvec_storage.dealloc_count", HitVectorStorage::Pool->Stats.DeallocCount);
  Stats.create("memory.hitvec_storage.dealloc_bytes", HitVectorStorage::Pool->Stats.DeallocBytes);
  Stats.create("memory.hitvec_storage.malloc_fallback_count", HitVectorStorage::Pool->Stats.MallocFallbackCount);

  Stats.create("memory.cluster_storage.alloc_count", ClusterPoolStorage::Pool->Stats.AllocCount);
  Stats.create("memory.cluster_storage.alloc_bytes", ClusterPoolStorage::Pool->Stats.AllocBytes);
  Stats.create("memory.cluster_storage.dealloc_count", ClusterPoolStorage::Pool->Stats.DeallocCount);
  Stats.create("memory.cluster_storage.dealloc_bytes", ClusterPoolStorage::Pool->Stats.DeallocBytes);
  Stats.create("memory.cluster_storage.malloc_fallback_count", ClusterPoolStorage::Pool->Stats.MallocFallbackCount);

  // clang-format on

  std::function<void()> inputFunc = [this]() { CAENBase::input_thread(); };
  Detector::AddThreadFunction(inputFunc, "input");

  std::function<void()> processingFunc = [this]() {
    CAENBase::processing_thread();
  };
  Detector::AddThreadFunction(processingFunc, "processing");

  XTRACE(INIT, ALW, "Creating %d Multiblade Rx ringbuffers of size %d",
         EthernetBufferMaxEntries, EthernetBufferSize);
}

void CAENBase::input_thread() {
  Socket::Endpoint local(EFUSettings.DetectorAddress.c_str(),
                         EFUSettings.DetectorPort);
  UDPReceiver receiver(local);
  receiver.setBufferSizes(EFUSettings.TxSocketBufferSize,
                          EFUSettings.RxSocketBufferSize);
  receiver.checkRxBufferSizes(EFUSettings.RxSocketBufferSize);
  receiver.printBufferSizes();
  receiver.setRecvTimeout(0, 100000); /// secs, usecs 1/10s

  for (;;) {
    int readSize;
    unsigned int rxBufferIndex = RxRingbuffer.getDataIndex();

    // this is the processing step
    RxRingbuffer.setDataLength(rxBufferIndex, 0);
    if ((readSize = receiver.receive(RxRingbuffer.getDataBuffer(rxBufferIndex),
                                   RxRingbuffer.getMaxBufSize())) > 0) {
      RxRingbuffer.setDataLength(rxBufferIndex, readSize);
      XTRACE(INPUT, DEB, "Received an udp packet of length %d bytes", readSize);
      Counters.RxPackets++;
      Counters.RxBytes += readSize;

      if (InputFifo.push(rxBufferIndex) == false) {
        Counters.FifoPushErrors++;
      } else {
        RxRingbuffer.getNextBuffer();
      }
    } else {
      Counters.RxIdle++;
    }

    // Checking for exit
    if (not runThreads) {
      XTRACE(INPUT, ALW, "Stopping input thread.");
      return;
    }
  }
}

void CAENBase::processing_thread() {

  MBCaenInstrument MBCaen(Counters, EFUSettings, MBCAENSettings);

  // Event producer
  Producer eventprod(EFUSettings.KafkaBroker, MBCaen.topic);
  auto Produce = [&eventprod](auto DataBuffer, auto Timestamp) {
    eventprod.produce(DataBuffer, Timestamp);
  };
  EV42Serializer events{KafkaBufferSize, "multiblade", Produce};

  // Event producer
  Producer monprod(EFUSettings.KafkaBroker, "amor_beam_monitor");
  auto ProduceII = [&monprod](auto DataBuffer, auto Timestamp) {
    monprod.produce(DataBuffer, Timestamp);
  };
  EV42Serializer monitor{KafkaBufferSize, "ttlmon0", ProduceII};

  unsigned int data_index;
  TSCTimer produce_timer(EFUSettings.UpdateIntervalSec * 1000000 * TSC_MHZ);
  Timer h5flushtimer;
  // Monitor these counters
  RuntimeStat RtStat({Counters.RxPackets, Counters.Events, Counters.TxBytes});


    while (true) {
      if (InputFifo.pop(data_index)) { // There is data in the FIFO - do processing
        auto datalen = RxRingbuffer.getDataLength(data_index);
        if (datalen == 0) {
          Counters.FifoSeqErrors++;
          continue;
        }

        /// \todo use the Buffer<T> class here and in parser
        auto dataptr = RxRingbuffer.getDataBuffer(data_index);

        uint64_t efu_time = 1000000000LU * (uint64_t)time(NULL); // ns since 1970
        events.pulseTime(efu_time);

        // \todo state some assumptions here
        if (not MBCaen.parseAndProcessPacket(dataptr, datalen, events)) {
          continue;
        }

        XTRACE(PROCESS, DEB, "Received %u monitor counts", MBCaen.MonitorHits.size());
        for (Hit & hit : MBCaen.MonitorHits) {
          Counters.MonitorTxBytes += monitor.addEvent(hit.time, 1);
        }
        MBCaen.MonitorHits.clear();

        //when alignment mode is active, process 2D events with event builders
        if(MBCaen.ModuleSettings.Alignment) {
          // Strips == X == ClusterA
          // Wires  == Y == ClusterB
          for (int Cassette = 0; Cassette <= MBCaen.amorgeom.Cassette2D; Cassette++) {
            for (const auto &Event : MBCaen.builders[Cassette].Events) {
              
              if (MBCaen.amorgeom.is1DDetector(Cassette)) {
                Counters.Events1DDiscard++;
                continue;
              }

              // 2D events must have coincidences for both planes, but not 1D
              // This is only relevant for alignment mode and for 2D detectors
              if (not Event.both_planes()) {
                XTRACE(EVENT, INF, "No coincidence\n %s", Event.to_string({}, true).c_str());
                Counters.EventsNoCoincidence++;
                continue;
              }
              
              // // Discard if there are gaps in the strip channels
              // if (Event.ClusterA.hits.size() < Event.ClusterA.coord_span()) {
              //   int StripGap = Event.ClusterA.coord_span() - Event.ClusterA.hits.size();
              //   if (StripGap >= MBCaen.config.MaxGapStrip) {
              //     Counters.EventsInvalidStripGap++;
              //     continue;
              //   }
              // }
              //

              // Discard if there are gaps in the wire channels
              if (MBCaen.config.CheckWireGap) {
                if (Event.ClusterB.hits.size() < Event.ClusterB.coord_span()) {
                  Counters.EventsWireGapCount++;
                  int WireGap = Event.ClusterB.coord_span() - Event.ClusterB.hits.size();
                  if (WireGap >= MBCaen.config.MaxGapWire) {
                    Counters.EventsWireGapInvalid++;
                    continue;
                  }
                }
              }

              if (Event.ClusterB.hits.size() > 1) {
                Counters.EventsWireMultTwoPlus++;
              }

              if (Event.ClusterB.hits.size() == 1) {
                Counters.EventsWireMultSingle++;
              }

              XTRACE(EVENT, DEB, "Event Valid\n %s", Event.to_string({}, true).c_str());

              uint16_t x{0};
              uint16_t y{0};

              if (MBCaen.ModuleSettings.Alignment) {
                x = static_cast<uint16_t>(std::round(Event.ClusterA.coord_center()));
              } else {
                x = 0;
              }
              y = static_cast<uint16_t>(std::round(Event.ClusterB.coord_center()));

              // Calculate event (t, pix)
              uint64_t time = Event.time_start();
              if (time > MBCaen.config.MaxTofNS) {
                Counters.EventsMaxTofNS++;
              }
              auto pixel_id = MBCaen.essgeom.pixel2D(x, y);
              XTRACE(EVENT, DEB, "time: %u, x %u, y %u, pixel %u", time, x, y, pixel_id);
            
              if (pixel_id == 0) {
                XTRACE(EVENT, DEB, "pixel error: time: %u, x %u, y %u, pixel %u", time, x, y, pixel_id);
                Counters.GeometryErrors++;
              } else {
                Counters.TxBytes += events.addEvent(time, pixel_id);
                Counters.Events++;
                Counters.Events2D++;
              }
            }
            MBCaen.builders[Cassette].Events.clear(); // else events will accumulate
          }
        } // interate over builders
        else { //when alignment mode isn't used, process 1D events directly per wire readout
          XTRACE(EVENT, DEB, "processing %u 1D events", MBCaen.Hits1D.size());
          // /// Attempt to use all 1D hits as events
          for (Hit & h1d : MBCaen.Hits1D) {
            uint16_t x{0};
            uint16_t y = h1d.coordinate;
            auto pixel_id = MBCaen.essgeom.pixel2D(x, y);
            uint64_t time = h1d.time;
            if (pixel_id == 0) {
                XTRACE(EVENT, DEB, "pixel error: time: %u, x %u, y %u, pixel %u", time, x, y, pixel_id);
                Counters.GeometryErrors++;
              } else {
                XTRACE(EVENT, DEB, "valid event: time: %u, x %u, y %u, pixel %u", time, x, y, pixel_id);
                Counters.TxBytes += events.addEvent(time, pixel_id);
                Counters.Events++;
                Counters.Events1D++;
              }
          }
          MBCaen.Hits1D.clear();
        }
      } else {
        // There is NO data in the FIFO - do stop checks and sleep a little
        Counters.ProcessingIdle++;
        usleep(10);
      }



      // if filedumping and requesting time splitting, check for rotation.
      if (MBCAENSettings.H5SplitTime != 0 and (MBCaen.dumpfile)) {
        if (h5flushtimer.timeus() >= MBCAENSettings.H5SplitTime * 1000000) {

          /// \todo user should not need to call flush() - implicit in rotate() ?
          MBCaen.dumpfile->flush();
          MBCaen.dumpfile->rotate();
          h5flushtimer.reset();
        }
      }

      if (produce_timer.timeout()) {

        RuntimeStatusMask =  RtStat.getRuntimeStatusMask({Counters.RxPackets, Counters.Events, Counters.TxBytes});

        Counters.TxBytes += events.produce();
        Counters.MonitorTxBytes += monitor.produce();

        if (!MBCaen.histograms.isEmpty()) {
          // XTRACE(PROCESS, INF, "Sending histogram for %zu readouts",
          //   histograms.hit_count());
          MBCaen.histfb.produce(MBCaen.histograms);
          MBCaen.histograms.clear();
        }

        /// Kafka stats update - common to all detectors
        /// don't increment as producer keeps absolute count
        Counters.kafka_produce_fails = eventprod.stats.produce_fails;
        Counters.kafka_ev_errors = eventprod.stats.ev_errors;
        Counters.kafka_ev_others = eventprod.stats.ev_others;
        Counters.kafka_dr_errors = eventprod.stats.dr_errors;
        Counters.kafka_dr_noerrors = eventprod.stats.dr_noerrors;
      }

      if (not runThreads) {
        // \todo flush everything here
        XTRACE(INPUT, ALW, "Stopping processing thread.");
        return;
      }
    }
  }
}



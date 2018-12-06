///
/// \file Tags.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TAG_H
#define ALICEO2_MONITORING_TAG_H

#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

namespace tags
{
  using namespace std::string_view_literals; 

  enum Detector { ACO = 0 , AD, CPV, EMC, FMD, HMP, MTR, PHS, PMD, ITS, T0, TOF, TPC, TRD, V0};
  enum Subsystem { QualityControl = 15, Readout, DPL, CRU };

  static constexpr std::string_view tagMap[] = {
    "detector=ACO"sv, // 0
    "detector=AD"sv,  // 1
    "detector=CPV"sv, // 2
    "detector=EMC"sv, // 3
    "detector=FMD"sv, // 4
    "detector=HMP"sv, // 5
    "detector=MTR"sv, // 6
    "detector=PHS"sv, // 7
    "detector=PMD"sv, // 8
    "detector=ITS"sv, // 9
    "detector=T0"sv,  // 10
    "detector=TOF"sv, // 11
    "detector=TPC"sv, // 12
    "detector=TRD"sv, // 13
    "detector=V0"sv,  // 14

    "sybsystem=QC"sv, // 15
    "sybsystem=Readout"sv, // 16
    "sybsystem=DPL"sv, // 17
    "sybsystem=CRU"sv, // 18

  };
}

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TAG_H

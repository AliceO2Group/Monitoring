// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file Tags.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_TAG_H
#define ALICEO2_MONITORING_TAG_H

#include <string>
#include <string_view>
#include <array>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

namespace tags
{
using namespace std::string_view_literals;

// Tag keys
enum class Key : unsigned short int {
  Hostname,
  Rolename,
  Name,
  Detector,
  Subsystem,
  CRU,
  FLP,
  EPN,
  Unit,
  ID,
  Type,
  CRORC,
  SerialId,
  Endpoint
};

/// Tag keys array
static constexpr std::array<std::string_view, 14> TAG_KEY = {
  "hostname"sv,
  "rolenane"sv,
  "name"sv,
  "detector"sv,
  "subsystem"sv,
  "CRU"sv,
  "FLP"sv,
  "EPN"sv,
  "unit"sv,
  "id"sv,
  "type"sv,
  "CRORC"sv,
  "serialId"sv,
  "endpoint"sv
};

// Tag values
enum class Value : unsigned short int {
  Null,
  AD,            // 1
  CPV,           // 2
  EMC,           // 3
  FMD,           // 4
  HMP,           // 5
  MCH,           // 6
  MTR,           // 7
  PHS,           // 8
  PMD,           // 9
  ITS,           // 10
  T0,            // 11
  TOF,           // 12
  TPC,           // 13
  TRD,           // 14
  V0,            // 15
  QC,            // 16
  Readout,       // 17
  DPL,           // 18
  CRU,           // 19
  Bytes,         // 20
  Megabytes,     // 21
  Gigabytes,     // 22
  Bits,          // 23
  Megabits,      // 24
  Gigabits,      // 25
  bps,           // 26
  Mbps,          // 27
  Gbps,          // 28
  Nanoseconds,   // 29
  Microseconds,  // 30
  Milliseconds,  // 31
  Seconds,       // 32
  PerSecond,     // 33
  PerCycle,      // 34
  PerRun,        // 35
  Monitoring,    // 36
  CRORC,         // 37
  ACO,           // 38
  Analysis,      // 39
  DataDistribution, // 40
  StfBuilder,    // 41
  StfSender,     // 42
  TfBuilder,     // 43
  TfScheduler,   // 44
  DataSampling,  // 45
  Mergers        // 46
};

// Tag value array
static constexpr std::array<std::string_view, 47> TAG_VALUE = {{
  "Null"sv,
  "AD"sv,      // 1
  "CPV"sv,     // 2
  "EMC"sv,     // 3
  "FMD"sv,     // 4
  "HMP"sv,     // 5
  "MCH"sv,     // 6
  "MTR"sv,     // 7
  "PHS"sv,     // 8
  "PHS"sv,     // 9
  "PMD"sv,     // 10
  "ITS"sv,     // 11
  "TOF"sv,     // 12
  "TPC"sv,     // 13
  "TRD"sv,     // 14
  "V0"sv,      // 15
  "QC"sv,      // 16
  "Readout"sv, // 17
  "DPL"sv,     // 18
  "CRU"sv,     // 19
  "B"sv,       // 20
  "MB"sv,      // 21
  "GB"sv,      // 22
  "b"sv,       // 23
  "Mb"sv,      // 24
  "Gb"sv,      // 25
  "bps"sv,     // 26
  "Mbps"sv,    // 27
  "Gbps"sv,    // 28
  "ns"sv,      // 29
  "us"sv,      // 30
  "ms"sv,      // 31
  "s"sv,       // 32
  "ps"sv,      // 33
  "pcycle"sv,  // 34
  "prun"sv,    // 35
  "monitoring"sv, // 36
  "CRORC"sv,   // 37
  "ACO"sv,     // 38
  "Analysis"sv,// 39
  "DD"sv,      // 40
  "StfB"sv,    // 41
  "StfS"sv,    // 42
  "TfB"sv,     // 43
  "TfS"sv,     // 44
  "DS"sv,      // 45
  "Mergers"sv  // 46
}};

static constexpr std::string_view GetValue(const int value)
{
  return TAG_VALUE[value];
}

} // namespace tags
} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_TAG_H

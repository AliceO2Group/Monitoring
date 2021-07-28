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
/// \file MonitoringException.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H
#define ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H

#include <exception>
#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Internal monitoring exception
class MonitoringException : public std::exception
{
 public:
  MonitoringException(const std::string& source, const std::string& message);
  ~MonitoringException() = default;
  const char* what() const throw();

 private:
  std::string message;
};

} // namespace monitoring
} // namespace o2

#endif //ALICEO2_MONITORING_EXCEPTIONS_INTERNAL_MONITORING_H

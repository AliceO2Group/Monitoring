///
/// \file Tag.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_TAG_H
#define ALICEO2_MONITORING_CORE_TAG_H

#include <string>

namespace AliceO2
{
/// ALICE O2 Monitoring system
namespace Monitoring
{

/// \brief Represent key-value metric tag
class Tag
{
  public:
    std::string name;
    std::string value;
};

} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_TAG_H

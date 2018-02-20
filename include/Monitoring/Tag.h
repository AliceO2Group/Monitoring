///
/// \file Tag.h
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#ifndef ALICEO2_MONITORING_CORE_TAG_H
#define ALICEO2_MONITORING_CORE_TAG_H

#include <string>

namespace o2
{
/// ALICE O2 Monitoring system
namespace monitoring
{

/// \brief Represent key-value metric tag
class Tag
{
  public:
    std::string name;
    std::string value;
};

} // namespace monitoring
} // namespace o2

#endif // ALICEO2_MONITORING_CORE_TAG_H

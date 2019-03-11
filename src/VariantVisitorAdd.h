// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <variant>

namespace o2
{
// ALICE O2 Monitoring system
namespace monitoring
{
/// \brief Adds boost variants
class VariantVisitorAdd
{
public:
  /// Overloads operator() that sums numeric values
  template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
  >
  std::variant< int, std::string, double, uint64_t > operator()(const T& a, const T& b) const {
    return a + b;
  }

  /// If arguments have different type an exception is raised
  /// \throws MonitoringException
  template<typename T, typename U>
  std::variant< int, std::string, double, uint64_t > operator()(const T&, const U&) const {
    throw MonitoringException("DerivedMetrics/Visitor", "Cannot operate on different or non-numeric types");
  }
};

} // namespace monitoring
} // namespace o2

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

namespace o2
{
// ALICE O2 Monitoring system
namespace monitoring
{
/// \brief Subtracts boost variants in order to calculate rate
class VariantVisitorRate
{
 private:
  /// Timestamp difference in milliseconds
  int timestampCount;

 public:
  /// Creates variant visitor functor
  /// \param count  timestamp difference in milliseconds
  VariantVisitorRate(int count) : timestampCount(count)
  {
  }

  /// Calculates rate only when two arguments of the same type are passed
  /// \return calculated rate in Hz
  template <
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  double operator()(const T& a, const T& b) const
  {
    return (1000 * (static_cast<double>(a) - b)) / timestampCount;
  }

  /// If arguments have different type an exception is raised
  /// \throws MonitoringException
  template <typename T, typename U>
  double operator()(const T&, const U&) const
  {
    throw MonitoringException("DerivedMetrics/Visitor", "Cannot operate on different or non-numeric types");
  }
};

} // namespace monitoring
} // namespace o2

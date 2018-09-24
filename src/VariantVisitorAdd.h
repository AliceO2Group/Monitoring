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

namespace o2
{
// ALICE O2 Monitoring system
namespace monitoring
{
/// \brief Adds boost variants
class VariantVisitorAdd : public boost::static_visitor<boost::variant<int, std::string, double, uint64_t>>
{
public:

  /// Overloads operator() that sums numeric values
  template<
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
  >
  T operator()(const T& a, const T& b) const {
    return a + b;
  }

  /// If arguments have different type an exception is raised
  /// \throws MonitoringInternalException
  template<typename T, typename U>
  double operator()(const T&, const U&) const {
    throw MonitoringInternalException("DerivedMetrics/Visitor", "Cannot operate on different or non-numeric types");
  }
};

} // namespace monitoring
} // namespace o2

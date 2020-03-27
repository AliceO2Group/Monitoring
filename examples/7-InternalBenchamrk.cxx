///
/// \file 7-InternalBenchmark.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"
#include <iomanip>

using namespace o2::monitoring;
using namespace std::chrono;

void test(std::unique_ptr<Monitoring>& monitoring)
{
  for (int i = 0; i < 100000; i++) {
    monitoring->send(Metric{"myMetricInt"}.addValue("value", 10));
    monitoring->send(Metric{"myMetricFloat"}.addValue("value", 10.10));
  }
}

void testWithTags(std::unique_ptr<Monitoring>& monitoring)
{
  monitoring->addGlobalTag("name", "benchmark");
  for (int i = 0; i < 100000; i++) {
    monitoring->send(Metric{"myMetricInt"}.addValue("value", 10).addTag(tags::Key::Detector, tags::Value::TPC));
    monitoring->send(Metric{"myMetricFloat"}.addValue("value", 10.10).addTag(tags::Key::Subsystem, tags::Value::QC));
  }
}

int main()
{
  static constexpr std::array<std::string_view, 3> backends = {
    "no-op://",
    "influxdb-udp://localhost:1234",
    "influxdb-stdout://"};
  std::cout << "| " << std::setw(30) << "Backend"
            << " |"
            << std::setw(10) << "no tags"
            << "   |"
            << std::setw(10) << " (2+1) tags"
            << "  |" << std::endl;
  std::cout << "+--------------------------------+-------------+-------------+" << std::endl;
  for (auto& backend : backends) {
    std::cout.setstate(std::ios_base::failbit);
    auto monitoring = MonitoringFactory::Get(std::string(backend));

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    test(monitoring);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    high_resolution_clock::time_point t3 = high_resolution_clock::now();
    testWithTags(monitoring);
    high_resolution_clock::time_point t4 = high_resolution_clock::now();

    std::cout.clear();
    std::cout << "| " << std::setw(30) << backend << " |";
    std::cout << std::setw(10) << duration_cast<milliseconds>(t2 - t1).count() << "us |";
    std::cout << std::setw(10) << duration_cast<milliseconds>(t4 - t3).count() << "us |" << std::endl;
  }
}

///
/// \file 2-TaggedMetrics.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Monitoring/MonitoringFactory.h"

using namespace o2::monitoring;

int main() {

  // Configure monitoring
  // Pass string with list of URLs as parameter
  auto monitoring = MonitoringFactory::Get("stdout://");

  /// Add global tags
  monitoring->addGlobalTag("name", "test");
  monitoring->addGlobalTag(tags::Key::Subsystem, tags::Value::DPL);

  // now send an application specific metric with additional tags
  // 10 is the value
  // myMetric is the name of the metric
  // then add predefined tag
  monitoring->send(Metric{10, "myMetric"}.addTag(tags::Key::Detector, tags::Value::TPC));
  monitoring->send(Metric{10, "myMetric"}.addTag(tags::Key::CRU, 123));
}

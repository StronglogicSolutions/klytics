#ifndef __KLYTICS_HPP__
#define __KLYTICS_HPP__

#include <iostream>
#include <ctime>
#include <string>
#include <process.hpp>
#include <nlohmann/json.hpp>
#include <tabulate/table.hpp>

namespace klytics {
namespace constants {
const std::string FOLLOWER_APP{"../external/followers/followers.js"};
} // namespace constants

ProcessResult execute(std::string program);

struct FollowerCount {
std::string name;
std::string platform;
std::string value;
};

class ResultInterface {
public:
virtual std::string to_string() = 0;
};

/**
 * FormatTable
 *
 * @helper function
 */
void FormatTable(tabulate::Table& table);

class JSONResult : public ResultInterface {
using counts = std::vector<FollowerCount>;
public:
virtual std::string to_string() override {
  using namespace tabulate;

  Table table{};
  table.add_row({"Platform", "Name", "Count"});

  for (const auto& result : m_counts) {
    table.add_row({result.platform, result.name, result.value});
  }

  FormatTable(table);

  return table.str();
}

/**
 * read
 */
bool read(std::string s) {
  using json = nlohmann::json;

  json r_json = json::parse(s);

  if (!r_json.is_null() && r_json.is_object()) {
    auto instagram = r_json["instagram"];
    auto youtube   = r_json["youtube"];

    if (!instagram.is_null() && instagram.is_object()) {
      for (const auto& it : instagram.items()) {
        m_counts.push_back(
          FollowerCount{
            .name     = it.key(),
            .platform = "instagram",
            .value    = it.value()["value"].dump()
          }
        );
      }
    }

    if (!youtube.is_null() && youtube.is_object()) {
      for (const auto& it : youtube.items()) {
      m_counts.push_back(
        FollowerCount{
          .name     = it.key(),
          .platform = "youtube",
          .value    = it.value()["value"].dump()
        }
      );
      }
    }
    return true;
  }
  return false;
}

private:
counts m_counts;
};

/**
 *
 * KLytics - Main class
 *
 */
class KLytics {
public:

/**
 * get_follower_count
 */
std::string get_follower_count() {
  ProcessResult result = execute(constants::FOLLOWER_APP);

  if (result.error) {
    return "Error executing followers app";
  }

  JSONResult json{};

  if (json.read(result.output)) {
    return json.to_string();
  }

  return "Error processing followers app data";
}
};

} // namespace klytics

#endif // __KLYTICS_HPP__

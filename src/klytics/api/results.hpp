#ifndef __RESULTS_HPP__
#define __RESULTS_HPP__

#include <nlohmann/json.hpp>
#include <process.hpp>

#include "klytics/api/analysis/html.hpp"
#include "klytics/common/constants.hpp"
#include "klytics/common/types.hpp"

class ResultInterface {
public:
virtual std::string to_string() = 0;
};

class TrendsJSONResult : public ResultInterface {
public:
TrendsJSONResult(std::string data)
: m_data(data) {}

virtual std::string to_string() override {
  return m_data;
}

std::vector<GoogleTrend> get_result() {
  using json = nlohmann::json;
  json parsed = json::parse(m_data);

  std::vector<GoogleTrend> result{};

  if (!parsed.is_null() && parsed.is_object()) {
    auto items = parsed["results"];

    if (!items.is_null() && items.is_array()) {
      for (const auto &item : items) {
        result.emplace_back(
          GoogleTrend{
            .term = SanitizeJSON(item["term"].dump()),
            .value = std::stoi(item["lastScore"].dump())
          }
        );
      }
    }
  }

  return result;
}

private:
std::string m_data;
};

class FollowersJSONResult : public ResultInterface {
using counts = std::vector<FollowerCount>;
public:
virtual std::string to_string() override {
  return counts_to_html(m_counts);
}

/**
 * read
 */
bool read(std::string s) {
  using json = nlohmann::json;
  using namespace constants;

  json r_json = json::parse(s);
  json f_json = json::parse(ReadFromFile(get_executable_cwd() + FOLLOWER_JSON), nullptr, false);
  json s_json{};

  bool file_valid = (!f_json.is_null() && f_json.is_object());

  if (!r_json.is_null() && r_json.is_object()) {
    json instagram   = r_json["instagram"];
    json youtube     = r_json["youtube"];
    json f_instagram = (file_valid && f_json.contains("instagram")) ? f_json["instagram"] : json{};
    json f_youtube   = (file_valid && f_json.contains("youtube"))   ? f_json["youtube"]   : json{};

    bool found_ig_file = (!f_instagram.is_null() && f_instagram.is_object());
    bool found_yt_file = (!f_youtube.is_null()   && f_youtube.is_object());

    std::string current_time = get_simple_datetime();

    if (!instagram.is_null() && instagram.is_object()) {
      for (const auto& it : instagram.items()) {
        std::string name           = it.key();
        std::string value          = it.value()["value"].dump();
        std::string previous_date {};
        uint32_t    previous_count{};

        s_json["instagram"][name]["value"] = value;
        s_json["instagram"][name]["date"]  = current_time;

        if (f_instagram.contains(name) && !f_instagram[name].is_null()) {
          previous_date  = f_instagram[name]["date"];
          previous_count = std::stoi(SanitizeJSON(f_instagram[name]["value"].dump()));
        }

        m_counts.push_back(
          FollowerCount{
            .name     = it.key(),
            .platform = "instagram",
            .value    = value,
            .time     = current_time,
            .delta_t  = datetime_delta_string(current_time, (previous_date.empty()) ? current_time : previous_date),
            .delta_v  = std::to_string(std::stoi(value) - previous_count),
          }
        );
      }
    }

    if (!youtube.is_null() && youtube.is_object()) {
      for (const auto& it : youtube.items()) {
        std::string name           = it.key();
        std::string value          = SanitizeJSON(it.value()["value"].dump());
        std::string previous_date {};
        uint32_t    previous_count{};

        s_json["youtube"][name]["value"] = value;
        s_json["youtube"][name]["date"]  = current_time;

        if (f_youtube.contains(name) && !f_youtube[name].is_null()) {
          previous_date  = f_youtube[name]["date"];
          previous_count = std::stoi(SanitizeJSON(f_youtube[name]["value"].dump()));
        }

        m_counts.push_back(
          FollowerCount{
            .name     = name,
            .platform = "youtube",
            .value    = value,
            .time     = current_time,
            .delta_t  = datetime_delta_string(current_time, (previous_date.empty()) ? current_time : previous_date),
            .delta_v  = std::to_string(std::stoi(value) - previous_count),
          }
        );
      }
    }

    SaveToFile(s_json.dump(), get_executable_cwd() + FOLLOWER_JSON);

    return true;
  }
  return false;
}

private:

void compute_delta() {
using json = nlohmann::json;
using namespace constants;

json follower_json = json::parse(ReadFromFile(FOLLOWER_JSON));
if (!follower_json.is_null()) {

}
}
counts m_counts;
};

#endif // __RESULTS_HPP__
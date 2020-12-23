#ifndef __KLYTICS_HPP__
#define __KLYTICS_HPP__

#include <iostream>
#include <ctime>
#include <cpr/cpr.h>
#include <string>

#include <nlohmann/json.hpp>

#include "process.hpp"
#include "api/api.hpp"

namespace klytics {
namespace constants {
const std::string FOLLOWER_APP{"../external/followers/followers.js"};
const std::string FOLLOWER_JSON{"../config/followers.json"};
} // namespace constants

ProcessResult execute(std::string program);

struct FollowerCount {
std::string name;
std::string platform;
std::string value;
std::string time;
std::string delta_t;
std::string delta_v;
};

/**
 * counts_to_html
 *
 * @param
 * @returns
 */
inline std::string counts_to_html(std::vector<FollowerCount> counts) {
  HTML::Document document{"KIQ Analytics"};

  document.addAttribute("lang", "en");
  document.head() << HTML::Meta("utf-8")
                  << HTML::Meta("viewport", "width=device-width, initial-scale=1, shrink-to-fit=no");
  document.head() << HTML::Style(".navbar{margin-bottom:20px;}");
  document.body().cls("follower-counts");

  HTML::Div main{"container"};
  main << HTML::Header1("KIQ Analytics");
  main << HTML::Header2("Follower Counts") << HTML::Break() << HTML::Break();

  HTML::Table table{};
  table.cls("table");
  table << HTML::Caption{"Results"};
  table << (HTML::Row() <<
    HTML::ColHeader("Name")  << HTML::ColHeader("Platform") <<
    HTML::ColHeader("Count") << HTML::ColHeader("CountΔ")   << HTML::ColHeader("TimeΔ")
  );

  for (const auto& count : counts)
    table << (HTML::Row() << HTML::Col(count.name) << HTML::Col(count.platform) << HTML::Col(count.value) << HTML::Col(count.delta_v) << HTML::Col(count.delta_t));

  main     << std::move(table);
  document << std::move(main);

  return SanitizeOutput(document.toString());
}

class ResultInterface {
public:
virtual std::string to_string() = 0;
};

class JSONResult : public ResultInterface {
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

/**
 *
 * KLytics - Main class
 *
 */
class KLytics {
public:


/**
 * fetch_follower_count
 * @returns [out] {std::string}
 */
std::string fetch_follower_count() {
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


/**
 * generate_report
 *
 * @returns [out] {std::string}
 */
std::string generate_report() {
  std::string::size_type extra_text_size{102};

  std::string            output, video_stats_output, competitor_stats_output;
  std::string            follower_count = fetch_follower_count();
  std::vector<VideoInfo> videos         = m_api.fetch_youtube_stats();

  if (!videos.empty()) {
    video_stats_output      = videos_to_html(videos);
    competitor_stats_output = videos_to_html(m_api.find_similar_videos(videos.front()));
  }

  output.reserve(
    follower_count.size() + video_stats_output.size() + competitor_stats_output.size() + extra_text_size
  );

  output += "FOLLOWER COUNT\n\n";
  output += follower_count;
  output += "\n\n";
  output += "LATEST VIDEOS\n\n";
  output += video_stats_output;
  output += "\n\n";
  output += "COMPETITOR VIDEOS (based on your most recent video)\n\n";
  output += competitor_stats_output;
  output += "\n\n";
  output += "Thank you!!!";

  return output;
}

/**
 * generate_video_stats_table
 *
 * @returns [out] {std::string}
 */
std::string generate_video_stats_table() {
  return videos_to_html(
    m_api.fetch_youtube_stats()
  );
}

/**
 * fetch_videos
 */
std::vector<VideoInfo> fetch_videos() {
  return m_api.fetch_channel_videos();
}
/**
 * get_youtube_videos
 *
 * @returns [out] {std::vector<VideoInfo}
 */
std::vector<VideoInfo> get_youtube_videos() {
  return m_api.get_videos();
}

private:
API m_api;
};

} // namespace klytics

#endif // __KLYTICS_HPP__

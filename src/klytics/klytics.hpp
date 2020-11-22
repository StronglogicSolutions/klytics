#ifndef __KLYTICS_HPP__
#define __KLYTICS_HPP__

#include <iostream>
#include <ctime>
#include <cpr/cpr.h>
#include <string>
#include <process.hpp>
#include <nlohmann/json.hpp>
#include <tabulate/table.hpp>

#include "api/api.hpp"

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

  FormatTable(table, 3);
  table.column(2).format().font_align(FontAlign::right);

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
  using namespace tabulate;

  std::string::size_type extra_text_size{102};

  std::string            output, video_stats_output, competitor_stats_output;
  std::string            follower_count = fetch_follower_count();
  std::vector<VideoInfo> videos         = m_api.fetch_youtube_stats();

  if (!videos.empty()) {
    video_stats_output      = table_to_formatted_string(videos_to_table(videos));
    competitor_stats_output = table_to_formatted_string(videos_to_table(m_api.find_similar_videos(videos.front())));
  }

  output.resize(follower_count.size() + video_stats_output.size() + competitor_stats_output.size() + extra_text_size);

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
  return table_to_formatted_string(
    videos_to_table(
      m_api.fetch_youtube_stats()
    )
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

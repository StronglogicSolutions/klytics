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
ProcessResult execute(std::string program);

/**
  @class

  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ KLytics ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class KLytics {
public:
std::string            fetch_follower_count();
std::string            generate_report();
std::string            generate_video_stats_table();
std::vector<VideoInfo> fetch_videos();
std::vector<VideoInfo> get_youtube_videos();

private:
API               m_api;
ContentComparator m_comparator;
};

} // namespace klytics

#endif // __KLYTICS_HPP__

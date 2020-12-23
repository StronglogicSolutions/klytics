#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include "klytics/common/types.hpp"

class ContentComparator {

bool add_content(std::string key, std::vector<VideoInfo> videos) {
  if (m_map.find(key) != m_map.end()) {
    return false;
  }

  m_map[key] = videos;

  return true;
}

private:
VideoMap m_map;
};

class VideoAnalyzer {
using Videos = std::vector<VideoInfo>;

public:
VideoAnalyzer(Videos videos)
: m_videos(videos) {}

bool analyze() {
  if (!m_videos.empty()) {
    return true;
  }

  return false;
}

VideoInfo most_likes() {
  VideoInfo p_video{};
  int       most_likes{};

  for (const auto& video : m_videos) {
    int likes = std::stoi(video.stats.likes);
    if (likes > most_likes) {
      most_likes = likes;
      p_video = video;
    }
  }
  return p_video;
}

VideoInfo most_controversial() {
  VideoInfo p_video{};
  float     most_controversial{};

  for (const auto& video : m_videos) {
    float controversy_score = std::stoi(video.stats.likes);
    if (controversy_score > most_controversial) {
      most_controversial = controversy_score;
      p_video = video;
    }
  }
  return p_video;
}

private:

double compute_view_score(VideoInfo v) {
  int     views   = std::stoi(v.stats.views);
  int64_t delta_t = std::chrono::duration_cast<std::chrono::seconds>(
    get_datetime_delta(
      get_simple_datetime(), v.datetime)
  ).count();

  return static_cast<double>(views / delta_t);
}

Videos m_videos;
};

#endif // __TOOLS_HPP__

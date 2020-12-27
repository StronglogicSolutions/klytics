#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include "klytics/common/types.hpp"

enum Platform {
  INSTAGRAM = 0x00,
  UNKNOWN   = 0x01
};

class PlatformSpecificComparator {
public:
  virtual ~PlatformSpecificComparator() {}
  virtual Platform get_type() = 0;
};


class VideoStudy {
public:
using Videos = std::vector<VideoInfo>;

VideoStudy(Videos videos)
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

Videos get_videos() { return m_videos; }

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


class ContentComparator : public PlatformSpecificComparator {
using StudyMap = std::unordered_map<std::string, VideoStudy>;
using Videos = VideoStudy::Videos;

public:
/**
 * get_type
 *
 * @override
 * @returns [out] {Platform}
 */
virtual Platform get_type() override {
  return Platform::INSTAGRAM;
}

/**
 * add_content
 *
 * @param [in]
 * @param [in]
 * @returns [out] {bool}
 */
bool add_content(std::string key, Videos videos) {
  if (m_map.find(key) != m_map.end()) {
    return false;
  }

  VideoStudy study{videos};
  study.analyze();

  m_map.insert({key, study});

  return true;
}

private:
StudyMap m_map;
};


#endif // __TOOLS_HPP__

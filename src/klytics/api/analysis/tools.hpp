#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include "klytics/common/types.hpp"


/**
 * Platform
 * @enum
 */
enum Platform {
  INSTAGRAM = 0x00,
  UNKNOWN   = 0x01
};

/**
 * @interface
 */
class PlatformSpecificComparator {
public:
  virtual ~PlatformSpecificComparator() {}
  virtual Platform get_type() = 0;
};

class VideoStudy {
public:
using Videos = std::vector<VideoInfo>;

struct VideoStudyResult {
Videos::const_iterator most_likes;
Videos::const_iterator most_dislikes;
Videos::const_iterator most_comments;
Videos::const_iterator top_view_score;
};

/**
 * VideoStudy
 *
 * @constructor
 * @returns [out] {VideoStudy}
 */
VideoStudy(Videos videos)
: m_videos(videos) {}

/**
 * analyze
 *
 * @returns [out] {VideoStudyResult}
 */
const VideoStudyResult analyze() {
  VideoStudyResult result{};

  if (!m_videos.empty()) {
    result.most_likes     = most_liked();
    result.most_dislikes  = most_controversial();
    result.most_comments  = m_videos.end();
    result.top_view_score = top_view_score();
  }

  return result;
}

/**
 * most_liked
 *
 * @returns [out] {Videos::const_iterator} The video with the most likes
 */
const Videos::const_iterator most_liked() const {
  return std::max_element(
    m_videos.begin(),
    m_videos.end(),
    [](const VideoInfo& a, const VideoInfo& b) {
      return std::stoi(a.stats.likes) > std::stoi(b.stats.likes);
    }
  );
}


/**
 * most_controversial
 *
 * @returns [out] {Videos::const_iterator} The video with the most dislikes
 */
const Videos::const_iterator most_controversial() const {
  return std::max_element(
    m_videos.begin(),
    m_videos.end(),
    [](const VideoInfo& a, const VideoInfo& b) {
      return std::stoi(a.stats.dislikes) > std::stoi(b.stats.dislikes);
    }
  );
}

/**
 * top_view_score
 *
 * @returns [out] {Videos::const_iterator} The video with the most dislikes
 */
const Videos::const_iterator top_view_score() const {
  return std::max_element(
    m_videos.begin(),
    m_videos.end(),
    [](const VideoInfo& a, const VideoInfo& b) {
      return a.stats.view_score > b.stats.view_score;
    }
  );
}

/**
 * get_videos
 *
 * @returns [out] {std::vector<VideoInfo>}
 */
Videos get_videos() { return m_videos; }

private:

/**
 * compute_view_score
 *
 * @param   [in]  {VideoInfo}
 * @returns [out] {double}
 */
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

using StudyMap = std::unordered_map<std::string, VideoStudy>;

struct VideoCreatorComparison {
public:
using ResultMap = std::unordered_map<std::string, VideoStudy::VideoStudyResult>;

VideoCreatorComparison(StudyMap study_map)
: map(study_map) {
  result.reserve(map.size());
}

void      analyze() {
  for (auto&& [key, value] : map) {
    result[key] = value.analyze();
  }
}

ResultMap get_result() { return result; }

private:
ResultMap result;
StudyMap  map;
};


class ContentComparator : public PlatformSpecificComparator {
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

  m_map.insert({key, study});

  return true;
}

const VideoCreatorComparison analyze() const {
  VideoCreatorComparison comparison{m_map};

  comparison.analyze();

  return comparison;
}

private:
StudyMap m_map;
};


#endif // __TOOLS_HPP__

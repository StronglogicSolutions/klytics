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

  std::for_each(m_videos.begin(), m_videos.end(),
    [this](VideoInfo& video) {
      video.stats.view_score = compute_view_score(video);
    }
  );

  if (!m_videos.empty()) {
    std::cout << __PRETTY_FUNCTION__ << ": Implement most comments max counter" << std::endl;
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
      return std::stoi(a.stats.likes) < std::stoi(b.stats.likes);
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
      return std::stoi(a.stats.dislikes) < std::stoi(b.stats.dislikes);
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
      return a.stats.view_score < b.stats.view_score;
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

  int64_t delta_t = std::chrono::duration_cast<std::chrono::minutes>(
    get_datetime_delta(
      get_simple_datetime(), v.datetime)
  ).count();

  return static_cast<double>(views * 1000 / delta_t);
}

Videos m_videos;
};

using StudyMap  = std::unordered_map<std::string, VideoStudy>;
using ResultMap  = std::unordered_map<std::string, VideoStudy::VideoStudyResult>;
using ResultPair = std::pair<std::string, VideoStudy::VideoStudyResult>;

/**
 * VideoAnalyst
 *
 * @class
 */
class VideoAnalyst {
public:
/**
 * VideoAnalysis
 *
 * @struct
 */
struct VideoAnalysis {
ResultMap get_result_map() {
  return map;
}

std::string most_likes_key;
std::string most_dislikes_key;
std::string most_comments_key;
std::string best_viewscore_key;

std::string most_likes_channel_name() {
  return most_likes_key;
}

std::string most_dislikes_channel_name() {
  return most_dislikes_key;
}

std::string most_comments_channel_name() {
  return most_comments_key;
}

std::string best_viewscore_channel_name() {
  return best_viewscore_key;
}

ResultMap map;
};

/**
 * get_analysis
 *
 * @returns [out] {VideoAnalysis}
 */
VideoAnalysis get_analysis() {
  return m_analysis;
}

/**
 * analyze
 */
void analyze(StudyMap map) {
  m_map = map;
  for (auto&& [key, value] : m_map) {
    m_analysis.map[key] = value.analyze();
  }

  find_maximums();
}

private:
VideoAnalysis m_analysis;
StudyMap      m_map;

void find_maximums() {
  using StudyResult = VideoStudy::VideoStudyResult;

  auto most_likes_index = std::max_element(
    m_analysis.map.begin(),
    m_analysis.map.end(),
    [](const ResultPair& a, const ResultPair& b) {
      return std::stoi(a.second.most_likes->stats.likes) < std::stoi(b.second.most_likes->stats.likes);
    }
  );

  if (most_likes_index != m_analysis.map.end()) {
    m_analysis.most_likes_key = most_likes_index->first;
  }

  auto most_dislikes_index = std::max_element(
    m_analysis.map.begin(),
    m_analysis.map.end(),
    [](const ResultPair& a, const ResultPair& b) {
      return std::stoi(a.second.most_dislikes->stats.dislikes) < std::stoi(b.second.most_dislikes->stats.dislikes);
    }
  );

  if (most_dislikes_index != m_analysis.map.end()) {
    m_analysis.most_dislikes_key = most_dislikes_index->first;
  }

  auto best_viewscore_index = std::max_element(
    m_analysis.map.begin(),
    m_analysis.map.end(),
    [](const ResultPair& a, const ResultPair& b) {
      return a.second.top_view_score->stats.view_score < b.second.top_view_score->stats.view_score;
    }
  );

  if (best_viewscore_index != m_analysis.map.end()) {
    m_analysis.best_viewscore_key = best_viewscore_index->first;
  }
}

};

struct VideoCreatorComparison {
public:
using VideoAnalysis = VideoAnalyst::VideoAnalysis;

/**
 * VideoCreatorComparison
 *
 * @constructor
 */
VideoCreatorComparison(StudyMap study_map)
: map(study_map) {}

/**
 * analyze
 */
void      analyze() {
  analyst.analyze(map);
}

/**
 * get_result
 *
 * @returns [out] {VideoAnalysisResult}
 */
VideoAnalysis get_result() { return analyst.get_analysis(); }

private:
VideoAnalyst analyst;
StudyMap     map;
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

/**
 * analyze
 *
 * @returns [out] {VideoCreatorComparison}
 */
const VideoCreatorComparison analyze() const {
  VideoCreatorComparison comparison{m_map};

  comparison.analyze();

  return comparison;
}

private:
StudyMap m_map;
};


#endif // __TOOLS_HPP__

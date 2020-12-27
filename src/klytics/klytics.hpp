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

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ Interfaces ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class SummaryGenerator {
public:
virtual ~SummaryGenerator() {}
virtual std::string generate_report() = 0;
};

class SocialMediaAnalyzer {
public:
virtual ~SocialMediaAnalyzer() {}
virtual std::string fetch_follower_count() = 0;
};

class YouTubeAnalyzer {
public:
virtual ~YouTubeAnalyzer() {}

virtual std::vector<VideoInfo> fetch_videos()       = 0;
virtual std::vector<VideoInfo> get_youtube_videos() = 0;
};

struct VideoCreatorComparison {};

class VideoAnalyzer {
public:
virtual ~VideoAnalyzer() {}

virtual bool add_videos(std::vector<VideoInfo> v) = 0;
virtual VideoCreatorComparison get_findings()     = 0;
};

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ Helpers ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/
ProcessResult execute(std::string program);

/**
  @class
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ KLytics ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class KLytics : public YouTubeAnalyzer,
                public SocialMediaAnalyzer,
                public SummaryGenerator,
                public VideoAnalyzer {

using VComparison = VideoCreatorComparison;
public:
virtual ~KLytics() {}

virtual std::string            fetch_follower_count()             override;
virtual std::string            generate_report()                  override;
virtual std::vector<VideoInfo> fetch_videos()                     override;
virtual std::vector<VideoInfo> get_youtube_videos()               override;
virtual bool                   add_videos(std::vector<VideoInfo>) override;
virtual VideoCreatorComparison get_findings()                     override;

std::string                    generate_video_stats_table();

private:
API               m_api;
ContentComparator m_comparator;
};

} // namespace klytics

#endif // __KLYTICS_HPP__

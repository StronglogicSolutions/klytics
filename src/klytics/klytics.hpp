#pragma once

#include <iostream>
#include <ctime>
#include <string>
#include <process.hpp>
#include <ktube/ktube.hpp>
#include "common/types.hpp"
#include "common/constants.hpp"
#include "common/util.hpp"

namespace klytics {
using Video                  = ktube::Video;
using VideoCreatorComparison = ktube::VideoCreatorComparison;
using GoogleTrend            = ktube::GoogleTrend;


struct ExecuteConfig {
std::string username;
};

const inline ExecuteConfig ParseRuntimeArguments(int argc, char** argv)
{
  ExecuteConfig config{};

  for (int i = 1; i < argc; i++) {
    std::string argument{argv[i]};

    if (argument.find("--user") == 0) {
      config.username = AlphaNumericOnly(argument.substr(7));
    }
  }

  return config;
}

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

virtual std::vector<Video> fetch_videos()       = 0;
virtual std::vector<Video> get_youtube_videos() = 0;
};

class VideoAnalyzer {
public:
virtual       ~VideoAnalyzer() {}

virtual       bool add_videos(std::vector<Video> v) = 0;
virtual const VideoCreatorComparison get_findings()     = 0;
};

class TrendsAnalyzer {
public:
virtual ~TrendsAnalyzer() {}
virtual std::vector<GoogleTrend> fetch_trends(std::vector<std::string> terms) = 0;
};

/**
  @class
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ KLytics ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class KLytics : public VideoAnalyzer,
                public SocialMediaAnalyzer,
                public YouTubeAnalyzer,
                public TrendsAnalyzer,
                public SummaryGenerator {

public:
virtual                                ~KLytics()                                   override;

virtual       std::string              fetch_follower_count()                       override;
virtual       std::string              generate_report()                            override;
virtual       std::vector<Video>       fetch_videos()                               override;
virtual       std::vector<Video>       get_youtube_videos()                         override;
virtual       bool                     add_videos(std::vector<Video> videos)        override;
virtual const VideoCreatorComparison   get_findings()                               override;
virtual       std::vector<GoogleTrend> fetch_trends(std::vector<std::string> terms) override;

              std::string              fetch_trends_string(std::vector<std::string> terms);
              std::string              generate_video_stats_table();
              std::string              fetch_ig_posts(const std::string& username);
              std::string              fetch_yt_posts(const std::string& channel_id);

private:
ktube::YouTubeDataAPI    m_api;
ktube::ContentComparator m_comparator;
};

} // namespace klytics

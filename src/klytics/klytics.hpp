#pragma once

#include <iostream>
#include <ctime>
#include <string>
#include <process.hpp>
#include <ktube/ktube.hpp>
#include "common/types.hpp"
#include "common/constants.hpp"
#include "kutils.hpp"

namespace klytics {
using Video                  = ktube::Video;
using VideoCreatorComparison = ktube::VideoCreatorComparison;
using GoogleTrend            = ktube::GoogleTrend;

struct ExecuteConfig {
std::string username;
std::string subject;
};

static ExecuteConfig ParseRuntimeArguments(int argc, char** argv)
{
  ExecuteConfig config{};

  for (int i = 1; i < argc; i++)
  {
    std::string argument{argv[i]};

    if (!argument.find("--user"))
      config.username = kutils::AlphaNumericOnly(argument.substr(7));
    else
    if (!argument.find("--header"))
      config.subject  = argument.substr(9);


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
                                       KLytics();
virtual                                ~KLytics()                                   final;
virtual       std::string              fetch_follower_count()                       final;
virtual       std::string              generate_report()                            final;
virtual       std::vector<Video>       fetch_videos()                               final;
virtual       std::vector<Video>       get_youtube_videos()                         final;
virtual       bool                     add_videos(std::vector<Video> videos)        final;
virtual const VideoCreatorComparison   get_findings()                               final;
virtual       std::vector<GoogleTrend> fetch_trends(std::vector<std::string> terms) final;

              std::string              fetch_trends_string(std::vector<std::string> terms);
              std::string              generate_video_stats_table();
              std::string              fetch_ig_posts (const std::string& username);
              std::string              fetch_yt_posts (const std::string& channel_id);
              std::string              fetch_tw_posts (const std::string& username);
              std::string              search_tw_posts(const std::string& subject);

private:
  ktube::YouTubeDataAPI    m_api;
  ktube::ContentComparator m_comparator;
  std::string              m_ig_feed_app_path;
  std::string              m_tw_feed_app_path;
  std::string              m_yt_foll_app_path;
  std::string              m_ig_foll_app_path;
};

} // namespace klytics

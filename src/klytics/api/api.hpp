#ifndef __API_HPP__
#define __API_HPP__
#include <cpr/cpr.h>

#include "interface.hpp"
#include "results.hpp"
#include "klytics/auth/auth.hpp"
#include "analysis/html.hpp"
#include "analysis/tools.hpp"

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░ API CLASS ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/
class API : public SecureAPI,
            public VideoAPI {

public:

  API();

  virtual bool                     is_authenticated() override;
  virtual bool                     init()             override;
  virtual bool                     has_videos()       override;

          std::vector<ChannelInfo> fetch_channel_info(std::string id_string);
          std::vector<VideoInfo>   fetch_channel_videos();
          std::vector<VideoStats>  fetch_video_stats(std::string id_string);
          std::vector<VideoInfo>   fetch_youtube_stats();
          std::vector<VideoInfo>   fetch_rival_videos(VideoInfo video);
          std::vector<VideoInfo>   find_similar_videos(VideoInfo video);
          std::vector<VideoInfo>   get_videos();
          std::vector<GoogleTrend> fetch_google_trends(std::vector<std::string> terms);
          std::vector<TermInfo>    fetch_term_info(std::vector<std::string> terms);
          std::vector<VideoInfo>   fetch_videos_by_terms(std::vector<std::string> terms);

  const   uint32_t                 get_quota_used() const;

private:
  Authenticator            m_authenticator;
  std::vector<VideoInfo>   m_videos;
  uint32_t                 m_quota;
  std::vector<std::string> m_channels;
};

#endif // __API_HPP__

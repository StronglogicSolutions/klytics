#include "klytics.hpp"

namespace klytics {
ProcessResult execute(std::string program) {
  return qx({program}, get_executable_cwd());
}

/**
 * fetch_follower_count
 * @returns [out] {std::string}
 */
std::string KLytics::fetch_follower_count() {
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
std::string KLytics::generate_report() {
  std::string::size_type extra_text_size{102};

  std::string            output, video_stats_output, competitor_stats_output;
  std::string            follower_count = fetch_follower_count();
  std::vector<VideoInfo> videos         = m_api.fetch_youtube_stats();
  std::vector<VideoInfo> c_videos;

  if (!videos.empty()) {
    video_stats_output      = videos_to_html(videos);
    c_videos = m_api.find_similar_videos(videos.front());
    competitor_stats_output = videos_to_html(c_videos);
  }

  output.reserve(
    follower_count.size() + video_stats_output.size() + competitor_stats_output.size() + extra_text_size
  );

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

  /** add video sets to comparator **/
  // TODO: VideoInfo should become a class with a container of videos and an ID member
  if (!videos.empty() && !c_videos.empty()) {
    if (!m_comparator.add_content(videos.front().channel_id, videos))
      log("Unable to add videos to comparator");
    if (!m_comparator.add_content(c_videos.front().channel_id, c_videos))
      log("Unable to add competitor videos to comparator");
  }

  return output;
}

/**
 * generate_video_stats_table
 *
 * @returns [out] {std::string}
 */
std::string KLytics::generate_video_stats_table() {
  return videos_to_html(
    m_api.fetch_youtube_stats()
  );
}

/**
 * fetch_videos
 */
std::vector<VideoInfo> KLytics::fetch_videos() {
  return m_api.fetch_channel_videos();
}
/**
 * get_youtube_videos
 *
 * @returns [out] {std::vector<VideoInfo}
 */
std::vector<VideoInfo> KLytics::get_youtube_videos() {
  return m_api.get_videos();
}

/**
 * add_videos
 */
bool KLytics::add_videos(std::vector<VideoInfo> v) {
  if (!v.empty()) {
    m_comparator.add_content(v.front().channel_id, v);
    return true;
  }
  return false;
}

/**
 * get_findings
 */
VideoCreatorComparison KLytics::get_findings() {
  return VideoCreatorComparison{};
}

} // namespace klytics

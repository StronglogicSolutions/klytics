#include "klytics.hpp"

namespace klytics {


/**
 * @destructor
 */

KLytics::~KLytics() {
  // Save number of youtube quota units used in this session
  // TODO: read and append for each day
  uint32_t youtube_quota_units_used = m_api.get_quota_used();
  SaveToFile(
    std::to_string(youtube_quota_units_used),
    get_executable_cwd() + constants::YOUTUBE_QUOTA_PATH
  );
}
/**
 * fetch_follower_count
 * @returns [out] {std::string}
 */
std::string KLytics::fetch_follower_count() {
  std::string   output{};
  // ProcessResult result = execute(constants::FOLLOWER_YT_APP);

  // if (result.error) {
  //   output += "Error executing followers app\n\n";
  // }

  // YouTubeFollowResult json{};
  // if (json.read(result.output)) {
  //   output += json.to_string();
  // }

  std::string ig_user = INIReader{
    std::string{get_executable_cwd() + "../" + constants::DEFAULT_CONFIG_PATH}
  }.GetString(constants::INSTAGRAM_CONFIG_SECTION, constants::INSTAGRAM_USERNAME, "");

  if (!ig_user.empty()) {
    std::string command{constants::FOLLOWER_IG_APP + " --i=\"true\" --u=\"" + ig_user + "\""};
    log(command);
    ProcessResult ig_result = execute(constants::FOLLOWER_IG_APP + " --i=\"true\" --u=\"" + ig_user + "\"");

    if (ig_result.error) {
      output += "Error return IG Followers app: " + ig_result.output + "\n\n";
    }

    InstagramFollowResult json{};
    if (json.read(ig_result.output)) {
      output += json.to_string();
    }
  }

  return output;
}


/**
 * generate_report
 *
 * @returns [out] {std::string}
 */
std::string KLytics::generate_report() {
  std::string::size_type extra_text_size{102};

  std::string            output, youtube_stats_output;
  std::string            competitor_stats_output{};
  std::string            follower_count = fetch_follower_count();
  std::vector<ChannelInfo> channel_data = m_api.fetch_youtube_stats();

  // TODO: modify "find_similar_videos" to return ChannelInfo objects
  std::vector<ChannelInfo> similar_channels;

  if (!channel_data.empty()) {
    youtube_stats_output = channel_videos_to_html(channel_data);
    similar_channels = m_api.find_similar_videos(channel_data.front().videos.front());
    competitor_stats_output = channel_videos_to_html(similar_channels);
  }

  output.reserve(
    follower_count.size() + youtube_stats_output.size() + competitor_stats_output.size() + extra_text_size
  );

  output += "FOLLOWER COUNT\n\n";
  output += follower_count;
  output += "\n\n";
  output += "LATEST VIDEOS\n\n";
  output += youtube_stats_output;
  output += "\n\n";
  output += "COMPETITOR VIDEOS (based on your most recent video)\n\n";
  output += competitor_stats_output;
  output += "\n\n";
  output += "QUOTA USED: " + std::to_string(m_api.get_quota_used()) + "\n\n";
  output += "Have a nice day";

  /** add video sets to comparator **/
  if (!channel_data.empty() && !channel_data.front().videos.empty()) {
    for (const auto& channel : channel_data) {
      if (!channel.videos.empty()) {
        if (!m_comparator.add_content(channel.videos.front().channel_id, channel.videos))
          log("Unable to add videos to comparator");
      }
    }
    for (const auto& channel : similar_channels) {
      if (!channel.videos.empty()) {
        if (!m_comparator.add_content(channel.videos.front().channel_id, channel.videos))
          log("Unable to add competitor videos to comparator");
      }
    }
  }

  return output;
}

/**
 * generate_video_stats_table
 *
 * @returns [out] {std::string}
 */
std::string KLytics::generate_video_stats_table() {
  return channel_videos_to_html(
    m_api.fetch_youtube_stats()
  );
}

/**
 * fetch_videos
 */
std::vector<VideoInfo> KLytics::fetch_videos() {
  std::vector<VideoInfo> videos;
  if (m_api.fetch_channel_videos()) {
    videos = m_api.get_videos();
  }
  return videos;
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
const VideoCreatorComparison KLytics::get_findings() {
  return m_comparator.analyze();
}

/**
 * fetch_trends
 *
 * @param  [in]  {std::vector<std::string>} terms
 * @return [out] {std::vector<GoogleTrend>}
 */
std::vector<GoogleTrend> KLytics::fetch_trends(std::vector<std::string> terms) {
  return m_api.fetch_google_trends(terms);
}

std::string KLytics::fetch_trends_string(std::vector<std::string> terms) {
  std::string result{};
  std::vector<GoogleTrend> trends = fetch_trends(terms);

  for (const auto& trend : trends) {
    result += "Term: " + trend.term + " Value: " + std::to_string(trend.value) + "\n";
  }

  return result;
}
} // namespace klytics

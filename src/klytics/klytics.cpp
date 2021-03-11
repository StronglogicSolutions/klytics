#include "klytics.hpp"

namespace klytics {
/**
 * @destructor
 */

KLytics::~KLytics() {
using namespace ktube;
  // Save number of youtube quota units used in this session
  // TODO: read and append for each day
  uint32_t youtube_quota_units_used = m_api.get_quota_used();
  SaveToFile(
    std::to_string(youtube_quota_units_used),
    get_current_working_directory() + constants::YOUTUBE_QUOTA_PATH
  );
}
/**
 * fetch_follower_count
 * @returns [out] {std::string}
 */
std::string KLytics::fetch_follower_count() {
  std::string   output{};

  // YOUTUBE
  ProcessResult result = ktube::execute(constants::FOLLOWER_YT_APP);

  if (result.error) {
    output += "Error executing followers app\n\n";
  }

  ktube::YouTubeFollowResult json{};
  if (json.read(result.output)) {
    output += json.to_string();
  }

  // INSTAGRAM
  std::string ig_user = INIReader{
    std::string{get_current_working_directory() + "../" + constants::DEFAULT_CONFIG_PATH}
  }.GetString(constants::INSTAGRAM_CONFIG_SECTION, constants::INSTAGRAM_USERNAME, "");

  if (!ig_user.empty()) {
    std::string result = ktube::system_read(constants::FOLLOWER_IG_APP + " --i=\"true\" --u=\"" + ig_user + "\"");
    if (result.empty()) {
      output += "Error running IG Followers app\n\n";
    }

    ktube::InstagramFollowResult json{};
    if (json.read(result)) {
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
  using namespace ktube;
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
std::vector<Video> KLytics::fetch_videos() {
  std::vector<Video> videos;
  if (m_api.fetch_channel_videos()) {
    videos = m_api.get_videos();
  }
  return videos;
}
/**
 * get_youtube_videos
 *
 * @returns [out] {std::vector<Video}
 */
std::vector<Video> KLytics::get_youtube_videos() {
  return m_api.get_videos();
}

/**
 * add_videos
 */
bool KLytics::add_videos(std::vector<Video> v) {
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

/**
 * @brief
 *
 * @param username
 * @return std::string
 */
std::string KLytics::fetch_ig_posts(const std::string& username)
{
  std::string output{};
  std::string result = ktube::system_read(constants::USERFEED_IG_APP + " --user=" + username);

  if (result.empty())
    output += "Error executing followers app\n\n";
  else
  {
    IGFeedJSONResult feed_result{};
    if (feed_result.read(result))
      output += feed_result.to_string();
  }

  return output;
}

/**
 * @brief
 *
 * @param channel_id
 * @return std::string
 */
std::string KLytics::fetch_yt_posts(const std::string& channel_id)
{
  using namespace ktube;

  nlohmann::json data =
  [this, &channel_id]()
  {
    nlohmann::json json_data = nlohmann::json::array();

    for (const auto& channel : m_api.fetch_youtube_stats())
    {
      if (channel.id == channel_id)
      {
        for (const auto& video : channel.videos)
        {
          nlohmann:json video_object{};
          video_object["channel_id"]  = video.channel_id;
          video_object["id"]          = video.id;
          video_object["title"]       = video.title;
          video_object["description"] = video.description;
          video_object["datetime"]    = video.datetime;
          video_object["keywords"]    = nlohmann::json::array();

          for (auto&& keyword : video.stats.keywords)
            video_object["keywords"].emplace_back(keyword);

          json_data.emplace_back(video_object);
        }
      }
    }
    return json_data;
  }();

  return data.dump();
}

} // namespace klytics

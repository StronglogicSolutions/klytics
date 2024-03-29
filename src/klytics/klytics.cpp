#include "klytics.hpp"
#include <process.hpp>

namespace klytics {

static std::string read_config(const std::string& section, const std::string& key)
{
  static const auto config = ktube::GetConfigReader();
  return config.GetString(section, key, "");
}

KLytics::KLytics()
{
  using namespace constants;
  if (ktube::GetConfigReader().ParseError() < 0)
    throw std::invalid_argument{"No configuration path"};

  if (m_ig_feed_app_path = read_config(KLYTICS_CONFIG_SECTION, IG_FEED_APP_KEY); m_ig_feed_app_path.empty())
    throw std::invalid_argument{"IG Feed app path must be set in configuration"};

  if (m_tw_feed_app_path = read_config(KLYTICS_CONFIG_SECTION, TW_FEED_APP_KEY); m_tw_feed_app_path.empty())
    throw std::invalid_argument{"TW Feed app path must be set in configuration"};

  if (m_yt_foll_app_path = read_config(KLYTICS_CONFIG_SECTION, YT_FOLL_APP_KEY); m_yt_foll_app_path.empty())
    throw std::invalid_argument{"YT follower app path must be set in configuration"};

  if (m_ig_foll_app_path = read_config(KLYTICS_CONFIG_SECTION, IG_FOLL_APP_KEY); m_ig_foll_app_path.empty())
    throw std::invalid_argument{"IG follower app path must be set in configuration"};
}

/**
 * @destructor
 */
KLytics::~KLytics() {
  kutils::SaveToFile(std::to_string(m_api.get_quota_used()),
             ktube::get_executable_cwd() + constants::YOUTUBE_QUOTA_PATH);
}
/**
 * fetch_follower_count
 * @returns [out] {std::string}
 */
std::string KLytics::fetch_follower_count()
{
  std::string   output;
  ProcessResult result = ktube::execute(m_yt_foll_app_path);

  if (result.error)
    output += "Error executing followers app\n\n";

  if (ktube::YouTubeFollowResult json{}; json.read(result.output))
    output += json.to_string();

  std::string ig_user = read_config(constants::INSTAGRAM_CONFIG_SECTION, constants::INSTAGRAM_USERNAME);

  if (!ig_user.empty())
  {
    std::string result = ktube::system_read(m_ig_foll_app_path + " --i=\"true\" --u=\"" + ig_user + "\"");
    if (result.empty())
      output += "Error running IG Followers app\n\n";

    if (ktube::InstagramFollowResult json{}; json.read(result))
      output += json.to_string();
  }

  return output;
}


/**
 * generate_report
 *
 * @returns [out] {std::string}
 */
std::string KLytics::generate_report()
{
  using namespace ktube;
  std::string::size_type extra_text_size{102};

  std::string            output, youtube_stats_output;
  std::string            competitor_stats_output{};
  std::string            follower_count = fetch_follower_count();
  std::vector<ChannelInfo> channel_data = m_api.fetch_youtube_stats();
  std::vector<ChannelInfo> similar_channels;

  if (!channel_data.empty())
  {
    youtube_stats_output    = channel_videos_to_html(channel_data);
    similar_channels        = m_api.find_similar_videos(channel_data.front().videos.front());
    competitor_stats_output = channel_videos_to_html(similar_channels);
  }


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
  if (!channel_data.empty() && !channel_data.front().videos.empty())
  {
    for (const auto& channel : channel_data)
      if (!channel.videos.empty() &&
          !m_comparator.add_content(channel.videos.front().channel_id, channel.videos))
        log("Unable to add videos to comparator");

    for (const auto& channel : similar_channels)
      if (!channel.videos.empty() &&
          !m_comparator.add_content(channel.videos.front().channel_id, channel.videos))
        log("Unable to add competitor videos to comparator");
  }

  return output;
}

/**
 * generate_video_stats_table
 *
 * @returns [out] {std::string}
 */
std::string KLytics::generate_video_stats_table()
{
  return channel_videos_to_html(m_api.fetch_youtube_stats());
}

/**
 * fetch_videos
 */
std::vector<Video> KLytics::fetch_videos()
{
  std::vector<Video> videos;
  if (m_api.fetch_channel_videos())
    videos = m_api.get_videos();
  return videos;
}
/**
 * get_youtube_videos
 *
 * @returns [out] {std::vector<Video}
 */
std::vector<Video> KLytics::get_youtube_videos()
{
  return m_api.get_videos();
}

/**
 * add_videos
 */
bool KLytics::add_videos(std::vector<Video> v)
{
  if (!v.empty())
  {
    m_comparator.add_content(v.front().channel_id, v);
    return true;
  }
  return false;
}

/**
 * get_findings
 */
const VideoCreatorComparison KLytics::get_findings()
{
  return m_comparator.analyze();
}

/**
 * fetch_trends
 *
 * @param  [in]  {std::vector<std::string>} terms
 * @return [out] {std::vector<GoogleTrend>}
 */
std::vector<GoogleTrend> KLytics::fetch_trends(std::vector<std::string> terms)
{
  return m_api.fetch_google_trends(terms);
}

std::string KLytics::fetch_trends_string(std::vector<std::string> terms)
{
  std::string result;
  for (const auto& trend : fetch_trends(terms))
    result += "Term: " + trend.term + " Value: " + std::to_string(trend.value) + "\n";
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

  static const char bad_chars[] = "\ufffdｎｇ";

  auto sanitize_json = [](auto& s) {for (auto it = s.find(bad_chars); it != std::string::npos; it = s.find(bad_chars))
    s.erase(it); };

  std::string output{};
  std::string result = ktube::system_read(m_ig_feed_app_path + " --user=" + username);

  if (result.empty())
    output += "Error executing ig feed app\n\n";
  else
  {
    sanitize_json(result);
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
          video_object["datetime"]    = std::to_string(kutils::to_unixtime(video.datetime.c_str()));
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

/**
 * @brief
 *
 * @param username
 * @return std::string
 */
std::string KLytics::fetch_tw_posts(const std::string& username)
{
  std::string output{};
  std::string result = ktube::system_read(m_tw_feed_app_path + " --user=" + username);

  if (result.empty())
    output += "Error executing tw feed app\n\n";
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
 * @param subject
 * @return std::string
 */
std::string KLytics::search_tw_posts(const std::string& subject)
{
  std::string output{};
  std::string result = ktube::system_read(m_tw_feed_app_path + " --header=" + subject);

  if (result.empty())
    output += "Error executing tw feed app\n\n";
  else
  {
    IGFeedJSONResult feed_result{};
    if (feed_result.read(result))
      output += feed_result.to_string();
  }

  return output;
}

} // namespace klytics

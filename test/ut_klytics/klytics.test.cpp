#include "klytics.test.hpp"

TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

TEST(KLyticsTest, FindComparableVideosFromCompetitor) {

  klytics::KLytics klytics{};

  klytics.fetch_video_stats();

  std::vector<VideoInfo> videos       = klytics.get_youtube_videos();
  std::vector<VideoInfo> rival_videos = klytics.find_similar_videos(videos.front());
  tabulate::Table        table        = videos_to_table(rival_videos);

}


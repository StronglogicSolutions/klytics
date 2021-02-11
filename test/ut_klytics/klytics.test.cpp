#include "klytics.test.hpp"

/**
 * Instantiate
 */
TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

/**
* ComparatorComparesVideoVectors
*/
TEST(KLyticsTest, ComparatorComparesVideoVectors) {
  using namespace klytics;
  using VideoAnalysis = ktube::VideoAnalyst::VideoAnalysis;

  bool all_videos_added{true};
  KLytics klytics{};

  std::vector<std::vector<VideoInfo>> videos{
    GetTestVideos_1(),
    GetTestVideos_2()
  };

  for (const auto& video : videos) {
    if (!klytics.add_videos(video)) {
      all_videos_added = false;
    }
  }

  VideoCreatorComparison  comparison_result = klytics.get_findings();
  VideoAnalysis           analysis          = comparison_result.get_result();
  ktube::ResultMap        result_map        = analysis.get_result_map();

  auto most_likes_it           = result_map.at(analysis.most_likes_key).most_likes;
  auto most_dislikes_it        = result_map.at(analysis.most_dislikes_key).most_dislikes;
  auto best_viewscore_it       = result_map.at(analysis.best_viewscore_key).top_view_score;
  auto best_likescore_it       = result_map.at(analysis.best_likescore_key).top_view_score;
  auto best_dislikescore_it    = result_map.at(analysis.best_dislikescore_key).top_view_score;
  auto best_commentscore_it    = result_map.at(analysis.best_commentscore_key).top_view_score;
  auto most_likes              = std::stoi(most_likes_it->stats.likes);
  auto most_dislikes           = std::stoi(most_dislikes_it->stats.dislikes);
  auto best_viewscore          = best_viewscore_it->stats.view_score;
  auto most_likes_channel      = analysis.most_likes_channel_name();
  auto most_dislikes_channel   = analysis.most_dislikes_channel_name();
  auto best_viewscore_channel  = analysis.best_viewscore_channel_name();

  std::cout <<
    "\n\nMOST LIKES\n"     << *most_likes_it     <<
    "\n\nMOST DISLIKES\n"  << *most_dislikes_it  <<
    "\n\nBEST VIEWSCORE\n" << *best_viewscore_it <<
  std::endl;

  EXPECT_TRUE(all_videos_added);
  EXPECT_TRUE(most_likes > 0);
  EXPECT_TRUE(most_dislikes > 0);
  EXPECT_TRUE(best_viewscore > 0);

  EXPECT_FALSE(result_map.empty());
  EXPECT_FALSE(analysis.most_likes_key.empty());
  EXPECT_FALSE(analysis.most_dislikes_key.empty());
  EXPECT_FALSE(analysis.best_viewscore_key.empty());

  EXPECT_EQ(most_likes_channel,     most_likes_it->channel_id);
  EXPECT_EQ(most_dislikes_channel,  most_dislikes_it->channel_id);
  EXPECT_EQ(best_viewscore_channel, best_viewscore_it->channel_id);
}

TEST(KLyticsTest, GoogleTrends) {
  using namespace klytics;

  KLytics klytics{};

  std::vector<GoogleTrend> trends = klytics.fetch_trends(GetTestTerms());

  std::cout << "Google Trends" << std::endl;
  for (const auto& trend : trends) {
    std::cout << "Trend: " << trend.term << "Value: " << trend.value << std::endl;
  }

  EXPECT_FALSE(trends.empty());
  EXPECT_EQ(trends.front().term, GetTestTerms().front());
}

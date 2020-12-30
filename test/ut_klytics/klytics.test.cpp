#include "klytics.test.hpp"

TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

/**

*/
TEST(KLyticsTest, ComparatorComparesVideoVectors) {
  using namespace klytics;
  using VideoAnalysis = VideoAnalyst::VideoAnalysis;
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
  ResultMap               result_map        = analysis.get_result_map();

  auto most_likes_it     = result_map.at(analysis.most_likes_index).most_likes;
  auto most_dislikes_it  = result_map.at(analysis.most_dislikes_index).most_dislikes;
  auto best_viewscore_it = result_map.at(analysis.best_view_score_index).top_view_score;
  auto most_likes        = std::stoi(most_likes_it->stats.likes);
  auto most_dislikes     = std::stoi(most_dislikes_it->stats.dislikes);
  auto best_viewscore    = best_viewscore_it->stats.view_score;

  std::cout << "\nMost likes: "      << most_likes     <<
                "\nMost dislikes: "  << most_dislikes  <<
                "\nBest Viewscore: " << best_viewscore << std::endl;

  EXPECT_TRUE(all_videos_added);
  EXPECT_FALSE(result_map.empty());
  EXPECT_FALSE(analysis.most_likes_index.empty());
  EXPECT_FALSE(analysis.most_dislikes_index.empty());
  EXPECT_FALSE(analysis.best_view_score_index.empty());

  EXPECT_TRUE(most_likes > 0);
  EXPECT_TRUE(most_dislikes > 0);
  EXPECT_TRUE(best_viewscore > 0);

}
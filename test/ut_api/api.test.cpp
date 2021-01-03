#include "api.test.hpp"

/**
 * Instantiate
 */
TEST(APITest, Instantiate) {
  EXPECT_NO_THROW(API{});
}

/**
 * ComputeYouTubeKeywordScore
 */
TEST(APITest, ComputeYouTubeKeywordScore) {
  API api{};

  std::vector<TermInfo> term_info_v =
  api.fetch_term_info(
    api.fetch_youtube_stats().front().get_primary_keywords()
  );

  for (const auto& info : term_info_v)
    std::cout << "Term:  " << info.term << "\nScore: " << info.value << "\n" << std::endl;

  std::cout << "Used " << api.get_quota_used() << " quota units" << std::endl;

  EXPECT_FALSE(term_info_v.empty());
}

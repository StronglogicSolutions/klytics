#include <iostream>
#include "klytics/klytics.hpp"

int main(int argc, char** argv) {
  klytics::KLytics k_lytics{};

  std::string      std_out{};

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "followers") == 0) {
        std_out += k_lytics.fetch_follower_count() + "\n";
      }
      if (strcmp(argv[i], "stats") == 0) {
        std_out += k_lytics.generate_video_stats_table()  + "\n";
      }
      if (strcmp(argv[i], "report") == 0) {
        std_out += k_lytics.generate_report()    + "\n";
      }

    }
  }

  std::cout << std_out << std::endl;

  return 0;
}

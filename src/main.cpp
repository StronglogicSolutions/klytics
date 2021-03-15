#include <iostream>
#include <stdio.h>
#include "klytics/klytics.hpp"

int main(int argc, char** argv) {
  klytics::KLytics       k_lytics{};

  std::string            std_out{};
  klytics::ExecuteConfig config = klytics::ParseRuntimeArguments(argc, argv);

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      std::string arg = ktube::SanitizeInput(argv[i]);

      if (arg == "followers")
        std_out += k_lytics.fetch_follower_count() + "\n";
      else
      if (arg == "stats")
        std_out += k_lytics.generate_video_stats_table()  + "\n";
      else
      if (arg == "report")
        std_out += k_lytics.generate_report()    + "\n";
      else
      if (arg == "ig_feed")
        if (config.username.empty())
          throw std::invalid_argument{"No username provided"};
        else
          std_out += k_lytics.fetch_ig_posts(config.username);
      else
      if (arg == "yt_feed")
        if (config.username.empty())
          throw std::invalid_argument{"No username provided"};
        else
          std_out += k_lytics.fetch_yt_posts(config.username);
    }
  }

  std::cout << std_out << std::endl;

  return 0;
}

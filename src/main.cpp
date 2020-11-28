#include <iostream>
#include <stdio.h>
#include "klytics/klytics.hpp"

int main(int argc, char** argv) {
  klytics::KLytics k_lytics{};

  std::string      std_out{};

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      std::string arg = SanitizeInput(arg);

      if (arg.compare("followers") == 0) {
        std_out += k_lytics.fetch_follower_count() + "\n";
      }
      if (arg.compare("stats") == 0) {
        std_out += k_lytics.generate_video_stats_table()  + "\n";
      }
      if (arg.compare("report") == 0) {
        std_out += k_lytics.generate_report()    + "\n";
      }
      if (arg.compare("test") == 0) {
        std_out += "TEST!\n";
      }
    }
  }

  std::cout << std_out << std::endl;

  return 0;
}

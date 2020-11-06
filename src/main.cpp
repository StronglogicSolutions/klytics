#include <iostream>
#include "klytics/klytics.hpp"

int main(int argc, char** argv) {
  klytics::KLytics k_lytics{};

  auto result = k_lytics.get_follower_count();

  std::cout << "Result is: \n" << result << std::endl;

  return 0;
}

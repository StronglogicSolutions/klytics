#ifndef __KLYTICS_HPP__
#define __KLYTICS_HPP__

#include <iostream>
#include <ctime>
#include <string>
#include <process.hpp>

namespace klytics {
namespace constants {
const std::string FOLLOWER_APP{"../external/followers/followers.js"};
} // namespace constants

ProcessResult execute(std::string program);

class KLytics {
public:

std::string get_follower_count() {
  ProcessResult result = execute(constants::FOLLOWER_APP);

  if (result.error) {
    return "Error executing followers app";
  }

  return result.output;
}
};

} // namespace klytics

#endif // __KLYTICS_HPP__

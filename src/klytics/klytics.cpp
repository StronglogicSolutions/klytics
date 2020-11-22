#include "klytics.hpp"

namespace klytics {
ProcessResult execute(std::string program) {
  return qx({program}, get_executable_cwd());
}

} // namespace klytics

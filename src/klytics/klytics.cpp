#include "klytics.hpp"

namespace klytics {
ProcessResult execute(std::string program) {
  return qx({program}, get_executable_cwd());
}

void FormatTable(tabulate::Table& table) {
using namespace tabulate;
  for (int i = 0; i < 3; ++i) {
    table[0][i].format()
      .font_color(Color::blue)
      .font_align(FontAlign::center)
      .font_style({FontStyle::bold});
  }
  table.column(2).format()
  .font_align(FontAlign::right);
}
} // namespace klytics

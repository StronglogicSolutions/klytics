#include "api.hpp"

void FormatTable(tabulate::Table& table, uint8_t column_num) {
  using namespace tabulate;

  for (int i = 0; i < column_num; ++i) {
    table[0][i].format()
      .font_color(Color::blue)
      .font_align(FontAlign::center)
      .font_style({FontStyle::bold});
  }
}
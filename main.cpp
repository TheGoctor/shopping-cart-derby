#include "src/Game.h"

int main(int argc, char** argv) {
  scd::game app({argc, argv});
  return app.exec();
}

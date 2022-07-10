#include "unlockable_manager.h"

#include <fstream>

unlockable_manager::unlockable_manager() {
  // see what we have unlocked
  _file_name = "Unlockables.bin";
  std::ifstream fin(_file_name.c_str(), std::ios_base::binary);

  if (fin.is_open()) {
    // get unlocked character and other statuses
    fin.read(&_characters_unlocked, sizeof(bool));
    fin.read(&_allow_intro_skip, sizeof(bool));
    fin.read(&_player_notified, sizeof(bool));
    fin.close();
    fin.clear();
  }
}

void unlockable_manager::update_file() {
  std::ofstream out(_file_name.c_str(), ios_base::trunc);
  if (out.is_open()) {
    out.write(&_characters_unlocked, sizeof(bool));
    out.write(&_allow_intro_skip, sizeof(bool));
    out.write(&_player_notified, sizeof(bool));
    out.close();
  }
}

void unlockable_manager::unlock_features() {
  allow_intro_skip();
  unlock_characters();
  notify_player();
}

#include "unlockable_manager.h"

#include <fstream>

unlockable_manager::unlockable_manager(const std::string& file_name)
  : _file_name(file_name)
{
  read_file();
}

void unlockable_manager::read_file() {
  std::ifstream fin(_file_name.c_str(), std::ios_base::binary);

  if (fin.is_open()) {
    // get unlocked character and other statuses
    fin.read(reinterpret_cast<char*>(&_characters_unlocked), sizeof(bool));
    fin.read(reinterpret_cast<char*>(&_allow_intro_skip), sizeof(bool));
    fin.read(reinterpret_cast<char*>(&_player_notified), sizeof(bool));
  }
}

void unlockable_manager::update_file() const {
  std::ofstream out(_file_name.c_str(), std::ios_base::trunc);
  if (out.is_open()) {
    out.write(reinterpret_cast<const char*>(&_characters_unlocked), sizeof(bool));
    out.write(reinterpret_cast<const char*>(&_allow_intro_skip), sizeof(bool));
    out.write(reinterpret_cast<const char*>(&_player_notified), sizeof(bool));
  }
}

void unlockable_manager::unlock_features() {
  allow_intro_skip();
  unlock_characters();
  notify_player();
}

#pragma once

#include <string>

class unlockable_manager {
public:
  unlockable_manager(const std::string& file_name);

  void unlock_features();

  void read_file();

  // updates the file to reflect any changes
  void update_file() const;

  // accessors and mutators
  std::string get_file_name() const { return _file_name; }
  bool get_characters_unlocked() const { return _characters_unlocked; }
  bool was_player_notified() const { return _player_notified; }
  bool can_skip_intro() const { return _allow_intro_skip; }

  void allow_intro_skip() {
    _allow_intro_skip = true;
    update_file();
  }
  void unlock_characters() {
    _characters_unlocked = true;
    update_file();
  }
  void notify_player() {
    _player_notified = true;
    update_file();
  }

private:
  // bools for which characters are unlocked
  std::string _file_name;

  // whether the player has unlocked the hidden characters
  bool _characters_unlocked = false;

  // whether the player has been alerted to the new characters
  bool _player_notified = false;

  // whether the player has reached the main menu once
  bool _allow_intro_skip = false;
};

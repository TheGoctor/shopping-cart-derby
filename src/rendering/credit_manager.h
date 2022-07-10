////////////////////////////////////////////////////////////////////////////////
//  File			:	"CCreditManager.h"
//
//  Author			:	Huy Nguyen (HN)
//
//  Date Created	:	05/18/11
//
//	Last Changed	:	07/24/11
//
//	Changed By		:	HN
//
//  Purpose			:	Wrapper class for managing the credit screen
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <string>

namespace scd {
class base_component;
class event;
class object;
class texture_manager;
class object_manager;
class sprite;
class font_component;
class button;
class scroller;

class credit_manager {
public:
  static credit_manager& get() {
    static credit_manager _manager;
    return _manager;
  }
  void init(void);
  void shutdown(void);
  void update(void);
  static void on_update(event* pEvent, base_component* pComp);
  static void on_init_credits(event* pEvent, base_component* pComp);
  static void on_disable_credits(event* pEvent, base_component* pComp);

private:
  texture_manager* _texture_manager;
  object_manager* _object_manager;
  sprite* _font_sprite;
  font_component* _credits;

  float m_fSeconds;
  float m_fDT;

  std::array<object*, 2> _credit_objects;

  float fX, fY;

  std::string m_szCredits;

  // credit start
  button* pButton;
  scroller* pScroller;

  // credit middle
  button* pButton2;
  scroller* pScroller2;

  // credit end
  button* pButton3;
  scroller* pScroller3;

  credit_manager();
  ~credit_manager();
};

} // namespace scd

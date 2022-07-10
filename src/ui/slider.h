#ifndef __scd_components_slider_h__
#define __scd_components_slider_h__

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "component.h"

// For TSpriteData
#include "Managers\Global Managers\Rendering Managers\Texture Managers\CTextureManager.h"
#include "enums.h"
#include "object.h"

#include "component/button.h"

namespace scd {
namespace components {
  class IEvent;

  class slider : public scd::button {
  private:
    scd::vector3 _start_position;
    scd::vector3 _end_position;

    int _slider_for;

    float _slider_value;
    float _state_entered_slider_value;

  public:
    slider(scd::object* owner);

    static int create(lua_State* pLua);
    static slider* create(
      scd::object* owner, std::string event_name, std::string sprite_texture_name,
      int left, int top, int right, int bottom, int texture_depth,
      bool start_selected, int slider_for, int game_state);

    void init(std::string button_texture_name, int texture_depth);
    void reinit();
    void activate();
    void deactivate();
    void hide();

    void set_screen_position(int left, int top, int right, int bottom);

    void calculate_and_send_float_event();
    void move_slider_one_tick(int direction);
    void set_slider_to_value(float amount, bool send_value = true);
    static int set_slider_value(lua_State* pLua);

    ///////////////////////////////////////////////////////////////////////////
    //	Name:			GameWon
    //	Parameters:		scd::base_component*		cSender	//The sender of the event
    //					IEvent			cEvent	//Actually a CUpdateEvent
    //											//Holds the frame's delta time
    //	Return:			void
    //	Description:	Calls all of the functions that need to
    //					 be called every frame. This function is
    //					 called by the event system.
    ///////////////////////////////////////////////////////////////////////////

    static void SelectPressed(IEvent* cEvent, scd::base_component* cCenter);
    static void SlideLeft(IEvent* cEvent, scd::base_component* cCenter);
    static void SlideRight(IEvent* cEvent, scd::base_component* cCenter);
    static void SliderStateInit(IEvent* cEvent, scd::base_component* cCenter);
    static void SliderCancelPause(IEvent* cEvent, scd::base_component* cCenter);
    static void SliderCancelMain(IEvent* cEvent, scd::base_component* cCenter);

    void SliderCancelNonStatic(int eState);
  };

} // namespace components
} // namespace scd

#endif // __scd_components_slider_h__

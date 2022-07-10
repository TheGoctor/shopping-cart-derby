#pragma once

//#include "audio/wwise/wwise_ids.h"

namespace scd {
// type enum for the shape of the bounding volume of an object
enum class shape { sphere = 1, aabb, box, line, plane, frustum, capsule };

// type enum for the object holding the collideable component
// NOTE: OBJPITEM was replaced with OBJHELDITEM, but the replacement was added.
// . .
// please make sure you replace when you're replacing, don't just add it in
enum class object_type {
  player = 1,
  goal_item,
  p_item,
  wall,
  shelf,
  end_cap,
  checkout,
  camera,
  frustum,
  held_item,
  fsd,
  banana,
  pie,
  peanut_butter,
  turkey,
  soup,
  donut,
  jam,
  energy_drink
};

enum class player_id { invalid = -1, player_1, player_2, player_3, player_4, max };

enum class held_item_type {
  none = -1,
  turkey,
  banana,
  peanut_butter,
  pie,
  energy_drink,
  soup,
  donut,
  jam,
  max_held_items,
  max_attack_items = pie
};

enum class heap_type { general, event, render };

enum class event_priority {
  immediate,
  normal,
  input,
  update,
  collision,
  render,
  shutdown
};

enum class object_id {
  global = 0,
  biker,
  sasha,
  larper,
  bandito,
  scientist,
  crygame,
  storytime,
  item,
  max
};

enum class department {
  none = -1,
  dairy,
  dry_goods,
  freezer,
  bakery,
  canned_goods,
  produce,
  deli,
  meats,
  snacks,
  max
};

enum class goal_item_type {
  none = -1,
  gorilla_milk,
  captain_flakey,
  pizza,
  cake,
  canned_bears,
  carrots,
  sammy_sandwich,
  squid,
  agent_orange,
  max
};

enum class character_type {
  biker = 0,
  bandito,
  larper,
  scientist,
  sasha,
  crygame,
  storytime,
  max
};

// Addd so I don't have to hardcode this value -Smith
#define NUM_ITEMS_IN_LEVEL 8

// Game States
// NOTE:	When adding or removing a state, verify with lua that
//				the numbers are still corresponding to the actual enums
//				(E.G. Objects associated with 1, the MAIN_MENU_STATE will
//				not be correct if something is added before MAIN_MENU_STATE
//				in this enum declaration since MAIN_MENU_STATE will be 2 at
//				that point.)
//			The safe way to add values is to add them to the end of the list,
//				before MAX_STATE. That way, a check is unnecessary.
//				(Also, label in comments the state name at any location you use a
//				hard number instead of the enum name, i.e. in lua, to assist
//				in any future changes)
//											- Smith
enum class game_state {
  min   = -1,
  intro = '0',
  main_menu,
  gameplay,
  pause,
  win,
  lose,
  console,
  game_mode_select,
  character_select_0,
  character_select_1,
  character_select_2,
  character_select_3,
  character_select_4,
  character_select_5,
  character_select_6,
  options,
  credits,
  keybind,
  how_to_play_0,
  how_to_play_1,
  how_to_play_2,
  how_to_play_3,
  how_to_play_4,
  how_to_play_5,
  how_to_play_6,
  how_to_play_7,
  how_to_play_8,
  how_to_play_9,
  how_to_play_10,
  how_to_play_11,
  how_to_play_12,
  how_to_play_13,
  how_to_play_14,
  how_to_play_15,
  pause_options,
  pause_keybinds,
  in_game_how_to_play,
  level_select_0,
  level_select_1,
  quit_confirmation,
  in_game_how_to_play_controller,
  max
};

///////////////////////////////////////////////
//		CHARACTER SET ENUM
//	Used Currently for Shadow Comps
///////////////////////////////////////////////
enum class character_set {
  min = -1,
  bikers,
  banditos,
  larpers,
  scientists,
  sasha,
  storytime,
  endgame,
  max
};

///////////////////////////////////////////////
//		RENDER FUCTION ENUMS
///////////////////////////////////////////////
enum class render_function {
  min = -1,
  vertex,
  indexed_vertex,
  index_vertex_texture2,
  bones,
  animation,
  particle,
  texture_change,
  vertex_texture2,
  texture_change_instance,
  max
};

///////////////////////////////////////////////////////
//       RENDER CONTEXT ENUMS
//////////////////////////////////////////////////////
enum class render_context {
  min = -1,
  flat,
  // level geomery
  ab_cart,
  ac_duct,
  bakery_sign,
  //basic_cutout,
  basic_cutout_a,
  basic_cutout_b,
  basic_cutout_c,
  boys_room,
  bb_window,
  butcher_sign,
  boxes,
  ceiling_0,
  ceiling_1,
  dairy,
  deli_sign,
  deli_window,
  employee_door,
  employee_of_the_month,
  end_cap,
  exit_sign,
  floor_0,
  floor_1,
  floor_2,
  floor_3,
  floor_4,
  floor_5,
  floor_6,
  floor_7,
  floor_8,
  floor_9,
  floor_trim,
  front_door,
  fsd_cl,
  fsd_cm,
  fsd_cs,
  fsd_sl,
  fsd_sm,
  fsd_ss,
  girls_room,
  glass_counter,
  glass_freezer,
  half_wall,
  help_desk,
  lights,
  lobster_sign,
  lobster_tank,
  meat,
  ofl,
  ofr,
  outside,
  pallets,
  pirate,
  produce_left,
  produce_right,
  register,
  shelf,
  shelf_stuff,
  snowman,
  startlight,
  umbra,
  wall_0,
  wall_1,
  wall_support,
  breakup_sign_0,
  breakup_sign_1,
  alien,
  little_evelyn,

  // goal items
  gorilla_milk,
  captain_flakey,
  pizza,
  cake,
  canned_bears,
  carrots,
  sammy_sandwich,
  squid,
  agent_orange,
  // character models
  RC_ANIM_STEVE,
  RC_ANIM_EUGENE,
  RC_ANIM_SCICART,
  RC_ANIM_BULLDOG,
  RC_ANIM_MA,
  RC_ANIM_BIKCART,
  RC_ANIM_SEMOUR,
  RC_ANIM_DAVE,
  RC_ANIM_LARPCART,
  RC_ANIM_GERMINAL,
  RC_ANIM_RICO,
  RC_ANIM_BANDCART,
  RC_ANIM_SASHA,
  RC_ANIM_FITZ,
  RC_ANIM_PHAROH,
  RC_ANIM_ALIENCART,
  RC_ANIM_EVELYN,
  RC_ANIM_SHADE,
  RC_ANIM_NLCART,
  RC_ANIM_DONUT,

  // power ups
  RC_BANANA,
  RC_CHICKENSOUP,
  RC_DONUT,
  RC_TURKEYBOX,
  RC_JAM,
  RC_PEANUTBUTTER,
  RC_PIE,
  RC_THOR,
  RC_TURKEY,
  RC_PEANUTPILE,

  // Effects
  RC_CIRCLE_SHADOW,
  RC_SHELF_SHADOW,
  RC_FSDS_L_SHADOW,
  RC_SQUARE_SHADOW,
  RC_CASHIER_SHADOW,

  RC_BLUE_LIGHT_SPECIAL,

  RC_CART_SKID_MARKS,
  RC_SCI_SKIDS,
  RC_PB_MARKS,
  RC_CART_MUD,
  RC_SNOWFLAKE_EFFECT,
  RC_SNOWFLAKE_B_EFFECT,

  RC_FLOOR_SPLAT,
  RC_CHARACTER_SHADOW_BLOB,

  RC_CART_SMOKE_EFFECT,
  RC_ITEM_EFFECT_GLOW,
  RC_ITEM_EFFECT_SHINE,
  RC_ITEM_EFFECT_STAR,

  RC_GOAL_ICON_EFFECT_SHEET,

  RC_BOOST_GLOW_BLUE,
  RC_BOOST_LIGHTNING,
  RC_CLOUD_GRAY,
  RC_TURKEY_KRACK_SMALL,
  RC_TURKEY_KRACK_BIG,
  RC_TURKEY_KRACK,

  RC_BROTH,
  RC_SOUP_SPARKLE,
  RC_SHLOOP_POPUP,
  RC_DRIP_LEFT,
  RC_DRIP_RIGHT,
  RC_PB_DRIP_LEFT,
  RC_PB_DRIP_RIGHT,
  RC_SQUISH_POPUP,

  RC_COLLISION_EFFECT_GREEN_SMALL,
  RC_COLLISION_EFFECT_GREEN_MID,
  RC_COLLISION_EFFECT_GREEN_BIG,
  RC_COLLISION_EFFECT_CRASH_POPUP,

  RC_COLLISION_EFFECT_RED_SMALL,
  RC_COLLISION_EFFECT_RED_MID,
  RC_COLLISION_EFFECT_RED_BIG,
  RC_COLLISION_EFFECT_POW_POPUP,

  RC_COLLISION_EFFECT_BLUE_SMALL,
  RC_COLLISION_EFFECT_BLUE_MID,
  RC_COLLISION_EFFECT_BLUE_BIG,
  RC_COLLISION_EFFECT_SMASH_POPUP,

  RC_COLLISION_EFFECT_YELLOW_SMALL,
  RC_COLLISION_EFFECT_YELLOW_MID,
  RC_COLLISION_EFFECT_YELLOW_BIG,
  RC_COLLISION_EFFECT_CRUNCH_POPUP,

  RC_JAM_USE_EFFECT_JAR,
  RC_JAM_HIT_EFFECT_BLOB,
  RC_JAM_HIT_EFFECT_SPLURCH_POPUP,
  RC_DONUT_EFFECT_PUFF,
  RC_BLUE_PUFF,
  RC_SPLAT_BURST,
  RC_SPLAT_POPUP,
  RC_FIREWORK_BLUE,
  RC_FIREWORK_GREEN,
  RC_FIREWORK_BLACK,
  RC_FIREWORK_ORANGE,
  RC_BOLT,
  RC_CONFETTI_STRIP,
  RC_CONFETTI_STAR,
  RC_CAUTION,

  RC_MAX
};

///////////////////////////////////////////////////////
//       SOUND API ENUMS
//////////////////////////////////////////////////////
enum class sound_id {
  ////Menu sounds
  MENU_SELECT            ,//= AK::EVENTS::PLAY_SFX_MENU_SELECT,
  MENU_INVALID_SELECTION ,//= AK::EVENTS::PLAY_SFX_MENU_INVALIDSELECTION,
  MENU_SCROLL            ,//= AK::EVENTS::PLAY_SFX_MENU_SCROLL,
  MENU_OPTION_CHANGE     ,//= AK::EVENTS::PLAY_SFX_MENU_OPTIONCHANGE,
  MENU_CASHREGISTER      ,//= AK::EVENTS::PLAY_SFX_MENU_CASHREGISTER,
  MENU_MUSIC_PLAY        ,//= AK::EVENTS::PLAY_MX_MENU,

  // Gameplay sounds and Annoucer Dialog
  GAMEPLAY_MUSIC_STOP     ,//= AK::EVENTS::STOP_MX_GAMEPLAY,
  START_COUNTDOWN         ,//= AK::EVENTS::PLAY_DX_GAMEPLAY_STARTRACECOUNTDOWN,
  WELCOME_TO              ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_WELCOME,
  ORANGE_SPAWN_SOUND      ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_AGENTORANGE,
  BEAR_IN_CAN_SPAWN_SOUND ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_BEARINACAN,
  BIRTHDAYCAKE_SPAWN      ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_BIRTHDAYCAKE,
  BLUEBOMBER_SPAWN        ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_BLUEBOMBER,
  CAPTAINFLAKYS_SPAWN     ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_CAPTAINFLAKYS,
  CARROTS_SPAWN           ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_CARROTS,
  GORILLAMILD_SPAWN       ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_GORILLAMILK,
  PIRATESQUID_SPAWN       ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_PIRATESQUID,
  SAMMYTHESANDWICH_SPAWN  ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_SAMMYTHESANDWICH,
  LEADTAKEN_BANDITOS      ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_LEADTAKEN_BANDITOS,
  LEADTAKEN_BIKERS        ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_LEADTAKEN_BIKERS,
  LEADTAKEN_LARPERS       ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_LEADTAKEN_LARPERS,
  LEADTAKEN_SASHA         ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_LEADTAKEN_SASHA,
  LEADTAKEN_SCIENTISTS    ,//= AK::EVENTS::PLAY_DX_ANNOUNCER_LEADTAKEN_SCIENTISTS,
  GOAL_ITEM_DROP          ,//= AK::EVENTS::PLAY_SFX_ITEM_GOALITEM_DROP,
  ITEM_PICK_UP            ,//= AK::EVENTS::PLAY_SFX_ITEM_PICKUP,
  WINSTATE_EXPLOSION      ,//= AK::EVENTS::PLAY_SFX_HUD_FIREWORKS,

  // Held item sounds
  ITEM_CREAMPIE_USE     ,//= AK::EVENTS::PLAY_SFX_ITEM_CREAMPIE_USE,
  ITEM_ENERGYDRINK_USE  ,//= AK::EVENTS::PLAY_SFX_ITEM_ENERGYDRINK_USE,
  ITEM_JAM_USE          ,//= AK::EVENTS::PLAY_SFX_ITEM_JAM_USE,
  ITEM_JAM_HIT          ,//= AK::EVENTS::PLAY_SFX_ITEM_JAM_HIT,
  ITEM_PEANUTBUTTER_USE ,//= AK::EVENTS::PLAY_SFX_ITEM_PEANUTBUTTER_USE,
  ITEM_DOUGHNUT_USE     ,//= AK::EVENTS::PLAY_SFX_ITEM_DOUGHNUT_USE,
  ITEM_DOUGHNUT_AURA    ,//= AK::EVENTS::PLAY_SFX_ITEM_DOUGHNUT_AURA,
  ITEM_DOUGHNUT_IMPACT  ,//= AK::EVENTS::PLAY_SFX_ITEM_DOUGHNUT_IMPACT,
  ITEM_CHICKENSOUP_USE  ,//= AK::EVENTS::PLAY_SFX_ITEM_CHICKENSOUP,
  ITEM_TURKEY_USE       ,//= AK::EVENTS::PLAY_SFX_ITEM_TURKEY_USE,
  ITEM_TURKEY_TRAVEL    ,//= AK::EVENTS::PLAY_SFX_ITEM_TURKEY_TRAVEL,
  ITEM_TURKEY_IMPACT    ,//= AK::EVENTS::PLAY_SFX_ITEM_TURKEY_IMPACT,
  ITEM_TURKEY_EN_IMPACT ,//= AK::EVENTS::PLAY_SFX_ITEM_TURKEY_BOUNCE,

  // Biker Sounds
  BULLDOG_CART_BRAKE      ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_CART_BRAKE,
  BULLDOG_CART_BOOST      ,//= AK::EVENTS::PLAY_SFX_ITEM_BOOST,
  BULLDOG_HURT            ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_HURT,
  BULLDOG_COLLECTION      ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_COLLECTION,
  BULLDOG_ITEM_USE        ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_ITEMUSE,
  BULLDOG_SHOVE           ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_SHOVE,
  BULLDOG_VICTORY         ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_VICTORY,
  BULLDOG_SPEED_PLAY      ,//= AK::EVENTS::PLAY_SFX_CHARACTERS_BULLDOGMA_SPEED,
  BULLDOG_BIKE_SLIP       ,//= AK::EVENTS::PLAY_SFX_CART_BIKE_SLIP,
  BULLDOG_BIKE_START      ,//= AK::EVENTS::PLAY_SFX_CART_BIKE_START,
  BULLDOG_BIKE_START_FAIL ,//= AK::EVENTS::PLAY_SFX_CART_BIKE_START_FAIL,
  BULLDOG_ALLGOALITEMS    ,//= AK::EVENTS::PLAY_DX_BULLDOG_ALLGOALITEMS,
  BULLDOG_LOSS            ,//= AK::EVENTS::PLAY_DX_BULLDOG_LOSS,

  // Larper Sounds
  LARPER_ALLGOALITEMS ,//= AK::EVENTS::PLAY_DX_DAVE_ALLGOAL,
  LARPER_COLLECTION   ,//= AK::EVENTS::PLAY_DX_DAVE_COLLECTION,
  LARPER_HURT         ,//= AK::EVENTS::PLAY_DX_DAVE_HURT,
  LARPER_ITEMUSE      ,//= AK::EVENTS::PLAY_DX_DAVE_ITEMUSE,
  LARPER_LOSS         ,//= AK::EVENTS::PLAY_DX_DAVE_LOSS,
  LARPER_SHOVE        ,//= AK::EVENTS::PLAY_DX_DAVE_SHOVE,
  LARPER_VICTORY      ,//= AK::EVENTS::PLAY_DX_DAVE_VICTORY,
  WOODCART_BRAKE      ,//= AK::EVENTS::PLAY_SFX_WOODCART_BRAKE,
  WOODCART_DRIFT      ,//= AK::EVENTS::PLAY_SFX_WOODCART_DRIFT,
  WOODCART_SPEED      ,//= AK::EVENTS::PLAY_SFX_WOODCART_SPEED,

  // Scientist Sounds
  SCIENTIST_COLLECTION   ,//= AK::EVENTS::PLAY_DX_SCIENTIST_COLLECTION,
  SCIENTIST_HURT         ,//= AK::EVENTS::PLAY_DX_SCIENTIST_HURT,
  SCIENTIST_ITEMUSE      ,//= AK::EVENTS::PLAY_DX_SCIENTIST_ITEMUSE,
  SCIENTIST_LOSS         ,//= AK::EVENTS::PLAY_DX_SCIENTIST_LOSS,
  SCIENTIST_SHOVE        ,//= AK::EVENTS::PLAY_DX_SCIENTIST_SHOVE,
  SCIENTIST_VICTORY      ,//= AK::EVENTS::PLAY_DX_SCIENTIST_VICTORY,
  SCIENTIST_ALLGOALITEMS ,//= AK::EVENTS::PLAY_DX_SCIENTIST_ALLGOALITEMS,
  SCIENTIST_BRAKE        ,//= AK::EVENTS::PLAY_SFX_SCIENTIST_BRAKE,
  SCIENTIST_DRIFT        ,//= AK::EVENTS::PLAY_SFX_SCIENTIST_DRIFT,
  SCIENTIST_SPEED        ,//= AK::EVENTS::PLAY_SFX_SCIENTIST_SPEED,

  // Bandito sounds
  BANDITOS_ALLGOALITEMS ,//= AK::EVENTS::PLAY_DX_GERMINAL_ALLGOALITEMS,
  BANDITOS_COLLECTION   ,//= AK::EVENTS::PLAY_DX_GERMINAL_COLLECTION,
  BANDITOS_HURT         ,//= AK::EVENTS::PLAY_DX_GERMINAL_HURT,
  BANDITOS_ITEMUSE      ,//= AK::EVENTS::PLAY_DX_GERMINAL_ITEMUSE,
  BANDITOS_LOSS         ,//= AK::EVENTS::PLAY_DX_GERMINAL_LOSS,
  BANDITOS_SHOVE        ,//= AK::EVENTS::PLAY_DX_GERMINAL_SHOVE,
  BANDITOS_VICTORY      ,//= AK::EVENTS::PLAY_DX_GERMINAL_VICTORY,

  // SASHA sounds
  SASHA_BRAKE        ,//= AK::EVENTS::PLAY_SFX_SASHA_BRAKE,
  SASHA_DRIFT        ,//= AK::EVENTS::PLAY_SFX_SASHA_DRIFT,
  SASHA_SPEED        ,//= AK::EVENTS::PLAY_SFX_SASHA_SPEED,
  SASHA_ALLGOALITEMS ,//= AK::EVENTS::PLAY_DX_SASHA_ALLGOALITEMS,
  SASHA_HURT         ,//= AK::EVENTS::PLAY_DX_SASHA_HURT,
  SASHA_ITEMUSE      ,//= AK::EVENTS::PLAY_DX_SASHA_ITEMUSE,
  SASHA_LOSE         ,//= AK::EVENTS::PLAY_DX_SASHA_LOSE,
  SASHA_PICKUP       ,//= AK::EVENTS::PLAY_DX_SASHA_COLLECTION,
  SASHA_WIN          ,//= AK::EVENTS::PLAY_DX_SASHA_WIN,

  // CryGame Sounds
  CRYGAME_ALLGOALITEMS ,//= AK::EVENTS::PLAY_DX_FITZ_ALLGOAL,
  CRYGAME_COLLECTION   ,//= AK::EVENTS::PLAY_DX_FITZ_COLLECTION,
  CRYGAME_HURT         ,//= AK::EVENTS::PLAY_DX_FITZ_HURT,
  CRYGAME_ITEMUSE      ,//= AK::EVENTS::PLAY_DX_FITZ_ITEMUSE,
  CRYGAME_LOSS         ,//= AK::EVENTS::PLAY_DX_FITZ_LOSS,
  CRYGAME_SHOVE        ,//= AK::EVENTS::PLAY_DX_FITZ_SHOVE,
  CRYGAME_VICTORY      ,//= AK::EVENTS::PLAY_DX_FITZ_VICTORY,

  // Story Time Sounds
  STORYTIME_ALLGOALITEMS ,//= AK::EVENTS::PLAY_DX_GIRL_ALLGOAL,
  STORYTIME_COLLECTION   ,//= AK::EVENTS::PLAY_DX_GIRL_COLLECTION,
  GSTORYTIME_HURT        ,//= AK::EVENTS::PLAY_DX_GIRL_HURT,
  STORYTIME_LOSS         ,//= AK::EVENTS::PLAY_DX_GIRL_LOSS,
  STORYTIME_SHOVE        ,//= AK::EVENTS::PLAY_DX_GIRL_SHOVE,
  STORYTIME_VICTORY      ,//= AK::EVENTS::PLAY_DX_GIRL_VICTORY,

  // Cart sounds
  CART_WALL_COLLISION   ,//= AK::EVENTS::PLAY_SFX_IMPACT_CART_WALL,
  CART_PLAYER_COLLISION ,//= AK::EVENTS::PLAY_SFX_IMPACT_CART_ENEMY,

  // Status sounds
  STATUS_BLIND  ,//= AK::EVENTS::PLAY_SFX_PLAYER_STATUS_BLIND,
  STATUS_FROZEN ,//= AK::EVENTS::PLAY_SFX_PLAYER_STATUS_FROZEN,
  STATUS_SLOWED ,//= AK::EVENTS::PLAY_SFX_PLAYER_STATUS_SLOWED_01,
  STATUS_STUN   ,//= AK::EVENTS::PLAY_SFX_PLAYER_STATUS_STUN,

  // Stop sounds
  ITEM_DOUGHNUT_AURA_STOP ,//= AK::EVENTS::STOP_SFX_ITEM_DOUGHNUT_AURA,
  STATUS_FROZEN_STOP      ,//= AK::EVENTS::STOP_SFX_PLAYER_STATUS_FROZEN,
  BULLDOG_SPEED_STOP      ,//= AK::EVENTS::STOP_SFX_CHARACTERS_BULLDOGMA_SPEED,
  ITEM_TURKEY_TRAVEL_STOP ,//= AK::EVENTS::STOP_SFX_ITEM_TURKEY_TRAVEL,
  MENU_MUSIC_STOP         ,//= AK::EVENTS::STOP_MX_MENU,
  CHICKENSOUP_STOP        ,//= AK::EVENTS::STOP_SFX_ITEM_CHICKENSOUP_LOOP,
  SCIENTIST_SPEED_STOP    ,//= AK::EVENTS::STOP_SFX_SCIENTIST_SPEED,
  WOODCART_SPEED_STOP     ,//= AK::EVENTS::STOP_SFX_WOODCART_SPEED,
  SASHA_SPEED_STOP        ,//= AK::EVENTS::STOP_SFX_SASHA_SPEED,
  STOP_ALL_EXCEPT         ,//= AK::EVENTS::STOP_ALL_EXCEPTMENU,

  max
};

} // namespace scd

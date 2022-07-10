#pragma once

#include "components/held_items/banana.h"
#include "components/held_items/chicken_soup.h"
#include "components/held_items/donut.h"
#include "components/held_items/energy_drink.h"
#include "components/held_items/jam.h"
#include "components/held_items/peanut_butter.h"
#include "components/held_items/pie.h"
#include "components/held_items/turkey.h"
#include "enums.h"
#include "events/event_manager.h"

namespace scd {
class held_item_manager {
private:
  scd::list<component::turkey*> _turkeys;
  scd::list<donut*> _donuts;
  scd::list<energy_drink*> _energy_drinks;
  scd::list<banana*> _bananas;
  scd::list<chicken_soup*> _soups;
  scd::list<pie*> _pies;
  scd::list<jam*> _jams;
  scd::list<peanut_butter*> _peanut_butters;

  unsigned int m_nRenderContexts[MAX_GOAL_ITEMS];

public:
  /**
   * @brief Listens for the use pie event and creates and initializes data for
   * the pie to be shot.
   *
   * @param cEvent The event data
   */
  void on_use_pie(IEvent* cEvent);

  /**
   * @brief Listens for the use turkey event and creates and initializes data
   * for the turkey to be shot.
   *
   * @param cEvent The event data
   */
  void on_use_turkey(IEvent* cEvent);

  /**
   * @brief Listens for the use boost event and creates and initializes data for
   * the energy drink to be used.
   *
   * @param cEvent The event data
   */
  void on_use_boost(IEvent* cEvent);

  /**
   * @brief Listens for the use banana event and creates and initializes data
   * for the banana to be used.
   *
   * @param cEvent The event data
   */
  void on_use_banana(IEvent* cEvent);

  /**
   * @brief Listens for the use peanut butter event and creates and initializes
   * data for the peanut butter to be shot.
   *
   * @param cEvent The event data
   */
  void on_use_peanut_butter(IEvent* cEvent);

  /**
   * @brief Listens for the use soup event and creates and initializes data for
   * the soup to be used.
   *
   * @param cEvent The event data
   */
  void on_use_soup(IEvent* cEvent);

  /**
   * @brief Listens for the use donut event and creates and initializes data for
   * the donut to be used.
   *
   * @param cEvent The event data
   */
  void on_use_donut(IEvent* cEvent);

  /**
   * @brief Listens for the use jam event and creates and initializes data for
   * the jam to be used.
   *
   * @param cEvent The event data
   */
  void on_use_jam(IEvent* cEvent);

  /**
   * @brief Create a turkey object or recycles an unused one.
   *
   * @param position The position to place the turkey.
   * @return turkey* A new or recycled turkey component.
   */
  turkey* create_turkey(const scd::vector3& position);

  /**
   * @brief Create a banana object or recycles an unused one.
   *
   * @return banana* A new or recycled banana component.
   */
  banana* create_banana();

  /**
   * @brief Create a chicken soup object or recycles an unused one.
   *
   * @return chicken_soup* A new or recycled chicken_soup component.
   */
  chicken_soup* create_soup();

  /**
   * @brief Create an energy drink object or recycles an unused one.
   *
   * @return energy_drink* A new or recycled energy_drink component.
   */
  energy_drink* create_energy_drink();

  /**
   * @brief Create a pie object or recycles an unused one.
   *
   * @param position The position to place the pie.
   * @return pie* A new or recycled pie component.
   */
  pie* create_pie(const scd::vector3& position);

  /**
   * @brief Create a peanut butter object or recycles an unused one.
   *
   * @return peanut_butter* A new or recycled peanut butter component.
   */
  peanut_butter* create_peanut_butter();

  /**
   * @brief Create a donut object or recycles an unused one.
   *
   * @return donut* A new or recycled donut component.
   */
  donut* create_donut();

  /**
   * @brief Create a jam object or recycles an unused one.
   *
   * @return jam* A new or recycled jam component.
   */
  jam* create_jam();
};
} // namespace scd

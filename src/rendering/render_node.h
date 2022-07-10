////////////////////////////////////////////////////////////////////////////////
//	File			:	RenderNode.h
//	Date			:	5/10/11
//	Mod. Date		:	5/25/11
//	Mod. Initials	:	JL
//	Author			:	Joseph Leybovich
//	Purpose			:	Encapsulates the Chaining of Renderable
// Object
////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace scd {
class render_node {
private:
  friend class renderer;

protected:
  render_node* _next{nullptr}; // Pointer to the Next Node

public:
  // Render Function Pointer
  void (*render_function)(render_node& node);

  // Depth Render Function Pointer
  void (*depth_render_function)(render_node&, render_node&);

  // Render the Node with the Associated Render Function
  void render_process() { render_function(*this); }

  render_node* next() const { return _next; }
  void next(render_node* node) { _next = node; }
};
} // namespace scd

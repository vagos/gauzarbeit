// This is the base Widget class for all the UI elements.
// It holds a list of all the other widgets on the screen as a static member variable.
// It has some basic mouse interaction routines which can be utilised with the onSomething() functions.

#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <list>
#include <SFML/Graphics.hpp>
#include "../Graphics.hpp"

class Widget {
 public:
  int x, y, w, h;
  static Widget* focus;
  static Widget* keyboardFocus;

  Widget();
  Widget(int x, int y);
  Widget(int x, int y, int w, int h);
  virtual ~Widget();
  
  virtual void doRender(Graphics& gfx) {}
  virtual void doUpdate(Graphics& gfx) {}

  void move(int x_, int y_);
  
  void toggleVisible() { bVisible = !bVisible; }
  bool isVisible();

  void doFocus() {focus = this;}
  void doUnfocus() {focus = nullptr;}

  virtual void getKeyboardInput(Graphics& gfx) {}

  virtual void computeSize() {}
  
  void setBackgroundColor(const sf::Color col, const sf::Color colFocus);
  void setForegroundColor(const sf::Color col, const sf::Color colFocus);
  
  static void updateWidgets(Graphics& gfx);
  static void renderWidgets(Graphics& gfx);
  
  virtual void setPos(int x_, int y_) {x = x_; y = y_;}
  virtual void doMove(int x_, int y_) {}

  bool is_inside(int x_, int y_) {return x_ >= x && x_ <= x + w && y_ >= y && y_ <= y + h;}
 
  sf::Color colorFore       = sf::Color::White;
  sf::Color colorForeFocus  = sf::Color::Yellow;
  sf::Color colorBack       = sf::Color::Black;
  sf::Color colorBackFocus  = sf::Color::Black;
 
 private:
    
  virtual void _doUpdate(Graphics& gfx);
  virtual void _doRender(Graphics& gfx);

 protected:
  friend class Container;

  virtual void onMouseIn() {}
  virtual void onMouseOut() {}
  virtual void onButtonPress() {}
  virtual void onButtonRelease() {}
  virtual void onButtonClick() {}

  static std::list<Widget*> listWidgets;
  
  Widget * wParent = nullptr;
  bool bMouseInside     = false;
  bool bMousePressed    = false;
  bool bVisible         = true;
};

#endif /* WIDGET_HPP */


class Driver{ // shouldn't be a class
public:
  Display* display;
  Page* activePage;
  Point lastTouch;

  void update(){
    Point p = this->display->getTouchPoint();
    if(p != lastTouch){
      this->activePage->registerTouch(&p);
      lastTouch = p;
    }

    this->activePage->draw();
  } // #updateDraw
private:

} // Class Driver

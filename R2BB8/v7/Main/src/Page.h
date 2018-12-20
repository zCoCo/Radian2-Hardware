class Page{
public:
  PageItem** items;

  void draw(){
    for each item:{
      i->draw();
    }
  } // #draw

  void registerTouch(Point* tp){
    for each item{
      if i->contains(tp){
        i->action->perform();
      }
    }
  } // #registerTouch
private:

} // Class Page

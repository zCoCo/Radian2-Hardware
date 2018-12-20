class PageItem{
public:
  Point* centroid;
  GraphicsObject* go;
  Action* action;

  void draw(){
    this->go->draw();
  } // #draw
  
  void contains(){

  } // #contains

private:

} // Class PageItem

class Action{
public:
  void* argument;
  void()(void *) actionType;

  Action(void()(void*) a, argument arg){

  } // ctor

  void perform(){
    this->action(this->argument);
  } // #perform
private:

} // Class Action

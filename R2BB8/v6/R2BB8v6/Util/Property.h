/****
 * Class Property: Container Class for a Named Property, Containing a Value of
 * Type, T.
****/
#ifndef _CLASS_PROPERTY_H
#define _CLASS_PROPERTY_H

template <class T>
class Property {
  private:
    char* _name;//    -Name of Property
    T _val;//         -Value of Property

  public:
    Property(char* n, T v){
      _name = n;
      _val = v;
    }
    ~Property(){
      delete []_name;
      delete &_val;
    }

    /*
    Returns the value of _name prior to calling and, if given a argument,
    resets its value.
    */
    char* name(char* new_val = NULL){
      char* old_val = _name;
      if(!new_val){
        _name = new_val;
      }
      return old_val;
    }
    /*
    Returns the value of _val prior to calling and, if given a argument,
    resets its value.
    */
    T value(T new_val = NULL){
      T old_val = _val;
      if(!new_val){
        _val = new_val;
      }
      return old_val;
    }
};


#endif //_CLASS_PROPERTY_H

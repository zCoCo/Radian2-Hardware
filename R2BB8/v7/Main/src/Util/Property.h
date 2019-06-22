/****
 * Class Property: Container Class for a Named Property, Containing a Value of
 * Type, T.
****/
#ifndef _CLASS_PROPERTY_H
#define _CLASS_PROPERTY_H

template <class T>
class Property {
  private:
    const char* _name;//    -Name of Property
    T _val;//         -Value of Property

  public:
    Property(const char* n, T v){
      _name = n;
      _val = v;
    }
    ~Property(){
      delete []_name;
    }

    /*
    Returns the value of _name.
    */
    const char* name(){
      return _name;
    }
    /*
    Returns the value of _val prior to calling and, if given a argument,
    resets its value.
    */
    T value(T new_val = T()){
      if(new_val){
        T old_val = _val;
        _val = new_val;
        return old_val;
      } else{
        return _val;
      }
    }
};


#endif //_CLASS_PROPERTY_H

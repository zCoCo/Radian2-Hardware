#ifndef _UNITS_H
#define _UNITS_H

  /*
   * Set of named unit types for strong-typing measured quantities.
   * Example:
   *
   using Angle<T> = NamedType<Degree<T>, struct AngleParam>
   using Time<T> = NamedType<Millisecond<T>, struct AngleParam>
   void moveTo(Angle<T> pos, Time<T> duration){
     stepper.disableOutputs();
     delay(100);
     stepper.setSpeed(1000*pos*STEPS_PER_DEG/duration)
     stepper.move(pos*STEPS_PER_DEG);
     stepper.enableOutputs();
   }
   //...
   moveTo(Angle<int>(15_deg),);
   moveTo(Angle<float>(pi_rad));
   */
  #include <Util.h>

  using Degree<T> = NamedType<T, struct DegreeParam>;
  using Millisecond<T> = NamedType<T, struct MSParam>;

  template <typename T>
  Degree<T> operator"" _deg(T angle){
    return Degree<T>(angle);
  }
  template <typename T>
  Degree<T> operator"" _rad(T angle){
    return Degree<T>(angle * 0.01745329251f);
  }

  template <typename T>
  Millisecond<T> operator"" _ms(T time){
    return Millisecond<T>(time)
  }
  template <typename T>
  Millisecond<T> operator"" _s(T time){
    return Millisecond<T>(time * 1000)
  }
  template <typename T>
  Millisecond<T> operator"" _min(T time){
    return Millisecond<T>(time * 60000)
  }
  template <typename T>
  Millisecond<T> operator"" _hr(T time){
    return Millisecond<T>(time * 3600000)
  }

#endif // _UNITS_H

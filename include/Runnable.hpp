#ifndef RUNABLE_HPP
#define RUNABLE_HPP

struct Drawable {
  virtual ~Drawable() = default;
  virtual void draw() const = 0;
};

struct Updateable {
  virtual ~Updateable() = default;
  virtual void update() = 0;
};

struct Runnable : public Drawable, public Updateable {
  virtual ~Runnable() = default;
  virtual bool shouldStopRunning() const = 0;
  void run();
};

#endif
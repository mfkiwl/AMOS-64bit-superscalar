// Copyright (c) 2018 AMOS Developers
#ifndef INCLUDE_DEVICES_H_
#define INCLUDE_DEVICES_H_

#include <cstdlib>
#include <string>
#include <map>
#include <vector>

typedef uint64_t reg_t;

class abstract_device_t {
 public:
  virtual bool load(reg_t addr, size_t len, uint8_t* bytes) = 0;
  virtual bool store(reg_t addr, size_t len, const uint8_t* bytes) = 0;
  virtual ~abstract_device_t() {}
};

class bus_t : public abstract_device_t {
 public:
  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  void add_device(reg_t addr, abstract_device_t* dev);

  std::pair<reg_t, abstract_device_t*> find_device(reg_t addr);

 private:
  std::map<reg_t, abstract_device_t*> devices;
};

class mem_t : public abstract_device_t {
 public:
  mem_t(size_t size) : len(size) {
    data = (char*)calloc(1, size);
    if (!data)
      throw std::runtime_error("couldn't allocate " + std::to_string(size) + " bytes of target memory");
  }
  mem_t(const mem_t& that) = delete;
  ~mem_t() { free(data); }

  bool load(reg_t addr, size_t len, uint8_t* bytes);
  bool store(reg_t addr, size_t len, const uint8_t* bytes);
  char* contents() { return data; }
  size_t size() { return len; }

 private:
  char* data;
  size_t len;
};

#endif    // INCLUDE_DEVICES_H_


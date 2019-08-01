#pragma once
#include "FilePagedQueue_def.h"

namespace Utils {

  template<class T>
  class PersistentFilePagedQueue : public FilePagedQueue
  {
    public:
      // Constructor
      PersistentFilePagedQueue(std::string directory, std::string prefix, size_t page_size);
      ~PersistentFilePagedQueue();
    protected:
    private:
      void store();
      void restore();
  };

}

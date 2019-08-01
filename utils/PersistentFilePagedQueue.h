#pragma once
#include "PersistentFilePagedQueue_def.h"
#include "FilePagedQueue.h"

namespace Utils {
  template<class T>
  PersistentFilePagedQueue<T>::PersistentFilePagedQueue(std::string directory, std::string prefix, size_t page_size)
    : FilePagedQueue(directory,prefix,page_size)
  {
    restore();
  }

  template<class T>
  PersistentFilePagedQueue<T>::~PersistentFilePagedQueue() 
  {
    store();
  }

  template<class T>
  void PersistentFilePagedQueue<T>::store()
  {
    std::string settings_file = page_file(0);
    std::ofstream out(settings_file.c_str());
    if (!out.good()) {
      std::string message("Error opening queue file to write: ");
      throw std::runtime_error(message + file);
    }
    out << m_page_size << " ";
      m_records_paged << " ";
      m_current_read << " ";
      m_last_write << std::endl;
    out << size() << std::endl;
    std::queue<T> to_write;
    while (!empty()) {
      to_write.push(front());
      pop();
    }
    write_to_stream(out,to_write);
    out.close();
  }

  template<class T>
  void PersistentFilePagedQueue<T>::restore()
  {
    std::string settings_file = page_file(0);
    std::ifstream in(settings_file.c_str());
    if (!in.good()) {
      return;
    }

    in >> m_prefix;

  }
}

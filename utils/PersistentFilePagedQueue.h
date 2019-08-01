#pragma once
#include "PersistentFilePagedQueue_def.h"
#include "FilePagedQueue.h"
#include <stdexcept>

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
    syncronize();
    store();
  }

  template<class T>
  void PersistentFilePagedQueue<T>::store()
  {
    std::string settings_file = page_file(0);
    std::ofstream out(settings_file.c_str());
    if (!out.good()) {
      std::string message("Error opening settings file to write: ");
      throw std::runtime_error(message + settings_file);
    }
    std::string space(" ");
    out << m_page_size << space <<
      m_records_paged << space <<
      m_current_read << space <<
      m_last_write << std::endl;

    // Head
    out << m_head->size() << std::endl;
    write_to_stream(out,*m_head);

    // Tail
    if (!m_tail) {
      out << std::string("null") << std::endl;
    } else if (m_tail == m_head) {
      out << std::string("head") << std::endl;
    } else {
      out << std::string("live") << std::endl;
      out << m_tail->size() << std::endl;
      write_to_stream(out,*m_tail);
    }

    // Next
    if (!m_next) {
      out << std::string("null") << std::endl;
    } else if (m_next == m_tail) {
      out << std::string("tail") << std::endl;
    } else {
      out << std::string("live") << std::endl;
      out << m_next->size() << std::endl;
      write_to_stream(out,*m_next);
    }

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

    in >> m_page_size;
    in >> m_records_paged;
    in >> m_current_read;
    in >> m_last_write;

    size_t size;
    in >> size;
    read_from_stream(in, *m_head, size);

    // Tail
    std::string status;
    in >> status;
    if (status == "head") {
      m_tail = m_head;
    } else if (status == "live") {
      in >> size;
      m_tail = std::make_shared<std::queue<T>>();
      if (size > 0) {
        read_from_stream(in, *m_tail, size);
      }
    }

    // Next
    in >> status;
    if (status == "tail") {
      m_next = m_tail;
    } else if (status == "live") {
      in >> size;
      m_next = std::make_shared<std::queue<T>>();
      if (size > 0) {
        read_from_stream(in, *m_next, size);
      }
    }
    in.close();
    // Settings file no longer needed
    remove(settings_file.c_str());

  }
}

#pragma once
#include "FilePagedQueue_def.h"
#include <queue>
#include <thread>
#include <string>
#include <assert.h>
#include <stdio.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
namespace fs = std::experimental::filesystem;

namespace Utils {

  template<class T>
  FilePagedQueue<T>::FilePagedQueue(std::string directory, std::string prefix, size_t page_size)
    : m_current_read(0),
      m_last_write(0),
      m_records_paged(0),
      m_dir(directory),
      m_prefix(prefix),
      m_page_size(page_size)
  {
    assert(page_size > 0);
    assert(!directory.empty());
    assert(!prefix.empty());
    if (!fs::is_directory(directory)) {
      throw std::runtime_error("Invalid directory");
    }
    // Create an inital in-memory queue for the records
    m_head = std::make_shared<std::queue<T>>();
    // As there is initially only one queue, tail points to the same place
    m_tail = m_head;
  }

  template<class T>
  T& FilePagedQueue<T>::front()
  {
    return m_head->front();
  }

  template<class T>
  const T& FilePagedQueue<T>::front() const
  {
    return m_head->front();
  }

  // Remove the first element
  template<class T>
  void FilePagedQueue<T>::pop()
  {
    m_head->pop();
    if (m_head->empty()) {
      // If we are reading a file make sure we have finished
      sync_reader();
      
      // Set the head to the next list if there is one
      if (m_next) {
        m_head = m_next;
        // Kick thread off to load the next queue so hopefully it will
        // be ready when we need it
        if (m_current_read < m_last_write) {
          ++m_current_read;
          if (m_current_read == m_last_write) {
            // If reading the last file to be written
            // make sure it has actually finished!
            sync_writer();
          }
          m_next = std::make_shared<std::queue<T>>();
          m_reader = std::thread(&FilePagedQueue::unpage,this,m_next);
        } else if (m_head != m_tail) {
          // We have caught our own tail
          m_next = m_tail;
        } else {
          // Nowhere left to go
          m_next = nullptr;
        }
      } 
    }
  }

  // Add element to the end
  template<class T>
  void FilePagedQueue<T>::push(const T& value)
  {
    m_tail->push(value);
    if (m_tail->size() >= m_page_size) {
      // Tail has reachd page size
      // Case 1: If head and tail are the same split them into separate queues
      if (m_tail == m_head) {
        m_tail = std::make_shared<std::queue<T>>();
        assert(!m_next);
        m_next = m_tail;
      } else if (m_tail == m_next) {
        // Case 2: tail is the same as next. Split it again
        m_tail = std::make_shared<std::queue<T>>();
      } else {
        // Case 3: Page it out
        // Finish any existing write
        sync_writer();

        if (m_last_write == m_current_read && m_last_write > 0) {
          // Reset counters, but syncronise first just to be safe!
          syncronize();
          m_current_read = m_last_write = 0;
        }
        ++m_last_write;
        m_writer = std::thread(&FilePagedQueue::page,this,m_tail);
        m_records_paged += m_tail->size();
        //std::cout << "Records paged: " << m_records_paged << std::endl;
        // Start a new tail queue
        m_tail = std::make_shared<std::queue<T>>();
      }
    }
  }

  // Return if queue is empty
  template<class T>
  bool FilePagedQueue<T>::empty() const
  {
    return m_head->empty();
  }


  // Return the number of elements in the queue
  template<class T>
  size_t FilePagedQueue<T>::size() const
  {
    size_t count = m_head->size();
    if (m_tail != m_head) {
      count += m_tail->size();
    }
    if (m_next && m_next != m_tail && !m_reader.joinable())
    {
      assert(m_next != m_head);
      // There is a next queue that is not the tail that is not currently being read
      count += m_next->size();
    }
    count += m_records_paged;
    return count;
  }

  // Get the path to the pagefile to use
  template<class T>
  std::string FilePagedQueue<T>::page_file(int counter) const
  {
    fs::path dir = m_dir;
    fs::path file = m_prefix;
    file += std::to_string(counter);
    file += ".q";
    fs::path full = dir / file;
    return full.string();
  }
      
  // Write the queue to disk
  template<class T>
  void FilePagedQueue<T>::page(std::shared_ptr<std::queue<T>> queue)
  {
    assert(!queue->empty());
    assert(queue->size() == m_page_size);
    std::string file = page_file(m_last_write);
    std::ofstream out(file.c_str());
    if (!out.good()) {
      std::string message("Error opening queue file to write: ");
      throw std::runtime_error(message + file);
    }
    write_to_stream(out,*queue);
    out.close();
  }

  // Read the queue from disk
  template<class T>
  void FilePagedQueue<T>::unpage(std::shared_ptr<std::queue<T>> queue)
  {
    assert(queue->empty());
    std::string file = page_file(m_current_read);
    std::ifstream input(file.c_str());
    if (!input.good()) {
      std::string message("Error opening queue file to read: ");
      throw std::runtime_error(message + file);
    }
    read_from_stream(input,*queue);
    input.close();
    assert(queue->size() == m_page_size);
    // Page file no longer needed
    remove(file.c_str());
  }

  // Serialise a queue
  template<class T>
  void FilePagedQueue<T>::write_to_stream(std::ostream& os, std::queue<T>& queue) {
    while (!queue.empty()) {
      os << queue.front() << std::endl;
      queue.pop();
    }
  }

  // Deserialise a queue
  // Reads all items or only size items if it is set
  template<class T>
  void FilePagedQueue<T>::read_from_stream(std::istream& is, std::queue<T>& queue, size_t size)
  {
    T item;
    int read = 0;
    while (is >> item)
    {
      queue.push(item);
      if (++read == size) {
        break;
      }
    }
  }

  //----- Specialisation for std::string
  // Quote spaces and special characters so they can be safely serialised / deserialised
  template<>
  void FilePagedQueue<std::string>::write_to_stream(std::ostream& os, std::queue<std::string>& queue) {
    while (!queue.empty()) {
      os << std::quoted(queue.front()) << std::endl;
      queue.pop();
    }
  }

  // Read the queue from disk
  template<>
  void FilePagedQueue<std::string>::read_from_stream(std::istream& is, std::queue<std::string>& queue, size_t size)
  {
    std::string item;
    int read = 0;
    while (is >> std::quoted(item))
    {
      queue.push(item);
      if (++read == size) {
        break;
      }
    }
  }
  //------

  // Syncronise with reader
  template<class T>
  void FilePagedQueue<T>::sync_reader()
  {
    if (m_reader.joinable()) {
      m_reader.join();
      assert(m_next);
      m_records_paged -= m_next->size();
    }
  }

  // Syncronise with writer
  template<class T>
  void FilePagedQueue<T>::sync_writer()
  {
    if (m_writer.joinable()) {
      m_writer.join();
    }
  }

  template<class T>
  void FilePagedQueue<T>::syncronize()
  {
    sync_writer();
    sync_reader();
  }

  template<class T>
  FilePagedQueue<T>::~FilePagedQueue()
  {
    // Make sure threads finish
    syncronize();
  }

}

#pragma once
#include <queue>
#include <thread>
#include <string>

namespace Utils {

  template<class T>
  class FilePagedQueue {
    public:
      // Constructor
      FilePagedQueue(std::string directory, std::string prefix, size_t page_size);

      // Access the first element
      T& front();
      const T& front() const;

      // Access the last element
      T& back();
      const T& back() const;

      // Remove the first element
      void pop_front();

      // Add element to the end
      void push_back(const T&);

      // Return if queue is empty
      bool empty() const;

      // Return the number of elements in the queue
      size_t size() const;

    protected:
    private:
      // Get the path to the pagefile to use
      std::string page_file(int counter) const;
      // Write the queue to disk
      void page(std::shared_ptr<std::queue<T>> queue);
      // Read the queue from disk
      void unpage(std::shared_ptr<std::queue<T>> queue);

      std::string m_dir;
      std::string m_prefix;
      size_t m_page_size;
      std::shared_ptr<std::queue<T>> m_head;
      std::shared_ptr<std::queue<T>> m_next;
      std::shared_ptr<std::queue<T>> m_tail; 
      size_t m_records_paged;
      int m_current_read;
      int m_last_write;
      std::thread m_reader;
      std::thread m_writer;
  };

}

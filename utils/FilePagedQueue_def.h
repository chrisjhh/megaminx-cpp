#pragma once
#include <queue>
#include <thread>
#include <string>
#include <iosfwd>

namespace Utils {

  template<class T>
  class FilePagedQueue {
    public:
      // Constructor
      FilePagedQueue(std::string directory, std::string prefix, size_t page_size);

      // Access the first element
      T& front();
      const T& front() const;

      // Remove the first element
      void pop();

      // Add element to the end
      void push(const T&);

      // Return if queue is empty
      bool empty() const;

      // Return the number of elements in the queue
      size_t size() const;

      // Allow all threads to catch up
      // Should not be needed for normal use
      // But useful for testing
      void syncronize();

      ~FilePagedQueue();

    protected:
    private:
      // Syncronise with reader
      void sync_reader();
      // Syncronise with writer
      void sync_writer();
      // Get the path to the pagefile to use
      std::string page_file(int counter) const;
      // Write the queue to disk
      void page(std::shared_ptr<std::queue<T>> queue);
      // Read the queue from disk
      void unpage(std::shared_ptr<std::queue<T>> queue);
      // Serialise a queue
      void write_to_stream(std::ostream& os, std::queue<T>& queue);
      // Deserialise a queue
      // Reads all items or only size items if it is set
      void read_from_stream(std::istream& is, std::queue<T>& queue, int size=0);

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

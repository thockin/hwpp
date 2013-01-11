// Tim Hockin <thockin@hockin.org>
// Similar to ostringstream, but for stdio FILEs.

#ifndef HWPP_UTIL_STRING_FILE_H__
#define HWPP_UTIL_STRING_FILE_H__

#include "pipe_file.h"

namespace util {

// This is a simple helper class for allowing code to read a provided string
// from a FILE. In many respects it is very similar to std::ostringstream.
class StringFile {
 public:
  // Don't do any work that might fail in the ctor.
  StringFile() : m_pipe_file() {
  }

  // Clean up.
  ~StringFile() {
  }

  // Gets the read side FILE*.
  FILE *get_read_file() const {
    return m_pipe_file.get_read_file();
  }

  // Gets the entire contents of the file as a string.
  string get_contents() const {
    char buf[1024];
    string result;
    while (fgets(buf, sizeof(buf), m_pipe_file.get_read_file()) != NULL) {
      result += buf;
    }
    return result;
  }

  // Opens (or re-opens) the file for reading.
  bool open(const string &str) {
    if (!m_pipe_file.open()) {
      return false;
    }
    FILE *f = m_pipe_file.get_write_file();
    fputs(str.c_str(), f);          // put the data into the pipe
    m_pipe_file.close_write_file(); // force an EOF when the data is consumed
    return true;
  }

 private:
  PipeFile m_pipe_file;
  string m_string;
};

}  // namespace util

#endif  // HWPP_UTIL_STRING_FILE_H__

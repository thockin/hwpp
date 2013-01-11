// Tim Hockin <thockin@hockin.org>
// Similar to ostringstream, but for stdio FILEs.

#ifndef HWPP_UTIL_PIPE_FILE_H__
#define HWPP_UTIL_PIPE_FILE_H__

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

namespace util {

// This is a simple helper class for allowing code to write to a FILE and
// read it back as a string. In many respects it is very similar to
// std::ostringstream.
class PipeFile {
 public:
	// Don't do any work that might fail in the ctor.
	PipeFile() : m_read_file(NULL), m_write_file(NULL) {
	}

	// Clean up.
	~PipeFile() {
		close_read_file();
		close_write_file();
	}

	// Gets the read side FILE* of the pipe.
	FILE *get_read_file() const {
		return m_read_file;
	}

	// Closes the read end of the pipe.
	void close_read_file() {
		if (m_read_file) {
			fclose(m_read_file);
			m_read_file = NULL;
		}
	}

	// Gets the write side FILE* of the pipe.
	FILE *get_write_file() const {
		return m_write_file;
	}

	// Closes the write end of the pipe.
	void close_write_file() {
		if (m_write_file) {
			fclose(m_write_file);
			m_write_file = NULL;
		}
	}

	// Gets the entire contents of the pipe as a string.
	string get_contents() const {
		char buf[1024];
		string result;
		while (fgets(buf, sizeof(buf), m_read_file) != NULL) {
			result += buf;
		}
		return result;
	}

	// Flushes the pipe.
	void flush() const {
		get_contents();
	}

	// Opens the pipe for reading and writing.
	bool open() {
		int pipe_fds[2];
		FILE *read_file;
		FILE *write_file;

		if (pipe(pipe_fds) != 0) {
			return false;
		}
		fcntl(pipe_fds[0], F_SETFL, O_NONBLOCK);
		read_file = fdopen(pipe_fds[0], "r");
		if (read_file == NULL) {
			close(pipe_fds[0]);
			close(pipe_fds[1]);
			return false;
		}
		write_file = fdopen(pipe_fds[1], "w");
		if (write_file == NULL) {
			fclose(read_file);
			close(pipe_fds[1]);
			return false;
		}
		if (setvbuf(read_file, NULL, _IONBF, 0) != 0) {
			fclose(read_file);
			fclose(write_file);
			return false;
		}
		if (setvbuf(write_file, NULL, _IONBF, 0) != 0) {
			fclose(read_file);
			fclose(write_file);
			return false;
		}
		if (m_read_file) {
			fclose(m_read_file);
		}
		m_read_file = read_file;
		if (m_write_file) {
			fclose(m_write_file);
		}
		m_write_file = write_file;
		return true;
	}

 private:
	FILE *m_read_file;
	FILE *m_write_file;
};

}  // namespace util

#endif  // HWPP_UTIL_PIPE_FILE_H__

/* Copyright (c) Andrew Schran, 2008 */
#ifndef PP_SOCKETS_H__
#define PP_SOCKETS_H__

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string>
#include "sys_error.h"

namespace unix_socket {

class socket {
    private:
	int m_fd;
	struct sockaddr_un m_remote;

    public:
	socket(int fd)
	{
		m_fd = fd;
	}

	socket(const std::string &path)
	{
		int ret;

		// Make socket
		m_fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
		if (m_fd < 0) {
			throw sys_error(errno,
				"unix_socket::socket::socket(" + path + ")");
		}

		m_remote.sun_family = AF_UNIX;
		strncpy(m_remote.sun_path, path.c_str(),
				sizeof(m_remote.sun_path));
		ret = connect(m_fd, (struct sockaddr *)&m_remote,
			path.size() + sizeof(m_remote.sun_family));
		if (ret < 0) {
			throw sys_error(errno,
				"unix_socket::socket::socket(" + path + ")");
		}
	}

	~socket()
	{
		close();
	}

	bool
	is_connected() const
	{
		return (m_fd >= 0);
	}

	virtual void
	close()
	{
		if (is_connected()) {
			::close(m_fd);
			m_fd = -1;
		}
	}

	void
	send(std::string s) const
	{
		send(s.data(), s.length());
	}

	void
	send(const void *msg, int len) const
	{
		if (!is_connected()) {
			throw sys_error(ENOTCONN,
				"unix_socket::socket::send");
		}

		const char *buf = (char *)msg;
		int sent = 0;
		int left = len;

		while (sent < len) {
			int n = ::send(m_fd, buf + sent, left, 0);
			if (n == -1) {
				// FIXME: check for EINTR
				throw sys_error(errno,
					"unix_socket::socket::send");
			}
			sent += n;
			left -= n;
		}
	}

	// Receive and return one line of data
	// Discard the newline character
	std::string
	recv_line()
	{
		if (!is_connected()) {
			throw sys_error(ENOTCONN,
				"unix_socket::socket::recv_line");
		}
		
		char c;
		ssize_t len;
		std::string str;
		while ((len = ::recv(m_fd, &c, sizeof(c), 0)) > 0) {
			if (c == '\n')
				break;
			str.push_back(c);
		}

		if (len <= 0) {
			close();
		}

		return str;
	}

	// Attempt to receive exactly len bytes
	// Return number of bytes actually received
	ssize_t
	recv_all(void *buf, size_t len)
	{
		// Attempt to receive len bytes, if possible
		size_t total = 0;
		do {
			ssize_t rcvd = recv((char *)buf + total,
					len - total);
			if (rcvd <= 0) {
				// Connection closed, return whatever
				// we have
				return total;
			}
			total += rcvd;
		} while (total < len);

		return total;
	}

	// Receive up to len bytes
	// Return number of bytes actually received
	ssize_t
	recv(void *buf, size_t len) {
		if (!is_connected()) {
			throw sys_error(ENOTCONN,
				"unix_socket::socket::recv");
		}

		ssize_t ret = ::recv(m_fd, buf, len, 0);
		if (ret <= 0 && len > 0) {
			// FIXME: check for EINTR
			close();
		}

		return ret;
	}

}; // class unix_socket

class server
{
    private:
	int m_fd_listen;
	std::string m_path;
	struct sockaddr_un m_local, m_remote;
	struct ucred m_cred;

    public:
	server(const std::string &path)
	    : m_path(path)
	{
		int ret;

		// Make socket
		m_fd_listen = ::socket(AF_UNIX, SOCK_STREAM, 0);
		if (m_fd_listen < 0) {
			throw sys_error(errno,
				"unix_socket::server::server(" + path + ")");
		}

		// Bind to path
		m_local.sun_family = AF_UNIX;
		strncpy(m_local.sun_path, path.c_str(), path.size() + 1);
		// FIXME: should we unlink or not?
		::unlink(m_local.sun_path);
		socklen_t len = path.length()
				+ sizeof(m_local.sun_family);
		ret = bind(m_fd_listen, (struct sockaddr *)&m_local, len);
		if (ret < 0) {
			throw sys_error(errno,
				"unix_socket::server::server(" + path + ")");
		}

		// Listen for connections
		ret = listen(m_fd_listen, 10); // allow 10 to queue
		if (ret < 0) {
			throw sys_error(errno,
				"unix_socket::server::server(" + path + ")");
		}
	} // server

	~server()
	{
		::close(m_fd_listen);
		::unlink(m_path.c_str());
	}

	socket
	accept()
	{
		while (1) {
			socklen_t len = sizeof(struct sockaddr_un);
			int s = ::accept(m_fd_listen,
				     (struct sockaddr *)&m_remote, &len);
			if (s < 0) {
				s = 0;
				if (errno == EINTR) {
					continue; // signal
				}
				throw sys_error(errno,
					"unix_socket::server::accept()");
			}

			return socket(s);
		}
	} // accept

}; // class server

} // namespace unix_socket

#endif

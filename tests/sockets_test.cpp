#include "sockets.h"
#include "pp_test.h"

#define UNIX_SOCKET_NAME		"socket.exists"
#define UNIX_SOCKET_PATH		TEST_TMP_DIR "/" UNIX_SOCKET_NAME

TEST(test_ctors)
{
	// test server constructor
	try {
		unix_socket::server svr(UNIX_SOCKET_PATH);
	} catch (std::exception &e) { //FIXME: catch specific exceptions
		TEST_ERROR("unix_socket::server()");
	}
	// test again for proper unlinking
	try {
		unix_socket::server svr(UNIX_SOCKET_PATH);
	} catch (std::exception &e) {
		TEST_ERROR("unix_socket::server()");
	}
	// test socket constructor
	try {
		unix_socket::socket c(UNIX_SOCKET_PATH);
		TEST_ERROR("unix_socket::socket()");
	} catch (std::exception &e) {
	}
	try {
		unix_socket::socket c(UNIX_SOCKET_PATH ".not");
		TEST_ERROR("unix_socket::socket()");
	} catch (std::exception &e) {
	}
	try {
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
	} catch (std::exception &e) {
		TEST_ERROR("unix_socket::server() and unix_socket::socket()");
	}
	{
		{
			unix_socket::server svr(UNIX_SOCKET_PATH);
			unix_socket::socket c(UNIX_SOCKET_PATH);
		}
		try {
			unix_socket::socket c2(UNIX_SOCKET_PATH);
			TEST_ERROR("unix_socket::server()");
		} catch (std::exception &e) {
		}
	}
}

TEST(test_connection)
{
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		TEST_ASSERT(s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.close();
		TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.close();
		TEST_ASSERT(s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.close();
		TEST_ASSERT(s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.close();
		c.close();
		TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.close();
		c.recv_line();
		TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.close();
		s.recv_line();
		TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
}

TEST(test_send_recv)
{
	// Tests in this section are all done twice, once with server
	// sending and client receiving, and once with client sending
	// and server receiving
	
	// Test recv_line()
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("This is a test message\n");
		string str = s.recv_line();
		if (str != "This is a test message") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_line(): received \""
				<< str << "\"";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("This is a test message\n");
		string str = c.recv_line();
		if (str != "This is a test message") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_line(): received \""
				<< str << "\"";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("Testing recv_line return on connection close");
		c.close();
		string str = s.recv_line();
		if (str != "Testing recv_line return on connection close") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_line(): received \""
				<< str << "\"";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("Testing recv_line return on connection close");
		s.close();
		string str = c.recv_line();
		if (str != "Testing recv_line return on connection "
				     "close") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_line(): received \""
				<< str << "\"";
		}
	}

	// Test recv_all()
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("12345678901234567890");
		char buf[3];
		s.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "123") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received \""
				<< str << "\"";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("12345678901234567890");
		char buf[3];
		c.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "123") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received \""
				<< str << "\"";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("12345678901234567890");
		c.close();
		char buf[25];
		int i = s.recv_all(buf, 25);
		if (i != 20) {
			// This should stop trying to receive on remote close
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received "
				<< i << " bytes";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("12345678901234567890");
		s.close();
		char buf[25];
		int i = c.recv_all(buf, 25);
		if (i != 20) {
			// This should stop trying to receive on remote close
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received "
				<< i << " bytes";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("1234567890");
		char buf[3];
		int i = s.recv_all(buf, 3);
		if (i != 3) {
			// Should fill the buffer to capacity
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received "
				<< i << " bytes";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("1234567890");
		char buf[3];
		int i = c.recv_all(buf, 3);
		if (i != 3) {
			// Should fill the buffer to capacity
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received "
				<< i << " bytes";
		}
	}
	// Test receives after remote connection is closed
	// with incoming data still buffered
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("1234567890");
		c.close();
		char buf[3];
		s.recv_all(buf, 3);
		s.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "456") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received \""
				<< str << "\"";
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("1234567890");
		s.close();
		char buf[3];
		c.recv_all(buf, 3);
		c.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "456") {
			TEST_ERROR() << "unix_socket::send() and "
				<< "unix_socket::recv_all(): received \""
				<< str << "\"";
		}
	}
	// Test receiving 0 bytes; connection should still stay open
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		c.send("1234567890");
		char buf[1];
		s.recv_all(buf, 0);
		if (!s.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
		}
		if (!c.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
		}
	}
	{
		unix_socket::server svr(UNIX_SOCKET_PATH);
		unix_socket::socket c(UNIX_SOCKET_PATH);
		unix_socket::socket s = svr.accept();
		s.send("1234567890");
		char buf[1];
		c.recv_all(buf, 0);
		if (!s.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
		}
		if (!c.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
		}
	}
}

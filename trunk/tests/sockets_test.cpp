#include "pp_test.h"
#include "sockets.h"

int
test_setup()
{
	system("mkdir -p test_data");
	return 0;
}

int
test_ctors()
{
	int ret = 0;

	// test server constructor
	try {
		unix_socket::server svr("test_data/server.socket");
	} catch (std::exception &e) {
		TEST_ERROR("unix_socket::server()");
		ret++;
	}
	// test again for proper unlinking
	try {
		unix_socket::server svr("test_data/server.socket");
	} catch (std::exception &e) {
		TEST_ERROR("unix_socket::server()");
		ret++;
	}
	// test socket constructor
	try {
		unix_socket::socket c("test_data/server.socket");
		TEST_ERROR("unix_socket::socket()");
		ret++;
	} catch (std::exception &e) {
	}
	try {
		unix_socket::socket c("test_data/no_socket_here");
		TEST_ERROR("unix_socket::socket()");
		ret++;
	} catch (std::exception &e) {
	}
	try {
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
	} catch (std::exception &e) {
		TEST_ERROR("unix_socket::server() and unix_socket::socket()");
		ret++;
	}
	{
		{
			unix_socket::server svr("test_data/server.socket");
			unix_socket::socket c("test_data/server.socket");
		}
		try {
			unix_socket::socket c2("test_data/server.socket");
			TEST_ERROR("unix_socket::server()");
		} catch (std::exception &e) {
		}
	}
	return ret;
}

int
test_connection()
{
	int ret = 0;
	
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		ret += TEST_ASSERT(s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.close();
		ret += TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.close();
		ret += TEST_ASSERT(s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.close();
		ret += TEST_ASSERT(s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.close();
		c.close();
		ret += TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.close();
		c.recv_line();
		ret += TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.close();
		s.recv_line();
		ret += TEST_ASSERT(!s.is_connected(),
				"unix_socket::is_connected()");
		ret += TEST_ASSERT(!c.is_connected(),
				"unix_socket::is_connected()");
	}

	return ret;
}

int
test_send_recv()
{
	int ret = 0;

	// Tests in this section are all done twice, once with server
	// sending and client receiving, and once with client sending
	// and server receiving
	
	// Test recv_line()
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("This is a test message\n");
		if (s.recv_line() != "This is a test message") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_line()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("This is a test message\n");
		if (c.recv_line() != "This is a test message") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_line()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("Testing recv_line return on connection close");
		c.close();
		if (s.recv_line() != "Testing recv_line return on connection "
				     "close") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_line()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("Testing recv_line return on connection close");
		s.close();
		if (c.recv_line() != "Testing recv_line return on connection "
				     "close") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_line()");
			ret++;
		}
	}

	// Test recv_all()
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("12345678901234567890");
		char buf[3];
		s.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "123") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("12345678901234567890");
		char buf[3];
		c.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "123") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("12345678901234567890");
		c.close();
		char buf[25];
		int i = s.recv_all(buf, 25);
		if (i != 20) {
			// This should stop trying to receive on remote close
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("12345678901234567890");
		s.close();
		char buf[25];
		int i = c.recv_all(buf, 25);
		if (i != 20) {
			// This should stop trying to receive on remote close
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("1234567890");
		char buf[3];
		int i = s.recv_all(buf, 3);
		if (i != 3) {
			// Should fill the buffer to capacity
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("1234567890");
		char buf[3];
		int i = c.recv_all(buf, 3);
		if (i != 3) {
			// Should fill the buffer to capacity
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	// Test receives after remote connection is closed
	// with incoming data still buffered
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("1234567890");
		c.close();
		char buf[3];
		s.recv_all(buf, 3);
		s.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "456") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("1234567890");
		s.close();
		char buf[3];
		c.recv_all(buf, 3);
		c.recv_all(buf, 3);
		string str(buf, 3);
		if (str != "456") {
			TEST_ERROR("unix_socket::send() and "
				   "unix_socket::recv_all()");
			ret++;
		}
	}
	// Test receiving 0 bytes; connection should still stay open
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		c.send("1234567890");
		char buf[1];
		s.recv_all(buf, 0);
		if (!s.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
			ret++;
		}
		if (!c.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
			ret++;
		}
	}
	{
		unix_socket::server svr("test_data/server.socket");
		unix_socket::socket c("test_data/server.socket");
		unix_socket::socket s = svr.accept();
		s.send("1234567890");
		char buf[1];
		c.recv_all(buf, 0);
		if (!s.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
			ret++;
		}
		if (!c.is_connected()) {
			TEST_ERROR("unix_socket::recv_all()");
			ret++;
		}
	}

	return ret;
}

int
test_cleanup()
{
	system("rm -rf test_data");
	return 0;
}

TEST_LIST(
	TEST(test_setup), // keep this first
	TEST(test_ctors),
	TEST(test_connection),
	TEST(test_send_recv),
	TEST(test_cleanup), // keep this last
);

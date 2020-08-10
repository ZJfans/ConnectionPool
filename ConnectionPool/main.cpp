#include <iostream>
using namespace std;
#include "Connection.h"
#include "CommonConnectionPool.h"

int main() {

	Connection conn;
	conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");

	clock_t begin = clock();
	/* //ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 10000; ++i) {
		//shared_ptr<Connection> sp = cp->Getconnect();
		Connection conn;
		char sql[1024] = {0};
		sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',23,'man')");
		conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");
		conn.update(sql);
		//sp->update(sql);
	}*/

	thread t1([]() {
		/*for (int i = 0; i < 2000; ++i) {
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',21,'man')");
			conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}*/
		
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 2000; ++i) {
			shared_ptr<Connection> sp = cp->Getconnect();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',21,'man')");
			sp->update(sql);
		}
		});
	thread t2([]() {
		/*for (int i = 0; i < 2000; ++i) {
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',21,'man')");
			conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}*/
		
		ConnectionPool* cp = ConnectionPool::getConnectionPool();

		for (int i = 0; i < 2000; ++i) {
			shared_ptr<Connection> sp = cp->Getconnect();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',22,'man')");
			sp->update(sql);
		}
		});
	thread t3([]() {
		/*for (int i = 0; i < 2000; ++i) {
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',21,'man')");
			conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}*/
		
		ConnectionPool* cp = ConnectionPool::getConnectionPool();

		for (int i = 0; i < 2000; ++i) {
			shared_ptr<Connection> sp = cp->Getconnect();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',23,'man')");
			sp->update(sql);
		}
		});
	thread t4([]() {
		/*for (int i = 0; i < 2000; ++i) {
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',21,'man')");
			conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}*/
		
		ConnectionPool* cp = ConnectionPool::getConnectionPool();

		for (int i = 0; i < 2000; ++i) {
			shared_ptr<Connection> sp = cp->Getconnect();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',24,'man')");
			sp->update(sql);
		}
		});
	thread t5([]() {
		/*for (int i = 0; i < 2000; ++i) {
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',21,'man')");
			conn.Connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}*/
		
		ConnectionPool* cp = ConnectionPool::getConnectionPool();

		for (int i = 0; i < 2000; ++i) {
			shared_ptr<Connection> sp = cp->Getconnect();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('zhangsanfeng',25,'man')");
			sp->update(sql);
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	clock_t end = clock();

	cout << (end - begin)<<"ms" << endl;
	return 0;
}

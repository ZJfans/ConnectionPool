#include<string>
#include<queue>
#include<mutex>
#pragma once
#include<iostream>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
#include"Connection.h"
using namespace std;

class ConnectionPool {
public:
	//单例模式
	static ConnectionPool* getConnectionPool();

	shared_ptr<Connection>Getconnect();

private:
	ConnectionPool ();
	
	bool loadConfigFile();
	void produceConnect();
	void scannerConnectionTask();
	string  _ip;
	unsigned short _port;
	string _username;
	string _password;
	string _dbname;

	int initSize;
	int maxSize;
	int maxIdleTime;
	int connectionTimeOut;

	queue<Connection*>_connectQue;
	mutex _queMutex;
	atomic_int _connectCount;//记录连接总数量，原子性
	condition_variable cv; //设置条件变量，用于连接生产者和消费者的连接
};
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
	//����ģʽ
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
	atomic_int _connectCount;//��¼������������ԭ����
	condition_variable cv; //���������������������������ߺ������ߵ�����
};
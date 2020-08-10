#pragma once
#include<iostream>
#include<mysql.h>
#include<string>
#include<ctime>
using namespace std;
class Connection {

public:
	//初始化数据库连接
	Connection();
	//释放数据库连接资源
	~Connection();
	//连接数据库
	bool Connect(string ip, 
		unsigned short port,
		string username,
		string password,
		string dbname);
	//更新操作
	bool update(string sql);
	//查询操作
	MYSQL_RES* query(string str);
	void refreshAliceTime() { _alivetime = clock(); }
	clock_t getAlivetime() const  { return clock() - _alivetime; }
private:
	MYSQL* _conn;
	clock_t _alivetime;
};
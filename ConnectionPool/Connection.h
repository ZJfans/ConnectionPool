#pragma once
#include<iostream>
#include<mysql.h>
#include<string>
#include<ctime>
using namespace std;
class Connection {

public:
	//��ʼ�����ݿ�����
	Connection();
	//�ͷ����ݿ�������Դ
	~Connection();
	//�������ݿ�
	bool Connect(string ip, 
		unsigned short port,
		string username,
		string password,
		string dbname);
	//���²���
	bool update(string sql);
	//��ѯ����
	MYSQL_RES* query(string str);
	void refreshAliceTime() { _alivetime = clock(); }
	clock_t getAlivetime() const  { return clock() - _alivetime; }
private:
	MYSQL* _conn;
	clock_t _alivetime;
};
#include "CommonConnectionPool.h"
#include"public.h"

//�����Ļ�ȡһ��Ψһpoolʵ��
 ConnectionPool* ConnectionPool::getConnectionPool() {
	 static ConnectionPool pool;
	 return &pool;
}
 //1�Ź��˽�����
 bool ConnectionPool::loadConfigFile() {
	 FILE* pf = fopen("mysql.ini", "r");
	 if (pf == nullptr) {
		 LOG("mysql.ini file is not exist!");
		 return false;
	 }
	 while (!feof(pf)) {
		 char line[1024] = { 0 };
		 fgets(line, 1024, pf);
		 string str = line;
		 int idx = str.find('=', 0);//�ҵ������±�
		 if (idx == -1) {
			 continue;
		 }
		 int endidx = str.find('\n', idx);//' '���±�
		 string key = str.substr(0, idx);
		 string value = str.substr(idx + 1, endidx - idx - 1);
 		 if (key == "ip") {
			 _ip = value;
		 }
		 else if (key == "port") {
			 _port = atoi(value.c_str());
		 }
		 else if (key == "username") {
			 _username = value;
		 }
		 else if (key == "password") {
			 _password = value;
		 }
		 else if (key == "dbname") {
			 _dbname = value;
		 }
		 else if (key == "initSize") {
			 initSize = atoi(value.c_str());
		 }
		 else if (key == "maxSize") {
			 maxSize = atoi(value.c_str());
		 }
		 else if (key == "maxIdleTime") {
			 maxIdleTime = atoi(value.c_str());
		 }
		 else if (key == "connectionTimeOut") {
			 connectionTimeOut = atoi(value.c_str());
		 }
	 }
	 return true;
 }
 //�ϰ�����ʼ��
 ConnectionPool::ConnectionPool() {
	 if (!loadConfigFile()) {
		 return;
	 }

	 for (int i = 0;i < initSize; ++i) {
		 Connection* p = new Connection();
		 p->Connect(_ip, _port, _username, _password, _dbname);
		 p->refreshAliceTime();
		 _connectQue.push(p);
		 _connectCount++;
	 }
	 thread produce(std::bind(&ConnectionPool::produceConnect,this));
	 produce.detach();
	 //���ճ�ʱ������
	 thread scan(std::bind(&ConnectionPool::scannerConnectionTask,this));
	 scan.detach();
 }
 //4�Ź��˹ܻ���
 void ConnectionPool::scannerConnectionTask() {
	 for (;;) {
		 this_thread::sleep_for(chrono::seconds(maxIdleTime));

		 unique_lock<mutex>lock(_queMutex);
		 while (_connectCount > initSize) {
			 Connection* p = _connectQue.front();
			 if (p->getAlivetime() > (maxIdleTime * 1000)) {//�ж϶�ͷ��û�г���ʱ��
				 _connectQue.pop();
				 _connectCount--;
				 delete p;
			 }
			 else {
				 break;
			 }
		 }
	 }
 }
 //2�Ź��˹�����
 void ConnectionPool::produceConnect() {
	 for (;;) {
		 unique_lock<mutex>lock(_queMutex);
		 if (!_connectQue.empty()) {
			 cv.wait(lock);		//���в��գ������ȴ�
		 }
		 if (_connectCount < maxSize) {
			 Connection* p = new Connection();
			 p->Connect(_ip, _port, _username, _password, _dbname);
			 p->refreshAliceTime();
			 _connectQue.push(p);
			 _connectCount++;
		 }
		 cv.notify_all();//֪ͨ������������
	 }
 }
 //3�Ź��˹�����
 shared_ptr<Connection> ConnectionPool::Getconnect() {
	 //��һ�����ж϶����Ƿ�Ϊ�գ������ã�û��ȴ�
	 //�õ����ӣ�ʹ�ã�Ȼ���Զ����������������ӷŻص�������
	 unique_lock<mutex>lock(_queMutex);
	 while (_connectQue.empty()) {
		 if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(connectionTimeOut))) {					//�ڴ�ʱ�����о��ߣ�û�о͵ȴ��������ô��ʱ��û�У�Ȼ���ȡʧ��
			 if (_connectQue.empty()) {
				 LOG("��ȡ���ӳ�ʱ����ȡʧ�ܣ�����");
				 return nullptr;
			 }
		 }
	 }
	 shared_ptr<Connection> sp(_connectQue.front(),
		 [&](Connection* pcon) {
			 //�������߳��µ���Դ��Ҫ��֤�̰߳�ȫ
			 unique_lock<mutex>lock(_queMutex);
			 pcon->refreshAliceTime();
			 _connectQue.push(pcon);
		 });//����ʱ���ͷ����ӣ�����Ҫ�Զ�������
	 _connectQue.pop();
	 //�����꣬������б�Ϊ�գ���֪ͨ����������
	 if (_connectQue.empty()) {
		 cv.notify_all();
	 }
	 return sp;
 }







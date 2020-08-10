#include "CommonConnectionPool.h"
#include"public.h"

//单例的获取一个唯一pool实例
 ConnectionPool* ConnectionPool::getConnectionPool() {
	 static ConnectionPool pool;
	 return &pool;
}
 //1号工人接任务
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
		 int idx = str.find('=', 0);//找到返回下标
		 if (idx == -1) {
			 continue;
		 }
		 int endidx = str.find('\n', idx);//' '的下标
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
 //老板管理初始化
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
	 //回收超时的连接
	 thread scan(std::bind(&ConnectionPool::scannerConnectionTask,this));
	 scan.detach();
 }
 //4号工人管回收
 void ConnectionPool::scannerConnectionTask() {
	 for (;;) {
		 this_thread::sleep_for(chrono::seconds(maxIdleTime));

		 unique_lock<mutex>lock(_queMutex);
		 while (_connectCount > initSize) {
			 Connection* p = _connectQue.front();
			 if (p->getAlivetime() > (maxIdleTime * 1000)) {//判断队头有没有超过时间
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
 //2号工人管生产
 void ConnectionPool::produceConnect() {
	 for (;;) {
		 unique_lock<mutex>lock(_queMutex);
		 if (!_connectQue.empty()) {
			 cv.wait(lock);		//队列不空，生产等待
		 }
		 if (_connectCount < maxSize) {
			 Connection* p = new Connection();
			 p->Connect(_ip, _port, _username, _password, _dbname);
			 p->refreshAliceTime();
			 _connectQue.push(p);
			 _connectCount++;
		 }
		 cv.notify_all();//通知消费者消费了
	 }
 }
 //3号工人管消费
 shared_ptr<Connection> ConnectionPool::Getconnect() {
	 //第一步：判断队列是否为空，有则拿，没则等待
	 //拿到连接，使用，然后自定义析构，即把连接放回到队列中
	 unique_lock<mutex>lock(_queMutex);
	 while (_connectQue.empty()) {
		 if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(connectionTimeOut))) {					//在此时间内有就走，没有就等待，如果这么多时间没有，然后获取失败
			 if (_connectQue.empty()) {
				 LOG("获取连接超时，获取失败！！！");
				 return nullptr;
			 }
		 }
	 }
	 shared_ptr<Connection> sp(_connectQue.front(),
		 [&](Connection* pcon) {
			 //操作多线程下的资源，要保证线程安全
			 unique_lock<mutex>lock(_queMutex);
			 pcon->refreshAliceTime();
			 _connectQue.push(pcon);
		 });//析构时会释放连接，所以要自定义析构
	 _connectQue.pop();
	 //消费完，如果队列变为空，就通知生产者生产
	 if (_connectQue.empty()) {
		 cv.notify_all();
	 }
	 return sp;
 }







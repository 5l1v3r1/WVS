#pragma once
#ifndef THREAD_STATUS
#define THREAD_STATUS
typedef enum ThreadStatus{
	WAITTING,
	BUSY
}TheadStatus;
#endif

#include "CHttpClient.h"

class CMyThreadPool;
class CMyWorkerThread;
class CJob
{
protected:
	CMyWorkerThread * m_pWorkThread;        //在线程里面设置
public:
	CJob();
	virtual ~CJob();

	//获取或设置工作线程  
	CMyWorkerThread * GetWorkThread()
	{
		return m_pWorkThread;
	}
	void SetWorkThread(CMyWorkerThread * pWorkThread)
	{
		m_pWorkThread = pWorkThread;
	}

	//指定Run接口  
	virtual void Run(void *ptr) = 0;
	bool m_isDeleteAfterDone;
};

class CMyWorkerThread
{
protected:
	static unsigned int __stdcall ThreadFunction(void* args);
public:
	CMyWorkerThread(CMyThreadPool* m_pThreadPool);
	~CMyWorkerThread();
	void Run(void);

	ThreadStatus getStatus(){
		return m_status;
	}

	//为了网络添加的属性add_net
	CHttpClient* m_pHttpClient;
	CURLcode m_curlCode;
	string m_strHeader = "";
	CMyThreadPool* m_pThreadPool;
	unsigned long getThreadID(){ return m_ThreadID; }
private:
	CJob* m_Job;
	void* m_JobData;
	unsigned long m_ThreadID;
	TheadStatus m_status;
};


class CMyThreadPool
{
public:

	CMyThreadPool(int initWorkerNum = 4);
	~CMyThreadPool();
	void addJob(CJob *pJob, void* jobData);
	CJob* getJob(void* &pjobData);
	int getBusyWorkerNum();
	int getRestJobNum();
	CRITICAL_SECTION m_jobQueueCS;
	CONDITION_VARIABLE m_jobCond;
	volatile int status;	//线程运行状态
private:
	queue<CJob*> jobQueue;	//任务队列；
	queue<void*> jobDataQueue;//任务参数队列;
	

	vector<CMyWorkerThread*> workerVec;
	
};


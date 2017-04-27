#pragma once
#ifndef THREAD_STATUS
#define THREAD_STATUS
typedef enum ThreadStatus{
	WAITTING,
	BUSY
}TheadStatus;
#endif
#ifndef THREAD_POOL_STATUS
#define THREAD_POOL_STATUS
typedef enum ThreadPoolStatus{
	pause,
	work,
	stop
}ThreadPoolStatus;
#endif

#include "CHttpClient.h"

class CMyThreadPool;
class CMyWorkerThread;
class CJob
{
protected:
	CMyWorkerThread * m_pWorkThread;        //���߳���������
public:
	CJob();
	virtual ~CJob();

	//��ȡ�����ù����߳�  
	CMyWorkerThread * GetWorkThread()
	{
		return m_pWorkThread;
	}
	void SetWorkThread(CMyWorkerThread * pWorkThread)
	{
		m_pWorkThread = pWorkThread;
	}

	//ָ��Run�ӿ�  
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

	//Ϊ��������ӵ�����add_net
	
	CURLcode m_curlCode;
	string m_strHeader = "";
	CMyThreadPool* m_pThreadPool;
	unsigned long getThreadID(){ return m_ThreadID; }
	CHttpClient* getHttpClient();
private:
	CJob* m_Job;
	void* m_JobData;
	unsigned long m_ThreadID;
	TheadStatus m_status;	
	CHttpClient* m_pHttpClient;
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

	void pause();
	ThreadPoolStatus getStatus();
	void resume();
	void stop();

	CRITICAL_SECTION m_jobQueueCS;
	CONDITION_VARIABLE m_jobCond;
	CONDITION_VARIABLE m_statusCond;
	SRWLOCK m_statusSRW;
private:
	queue<CJob*> jobQueue;	//������У�
	queue<void*> jobDataQueue;//�����������;
	ThreadPoolStatus m_status;	//�߳�����״̬
	

	vector<CMyWorkerThread*> workerVec;
	
};


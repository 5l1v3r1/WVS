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

class ThreadPool;
class WorkerThread;
class Job
{
protected:
	WorkerThread * m_pWorkThread;        //���߳���������
public:
	Job();
	virtual ~Job();

	//��ȡ�����ù����߳�  
	WorkerThread * GetWorkThread()
	{
		return m_pWorkThread;
	}
	void SetWorkThread(WorkerThread * pWorkThread)
	{
		m_pWorkThread = pWorkThread;
	}

	//ָ��Run�ӿ�  
	virtual void Run(void *ptr) = 0;
	bool m_isDeleteAfterDone;
};

class WorkerThread
{
protected:
	static unsigned int __stdcall ThreadFunction(void* args);
public:
	WorkerThread(ThreadPool* m_pThreadPool);
	~WorkerThread();
	void Run(void);

	ThreadStatus getStatus(){
		return m_status;
	}

	//Ϊ��������ӵ�����add_net
	
	CURLcode m_curlCode;
	string m_strHeader = "";
	ThreadPool* m_pThreadPool;
	unsigned long getThreadID(){ return m_ThreadID; }
	CHttpClient* getHttpClient();
private:
	Job* m_Job;
	void* m_JobData;
	unsigned long m_ThreadID;
	TheadStatus m_status;	
	CHttpClient* m_pHttpClient;
};


class ThreadPool
{
public:

	ThreadPool(int initWorkerNum = -1);
	~ThreadPool();
	void setThreadNum(int threadNum);
	void addJob(Job *pJob, void* jobData);
	Job* getJob(void* &pjobData);
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
	queue<Job*> jobQueue;	//������У�
	queue<void*> jobDataQueue;//�����������;
	ThreadPoolStatus m_status;	//�߳�����״̬
	

	vector<WorkerThread*> workerVec;
	
};


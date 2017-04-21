#include "stdafx.h"
#include "MyThreadPool.h"


CMyThreadPool::CMyThreadPool(int initWorkerNum /*= 4*/)
{
	InitializeCriticalSection(&m_jobQueueCS);
	InitializeConditionVariable(&m_jobCond);
	status = 1;
	CMyWorkerThread *pWorkerThread;
	for (int i = 0; i < initWorkerNum; i++)
	{
		pWorkerThread = new CMyWorkerThread(this);
		workerVec.push_back(pWorkerThread);
	}

}

CMyThreadPool::~CMyThreadPool()
{
	DeleteCriticalSection(&m_jobQueueCS);
}


void CMyThreadPool::addJob(CJob *pJob, void* jobData)
{
	EnterCriticalSection(&m_jobQueueCS);
	jobQueue.push(pJob);
	jobDataQueue.push(jobData);
	LeaveCriticalSection(&m_jobQueueCS);
	WakeAllConditionVariable(&m_jobCond);
}

CJob* CMyThreadPool::getJob(void* &pjobData)
{
	CJob * pJob =NULL;
	if (jobQueue.size() > 0){
		pJob = jobQueue.front();
		jobQueue.pop();
		pjobData = jobDataQueue.front();
		jobDataQueue.pop();
	}
	return pJob;
}

int CMyThreadPool::getBusyWorkerNum()
{
	int num = 0;
	for (unsigned i = 0; i < workerVec.size();i++)
	{
		if (workerVec[i]->getStatus() == TheadStatus::BUSY)
			num++;
	}
	return num;
}

int CMyThreadPool::getRestJobNum()
{
	int num;
	EnterCriticalSection(&m_jobQueueCS);
	num = jobQueue.size();
	LeaveCriticalSection(&m_jobQueueCS);
	return num;
}

unsigned int CMyWorkerThread::ThreadFunction(void*arg)
{
	CMyWorkerThread * pThread = (CMyWorkerThread *)arg;
	pThread->Run();
	//	_cprintf("in threadFunction\n");
	return 0;
}


CMyWorkerThread::CMyWorkerThread(CMyThreadPool* pThreadPool)
{
	m_pThreadPool = pThreadPool;
	m_pHttpClient = new CHttpClient();
	m_pHttpClient->setHeaderOpt("", m_strHeader);		//为什么这两句在创建线程之后就会导致ThreadFunction里面的参数为CThread而不是CWorkerThread???
	m_ThreadID = _beginthreadex(NULL, NULL, ThreadFunction, this, 0, NULL);
	_cprintf("%d:\n", m_ThreadID);
}

CMyWorkerThread::~CMyWorkerThread()
{
	delete m_pHttpClient;
}

void CMyWorkerThread::Run(void)
{
	while (m_pThreadPool->status)
	{
		if (m_pThreadPool->status == 2){
			//暂停；
			Sleep(10000000);
		}
		EnterCriticalSection(&(m_pThreadPool->m_jobQueueCS));
		while ((m_Job = (m_pThreadPool->getJob(m_JobData))) == NULL)
		{
			m_status = TheadStatus::WAITTING;
			SleepConditionVariableCS(&(m_pThreadPool->m_jobCond), &(m_pThreadPool->m_jobQueueCS), INFINITE);
		}
		m_status = TheadStatus::BUSY;
		LeaveCriticalSection(&(m_pThreadPool->m_jobQueueCS));
		//Sleep(5000);
		m_Job->SetWorkThread(this);
		m_Job->Run(m_JobData);
		if (m_Job->m_isDeleteAfterDone){
			delete m_Job;
			if (m_JobData != NULL)
				delete m_JobData;
		}
		m_Job = NULL;
		m_JobData = NULL;
	}
	int x=1;
	x++;
	WriteLog("thead_id" + to_string(m_ThreadID) + "  end");
}

CJob::CJob()
{
	m_isDeleteAfterDone = true;
}

CJob::~CJob()
{

}

#include "stdafx.h"
#include "MyThreadPool.h"


CMyThreadPool::CMyThreadPool(int initWorkerNum /*= -1*/)
{
	InitializeCriticalSection(&m_jobQueueCS);
	InitializeConditionVariable(&m_jobCond);
	InitializeSRWLock(&m_statusSRW);
	InitializeConditionVariable(&m_statusCond);
	if (initWorkerNum <= 0)
	{
		//lazy init thread 
		m_status = ThreadPoolStatus::pause;
	}
	else{
		m_status = ThreadPoolStatus::work;
		CMyWorkerThread *pWorkerThread;
		for (int i = 0; i < initWorkerNum; i++)
		{
			pWorkerThread = new CMyWorkerThread(this);
			workerVec.push_back(pWorkerThread);
		}
	}
}

void CMyThreadPool::setThreadNum(int threadNum)
{
	if (workerVec.size() == 0)
	{
		m_status = ThreadPoolStatus::work;
		CMyWorkerThread *pWorkerThread;
		for (int i = 0; i < threadNum; i++)
		{
			pWorkerThread = new CMyWorkerThread(this);
			workerVec.push_back(pWorkerThread);
		}
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
	CJob * pJob = NULL;
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
	for (unsigned i = 0; i < workerVec.size(); i++)
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

void CMyThreadPool::pause()
{
	AcquireSRWLockExclusive(&m_statusSRW);
	m_status = ThreadPoolStatus::pause;
	ReleaseSRWLockExclusive(&m_statusSRW);
}

ThreadPoolStatus CMyThreadPool::getStatus()
{
	ThreadPoolStatus status;
	AcquireSRWLockShared(&m_statusSRW);
	status = m_status;
	ReleaseSRWLockShared(&m_statusSRW);
	return status;
}

void CMyThreadPool::resume()
{
	AcquireSRWLockExclusive(&m_statusSRW);
	m_status = ThreadPoolStatus::work;
	ReleaseSRWLockExclusive(&m_statusSRW);
	WakeAllConditionVariable(&m_statusCond);
}

void CMyThreadPool::stop()	//problem 线程对象没有被释放
{
	AcquireSRWLockExclusive(&m_statusSRW);
	m_status = ThreadPoolStatus::stop; 
	ReleaseSRWLockExclusive(&m_statusSRW);
	WakeAllConditionVariable(&m_jobCond);
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
	_cprintf("CMyWorkerThread: %d:\n", m_ThreadID);
}

CMyWorkerThread::~CMyWorkerThread()
{
	delete m_pHttpClient;
}

void CMyWorkerThread::Run(void)
{
	ThreadPoolStatus threadPoolStatus = ThreadPoolStatus::work;
	bool flag = TRUE;
	while (flag)
	{
		threadPoolStatus = m_pThreadPool->getStatus();
		switch (threadPoolStatus)
		{
			case ThreadPoolStatus::work:
				EnterCriticalSection(&(m_pThreadPool->m_jobQueueCS));
				while (((m_Job = (m_pThreadPool->getJob(m_JobData))) == NULL) &&
					   (m_pThreadPool->getStatus() != ThreadPoolStatus::stop))
				{
					m_status = TheadStatus::WAITTING;
					SleepConditionVariableCS(&(m_pThreadPool->m_jobCond), &(m_pThreadPool->m_jobQueueCS), INFINITE);
				}
				if (ThreadPoolStatus::stop == m_pThreadPool->getStatus())
					break;

				m_status = TheadStatus::BUSY;
				LeaveCriticalSection(&(m_pThreadPool->m_jobQueueCS));
				//Sleep(5000);
				m_Job->SetWorkThread(this);
				m_Job->Run(m_JobData);
				if (m_Job->m_isDeleteAfterDone)
				{
					delete m_Job;
					if (m_JobData != NULL)
						delete m_JobData;
				}
				m_Job = NULL;
				m_JobData = NULL;
				break;

			case ThreadPoolStatus::pause:
				m_status = TheadStatus::WAITTING;
				AcquireSRWLockShared(&(m_pThreadPool->m_statusSRW));
				SleepConditionVariableSRW(&(m_pThreadPool->m_statusCond), &(m_pThreadPool->m_statusSRW), INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED);
				ReleaseSRWLockShared(&(m_pThreadPool->m_statusSRW));
				break;

			case ThreadPoolStatus::stop:
				flag = false;
				break;

			default:
				WriteLog("threadPoolStatus Exception:" + to_string(threadPoolStatus));
		}
	}
	WriteLog("thead_id" + to_string(m_ThreadID) + "  end");
}

CHttpClient* CMyWorkerThread::getHttpClient()
{
	m_pHttpClient->setProxy(CHttpClient::s_useProxy, CHttpClient::s_proxy);
	return m_pHttpClient;
}


CJob::CJob()
{
	m_isDeleteAfterDone = true;
}

CJob::~CJob()
{

}

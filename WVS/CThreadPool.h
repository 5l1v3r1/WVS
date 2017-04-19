#ifndef CTHREAD_POOL_HEADER
#define CTHREAD_POOL_HEADER
#include <vector>
#include <assert.h>
#include <Windows.h>
#include <stdint.h>
#include <process.h>
#include <time.h>
#include "CHttpClient.h"
using namespace std;


static int urlNum = 60;
static int currentNum = 0;


#define INITNUM_MAX 3		//�궨������ʼ���߳�����
#define NUM_MAX	5			//�������߳���
#define AVALIBLE_LOW 2		//��С����

class CThreadPool;
class CJob;
class CThreadMutex
{
private:
	HANDLE m_mutex;
public:
	CThreadMutex();
	~CThreadMutex();
	void Lock();    
	void Unlock();
};

class CCondition
{
private:
	/*
	EnterCriticalSection (&BufferLock);
	StopRequested = TRUE;
	LeaveCriticalSection (&BufferLock);
	*/
	CRITICAL_SECTION m_critSection;
	CONDITION_VARIABLE m_conditionVar;
public:
	CCondition();
	~CCondition();
	void MyEnterCriticalSection();
	void MyLeaveCriticalSection();
	void Wait(unsigned int milliSec = INFINITE);
	void Signal();
};
typedef enum ThreadState
{
	THREAD_RUNNING,
	THREAD_SUSPENDING,
	THREAD_BUSY,
	THREAD_IDLE
}ThreadState;

class CThread
{
private:
	unsigned long m_ThreadID;  
	ThreadState m_ThreadState;      //�߳�״̬  
	
public:
	//Ϊ��������ӵ�����add_net
	CHttpClient* m_pHttpClient;
	CURLcode m_curlCode;
	string m_strHeader = "";

protected:
	static unsigned int __stdcall ThreadFunction(void* args);
public:
	CThread();
	virtual ~CThread();
	virtual void Run(void)
	{
		_cprintf("CThread::Run\n");
	}
	//���û��ȡ�̵߳�״̬  
	void SetThreadState(ThreadState state)
	{
		m_ThreadState = state;
	}
	ThreadState GetThreadState(void)
	{
		return m_ThreadState;
	}

	int GetThreadID()
	{
		return m_ThreadID;
	}
	bool Join();
};

class CWorkerThread : public CThread
{
private:
	CThreadPool*  m_ThreadPool;
	CJob* m_Job;
	void* m_JobData;
	CThreadMutex m_VarMutex;
	bool m_IsEnd;


public:
	CWorkerThread();
	virtual ~CWorkerThread();

	CCondition m_JobCond;
	CThreadMutex m_WorkMutex;

	void Run();

	void SetJob(CJob* job, void* jobdata);
	CJob* GetJob()
	{
		return m_Job;
	}

	void SetThreadPool(CThreadPool* thrpool);
	CThreadPool* GetThreadPool()
	{
		return m_ThreadPool;
	}
};

class CThreadPool
{
public:
	CThreadPool();
	CThreadPool(int initnum);

	virtual ~CThreadPool();

	//����æµ���еĻ�����  
	CThreadMutex m_BusyMutex;
	//���ʿ��ж��еĻ�����  
	CThreadMutex m_IdleMutex;
	//���ʹ�������  
	CThreadMutex m_JobMutex;
	CThreadMutex m_VarMutex;

	//����æµ���е���������  
	CCondition m_BusyCond;
	//���ʿ��ж��е���������  
	CCondition m_IdleCond;
	//ͬ����������  
	CCondition m_IdleJobCond;
	CCondition m_MaxNumCond;

	vector<CWorkerThread*>   m_ThreadList;
	//æµ����  
	vector<CWorkerThread*>   m_BusyList;
	//���ж���  
	vector<CWorkerThread*>   m_IdleList;

	//���û��ȡ�̳߳�������ڵ������߳���  
	void SetMaxNum(int maxNum)
	{
		m_MaxNum = maxNum;
	}
	int GetMaxNum(void)
	{
		return m_MaxNum;
	}

	//���û��ȡ��С��Ҫ�ж��ٸ��߳̿��ã��������̫�٣��򴴽�  
	void SetAvailLowNum(int minNum)
	{
		m_AvailLow = minNum;
	}
	int GetAvailLowNum(void)
	{
		return m_AvailLow;
	}

	//���û��ȡ�����Ҫ�ж��ٸ��߳̿��ã����̫�࣬��ɾ��  
	void SetAvailHighNum(int highNum)
	{
		m_AvailHigh = highNum;
	}
	unsigned int GetAvailHighNum()
	{
		return m_AvailHigh;
	}

	//���ʵ�ʿ��õ��߳����������ڿ���״̬��  
	int GetActualAvailNum()
	{
		return m_AvailNum;
	}
	//��������̵߳�����  
	int GetAllNum()
	{
		return m_ThreadList.size();
	}
	//�������״̬���߳�����  
	int GetBusyNum()
	{
		int num;
		m_BusyMutex.Lock();
		num = m_BusyList.size();
		m_BusyMutex.Unlock();
		return num;
		//return m_BusyList.size();
	}

	//���û��ȡ��ʼʱ�̵߳�����  
	void SetInitNum(int initnum)
	{
		m_InitNum = initnum;
	}
	int GetInitNum()
	{
		return m_InitNum;
	}

	//��ֹ�����߳�  
	void TerminateAll();
 
	void Run(CJob* job, void* jobdata);

	friend class CWorkerThread;

private:

	//������������߳�����  
	unsigned int m_MaxNum;
	//��������ٿ����߳�����  
	unsigned int m_AvailLow;
	//������������߳�����  
	unsigned int m_AvailHigh;
	//���еĿ����߳�����  
	unsigned int m_AvailNum;
	//��ʼʱ���߳�����  
	unsigned int m_InitNum;  //Normal thread num;  

protected:
	CWorkerThread* GetIdleThread(void);

	void AppendToIdleList(CWorkerThread* jobthread);
	
	void MoveToBusyList(CWorkerThread* idlethread);
	
	void MoveToIdleList(CWorkerThread* busythread);
	
	void DeleteIdleThread(int num);
	
	void CreateIdleThread(int num);
};

class CThreadManage
{
public:
	CThreadManage();
	CThreadManage(int num);
	
	virtual ~CThreadManage();

	void SetParallelNum(int num);
	void Run(CJob* job, void* jobArgs);
	void TerminateAll(void);
	int getBusyNum()
	{
		return m_Pool->GetBusyNum();
	}
private:
	CThreadPool* m_Pool; 
	int m_NumOfThread;   //m_NumOfThread�ǳ�ʼ����ʱ���������Ĳ������̸߳�����
};

class CJob
{
protected:
	int m_JobNo;                    //������  
	char * m_JobName;               //��������  
	CThread * m_pWorkThread;        //���߳���������
public:
	CJob();
	virtual ~CJob();

	//���û��ȡ������  
	int GetJobNo(void)
	{
		return m_JobNo;
	}
	void SetJobNo(int jobno)
	{
		m_JobNo = jobno;
	}

	//���û��ȡ��������  
	char* GetJobName(void) const
	{
		return m_JobName;
	}
	void SetJobName(char* jobname);

	//��ȡ�����ù����߳�  
	CThread * GetWorkThread()
	{
		return m_pWorkThread;
	}
	void SetWorkThread(CThread * pWorkThread)
	{
		m_pWorkThread = pWorkThread;
	}

	//ָ��Run�ӿ�  
	virtual void Run(void *ptr) = 0;
	bool m_isDeleteAfterDone;
};
#endif

//void testThreadPool();
//class ExtractJob;
//class Crawler : public CJob
//{
//public:
//	Crawler(string oriUrl, CThreadManage *pManage);
//	void Run(void *ptr);
//	CThreadMutex m_webSiteLinksMutex;
//private:
//	friend class ExtractJob;
//	string m_oriUrl;
//	vector<string> m_webSiteLinks;
//	CThreadManage *m_pManage;
//	CCondition m_webSiteLinksCondition;
//};

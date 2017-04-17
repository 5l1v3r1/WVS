#ifndef CTHREAD_POOL_HEADER
#define CTHREAD_POOL_HEADER
#include <vector>
#include <assert.h>
#include <Windows.h>
#include <stdint.h>
#include <process.h>
#include<time.h>
using namespace std;

#define WINDOWS_SECURITE
#ifdef WINDOWS_SECURITE
#define strcpy(x, y) strcpy_s(x, 200, y)	
#endif

//#define DEBUG_THREADPOOL
#ifdef DEBUG_THREADPOOL
#define print_TP	printf
#else
#define print_TP(...)
#endif

static int urlNum = 60;
static int currentNum = 0;


#define INITNUM_MAX 3		//宏定义最大初始化线程数。
#define NUM_MAX	5			//最大可用线程数
#define AVALIBLE_LOW 2		//最小可用
/*
最大可用
当前可用
*/



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
	void Wait();
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
	ThreadState m_ThreadState;      //线程状态  
protected:
	static unsigned int __stdcall ThreadFunction(void* args);
public:
	CThread();
	virtual ~CThread();
	virtual void Run(void) = 0;
	//设置或获取线程的状态  
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

	//访问忙碌队列的互斥锁  
	CThreadMutex m_BusyMutex;
	//访问空闲队列的互斥锁  
	CThreadMutex m_IdleMutex;
	//访问工作队列  
	CThreadMutex m_JobMutex;
	CThreadMutex m_VarMutex;

	//访问忙碌队列的条件变量  
	CCondition m_BusyCond;
	//访问空闲队列的条件变量  
	CCondition m_IdleCond;
	//同步工作队列  
	CCondition m_IdleJobCond;
	CCondition m_MaxNumCond;

	vector<CWorkerThread*>   m_ThreadList;
	//忙碌队列  
	vector<CWorkerThread*>   m_BusyList;
	//空闲队列  
	vector<CWorkerThread*>   m_IdleList;

	//设置或获取线程池允许存在的最大的线程数  
	void SetMaxNum(int maxNum)
	{
		m_MaxNum = maxNum;
	}
	int GetMaxNum(void)
	{
		return m_MaxNum;
	}

	//设置或获取最小需要有多少个线程可用，如果数量太少，则创建  
	void SetAvailLowNum(int minNum)
	{
		m_AvailLow = minNum;
	}
	int GetAvailLowNum(void)
	{
		return m_AvailLow;
	}

	//设置或获取最多需要有多少个线程可用，如果太多，则删除  
	void SetAvailHighNum(int highNum)
	{
		m_AvailHigh = highNum;
	}
	unsigned int GetAvailHighNum()
	{
		return m_AvailHigh;
	}

	//获得实际可用的线程数量（处于空闲状态）  
	int GetActualAvailNum()
	{
		return m_AvailNum;
	}
	//获得所有线程的数量  
	int GetAllNum()
	{
		return m_ThreadList.size();
	}
	//获得运行状态的线程数量  
	int GetBusyNum()
	{
		int num;
		m_BusyMutex.Lock();
		num = m_BusyList.size();
		m_BusyMutex.Unlock();
		return num;
		//return m_BusyList.size();
	}

	//设置或获取初始时线程的数量  
	void SetInitNum(int initnum)
	{
		m_InitNum = initnum;
	}
	int GetInitNum()
	{
		return m_InitNum;
	}

	//终止所有线程  
	void TerminateAll();
 
	void Run(CJob* job, void* jobdata);

	friend class CWorkerThread;

private:

	//允许创建的最大线程数量  
	unsigned int m_MaxNum;
	//允许的最少空闲线程数量  
	unsigned int m_AvailLow;
	//允许的最大空闲线程数量  
	unsigned int m_AvailHigh;
	//现有的空闲线程数量  
	unsigned int m_AvailNum;
	//初始时的线程数量  
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
	int m_NumOfThread;   //m_NumOfThread是初始创建时候允许创建的并发的线程个数。
};

class CJob
{
private:
	int m_JobNo;                    //任务编号  
	char * m_JobName;               //任务名称  
	CThread * m_pWorkThread;        //在线程里面设置

public:
	CJob();
	virtual ~CJob();

	//设置或获取任务编号  
	int GetJobNo(void)
	{
		return m_JobNo;
	}
	void SetJobNo(int jobno)
	{
		m_JobNo = jobno;
	}

	//设置或获取任务名称  
	char* GetJobName(void) const
	{
		return m_JobName;
	}
	void SetJobName(char* jobname);

	//获取或设置工作线程  
	CThread * GetWorkThread()
	{
		return m_pWorkThread;
	}
	void SetWorkThread(CThread * pWorkThread)
	{
		m_pWorkThread = pWorkThread;
	}

	//指定Run接口  
	virtual void Run(void *ptr) = 0;
	bool m_isDeleteAfterDone;
};

#endif
void testThreadPool();
class ExtractJob;
class Crawler : public CJob
{
public:
	Crawler(string oriUrl, CThreadManage *pManage);
	void Run(void *ptr);
	CThreadMutex m_webSiteLinksMutex;
private:
	friend class ExtractJob;
	string m_oriUrl;
	vector<string> m_webSiteLinks;
	CThreadManage *m_pManage;
	CCondition m_webSiteLinksCondition;
};
class ExtractJob : public CJob
{
public:
	ExtractJob(string url, Crawler *pCrawler);
	~ExtractJob();
	void Run(void *ptr);
	ExtractJob operator=(ExtractJob& a);
private:
	string m_url;
	Crawler *m_pCrawler;
};

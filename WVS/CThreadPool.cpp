#include "stdafx.h"
#include "CThreadPool.h"
CCondition::CCondition()
{
	InitializeCriticalSection(&m_critSection);
	InitializeConditionVariable(&m_conditionVar);
}
CCondition::~CCondition()
{
}
void CCondition::Wait(unsigned int milliSecond)
{
	SleepConditionVariableCS(&m_conditionVar, &m_critSection, milliSecond);
}

void CCondition::Signal()
{
	WakeAllConditionVariable(&m_conditionVar);
}
void CCondition::MyEnterCriticalSection()
{
	 EnterCriticalSection(&m_critSection);
}
void CCondition::MyLeaveCriticalSection()
{
	LeaveCriticalSection(&m_critSection);
}

CThreadMutex::CThreadMutex()
{
	m_mutex = CreateMutex(NULL, FALSE, NULL);
}
CThreadMutex::~CThreadMutex()
{
	if (NULL != m_mutex)
	{
		//CloseHandle(m_mutex);
		FindClose(m_mutex);
	}
	
}
void CThreadMutex::Lock()
{
	WaitForSingleObject(m_mutex, INFINITE);
}
void CThreadMutex::Unlock()
{
	if (!ReleaseMutex(m_mutex))
	{
		cout<<"线程释放失败---------------------\n";
	}
}
CThread::CThread()
{
	m_pHttpClient = new CHttpClient();
	m_pHttpClient->setHeaderOpt("", m_strHeader);		//为什么这两句在创建线程之后就会导致ThreadFunction里面的参数为CThread而不是CWorkerThread???
	m_ThreadID = _beginthreadex(NULL, NULL, ThreadFunction, this, 0, NULL);
}
CThread::~CThread()
{
	delete m_pHttpClient;
}

bool CThread::Join()
{
	if (m_ThreadState == THREAD_BUSY)
	{
		WaitForSingleObject((HANDLE)m_ThreadID, INFINITE);
	}
	return true;
}
unsigned int CThread::ThreadFunction(void*arg)
{
	CThread * pThread = (CThread *)arg;
	pThread->Run();
//	_cprintf("in threadFunction\n");
	return 0;
}

CWorkerThread::CWorkerThread()
{
	//_cprintf("in workerThread\n");
	m_Job = NULL;
	m_JobData = NULL;
	m_ThreadPool = NULL;
	m_IsEnd = false;
}

CWorkerThread::~CWorkerThread()
{
	if (NULL != m_Job)
		delete m_Job;
	if (m_ThreadPool != NULL)
		delete m_ThreadPool;
}

void CWorkerThread::Run()
{
	//_cprintf("CWorkerThread::Run\n");
	SetThreadState(THREAD_RUNNING);
	for (;;)
	{
		while (m_Job == NULL)
			m_JobCond.Wait();

		m_Job->Run(m_JobData);

		//任务执行完毕后的处理  
		m_Job->SetWorkThread(NULL);
		if (m_Job->m_isDeleteAfterDone)
		{
			if (m_Job != NULL)
				delete m_Job;
		}
		m_Job = NULL;
		m_ThreadPool->MoveToIdleList(this);

		//如果线程池中的线程数量过多，则删除一定数量的空闲线程  
		if (m_ThreadPool->m_IdleList.size() > m_ThreadPool->GetAvailHighNum())
		{
			m_ThreadPool->DeleteIdleThread(m_ThreadPool->m_IdleList.size() - m_ThreadPool->GetInitNum());
		}
		m_WorkMutex.Unlock();
	}
}

//设置要执行的任务  
void CWorkerThread::SetJob(CJob* job, void* jobArgs)
{
	m_VarMutex.Lock();
	m_Job = job;
	m_JobData = jobArgs;
	job->SetWorkThread(this);
	m_VarMutex.Unlock();
	m_JobCond.Signal();
}

//设置线程所在的线程池  
void CWorkerThread::SetThreadPool(CThreadPool* thrpool)
{
	m_VarMutex.Lock();
	m_ThreadPool = thrpool;
	m_VarMutex.Unlock();
}


CThreadPool::CThreadPool()
{
	m_MaxNum = 50;
	m_AvailLow = 5;
	m_InitNum = m_AvailNum = 10;
	m_AvailHigh = 20;

	m_BusyList.clear();
	m_IdleList.clear();

	//创建初始数量的工作线程  
	for (unsigned int i = 0; i < m_InitNum; i++)
	{
		CWorkerThread* workThread = new CWorkerThread();
		workThread->SetThreadPool(this);
		//添加到空闲队列  
		AppendToIdleList(workThread);

	}
}

CThreadPool::CThreadPool(int initnum)
{
	//用宏进行判断  
	assert(initnum > 0 && initnum <= INITNUM_MAX);
	m_MaxNum = NUM_MAX;
	m_AvailLow = initnum - 10 > 0 ? initnum - 10 : AVALIBLE_LOW;
	m_InitNum = m_AvailNum = initnum;
	m_AvailHigh = initnum + 10;
	/*m_AvailLow = 1;
	m_AvailHigh = 1;
	m_AvailNum = 1;
	m_InitNum = initnum;*/
	m_BusyList.clear();
	m_IdleList.clear();

	//创建初始数量的工作线程  
	for (unsigned int i = 0; i < m_InitNum; i++)
	{
		CWorkerThread* workThread = new CWorkerThread();
		AppendToIdleList(workThread);
		workThread->SetThreadPool(this);
	}
}

CThreadPool::~CThreadPool()
{
	TerminateAll();
}

//终止所有线程   
void CThreadPool::TerminateAll()
{
	for (unsigned int i = 0; i < m_ThreadList.size(); i++)
	{
		CWorkerThread* workThread = m_ThreadList[i];
		//等待线程终止  
		workThread->Join();
	}
}

//获得空闲线程   
CWorkerThread* CThreadPool::GetIdleThread(void)
{
	//判断条件，并获取所  
	while (m_IdleList.size() == 0)
		m_IdleCond.Wait();
	m_IdleMutex.Lock();

	if (m_IdleList.size() > 0)
	{
		//返回空闲队列首部的成员  
		CWorkerThread * workThread = (CWorkerThread*)m_IdleList.front();
		_cprintf("Get Idle thread %d\n", workThread->GetThreadID());
		m_IdleMutex.Unlock();
		return workThread;
	}

	//获取失败  
	m_IdleMutex.Unlock();
	return NULL;
}

//增加一个工作线程到空闲队列  
void CThreadPool::AppendToIdleList(CWorkerThread* jobthread)
{
	m_IdleMutex.Lock();
	//在所有的线程队列和空闲队列尾部增加一个线程  
	m_ThreadList.push_back(jobthread);  //只有这一个地方可以增加线程，所以全局线程锁可以不用加
	m_IdleList.push_back(jobthread);
	m_IdleMutex.Unlock();
}

//把空闲线程移动到忙碌队列中  
void CThreadPool::MoveToBusyList(CWorkerThread* idlethread)
{
	//在忙碌队列尾部添加线程  
	m_BusyMutex.Lock();
	m_BusyList.push_back(idlethread);
	m_AvailNum--;
	m_BusyMutex.Unlock();

	//在空闲队列中寻找线程，并删除  
	m_IdleMutex.Lock();
	vector<CWorkerThread*>::iterator pos;
	pos = find(m_IdleList.begin(), m_IdleList.end(), idlethread);
	if (pos != m_IdleList.end())
		m_IdleList.erase(pos);
	m_IdleMutex.Unlock();
}

//把忙碌线程移动到空闲队列中  
void CThreadPool::MoveToIdleList(CWorkerThread* busythread)
{
	//在空闲队列尾部添加线程  
	m_IdleMutex.Lock();
	m_IdleList.push_back(busythread);
	m_AvailNum++;
	m_IdleMutex.Unlock();

	//在忙碌队列中寻找线程，并删除  
	m_BusyMutex.Lock();
	vector<CWorkerThread*>::iterator pos;
	pos = find(m_BusyList.begin(), m_BusyList.end(), busythread);
	if (pos != m_BusyList.end())
		m_BusyList.erase(pos);
	m_BusyMutex.Unlock();

	m_IdleCond.Signal();
	m_MaxNumCond.Signal();
}

//创建指定数量的线程，并添加到空闲队列中  
void CThreadPool::CreateIdleThread(int num)
{
	for (int i = 0; i < num; i++)
	{
		CWorkerThread* newThread = new CWorkerThread();
		newThread->SetThreadPool(this);
		//把线程加入到空闲队列中  
		AppendToIdleList(newThread);

		m_VarMutex.Lock();
		m_AvailNum++;
		m_VarMutex.Unlock();
	}
}

//删除指定数量的线程   //bug_1：没有删除全局里面的引用
void CThreadPool::DeleteIdleThread(int num)
{
	_cprintf("Enter into CThreadPool::DeleteIdleThread\n");
	_cprintf("Delete Num is %d\n", num);

	m_IdleMutex.Lock();
	for (int i = 0; i<num; i++)
	{
		CWorkerThread* removeThread;
		if (m_IdleList.size() > 0)
		{
			removeThread = (CWorkerThread*)m_IdleList.front();
			_cprintf("Get Idle thread %d\n", removeThread->GetThreadID());
		}

		//在空闲队列中查找，并删除  
		vector<CWorkerThread*>::iterator pos;
		pos = find(m_IdleList.begin(), m_IdleList.end(), removeThread);
		if (pos != m_IdleList.end())
			m_IdleList.erase(pos);

		m_AvailNum--;

		_cprintf("The idle thread available num:%d \n", m_AvailNum);
		_cprintf("The idlelist num:%d \n", m_IdleList.size());
	}

	m_IdleMutex.Unlock();
}


//找出空闲线程，以执行任务  
void CThreadPool::Run(CJob* job, void* jobArgs)
{
	assert(job != NULL);

	//如果已经达到了最大线程数，则等待		//若可以从线程内部创建任务，则这里可能是导致死锁，可能所有线程都陷到这里。
	if (GetBusyNum() == m_MaxNum)
		m_MaxNumCond.Wait();

	//如果可用线程很少，则创建一定数量的新线程  
	if (m_IdleList.size() < m_AvailLow)
	{
		if (GetAllNum() + m_InitNum - m_IdleList.size() < m_MaxNum)
			CreateIdleThread(m_InitNum - m_IdleList.size());
	}
	else
	{
		CreateIdleThread(m_MaxNum - GetAllNum());
	}
	//获取空闲线程  
	CWorkerThread*  idleThread = GetIdleThread();
	if (idleThread != NULL)
	{
		idleThread->m_WorkMutex.Lock();
		//移动到忙碌队列  
		MoveToBusyList(idleThread);
		//idleThread->SetThreadPool(this);  线程刚开始创建时，已经设置过此属性。
		//任务调用线程进行执行  
		job->SetWorkThread(idleThread);
		_cprintf("Job is set to thread %d \n", idleThread->GetThreadID());
		idleThread->SetJob(job, jobArgs);
	}
}



CThreadManage::CThreadManage()
{
	m_NumOfThread = 10;
	m_Pool = new CThreadPool(m_NumOfThread);
}

CThreadManage::CThreadManage(int num)
{
	m_NumOfThread = num;
	m_Pool = new CThreadPool(m_NumOfThread);
}

CThreadManage::~CThreadManage()
{
	if (m_Pool != NULL)
		delete m_Pool;
}

//设置并行执行的线程数量  
void CThreadManage::SetParallelNum(int num)
{
	m_NumOfThread = num;
}

//具体的执行交给ThreadPool处理  
void CThreadManage::Run(CJob* job, void* jobArgs)
{
	m_Pool->Run(job, jobArgs);
}

//调用ThreadPool，以终结所有的线程  
void CThreadManage::TerminateAll(void)
{
	m_Pool->TerminateAll();
}


CJob::CJob() :m_pWorkThread(NULL), m_JobNo(0), m_JobName(NULL), m_isDeleteAfterDone(false)
{}

CJob::~CJob()
{
	if (NULL != m_JobName)
		free(m_JobName);
}

void CJob::SetJobName(char* jobname)
{
	if (NULL != m_JobName)
	{
		free(m_JobName);
		m_JobName = NULL;
	}
	if (NULL != jobname)
	{
		m_JobName = (char*)malloc(strlen(jobname) + 1);
		strcpy_s(m_JobName, 20, jobname);
	}
}
/*< / pre><strong>线程池使用示例< / strong><br>*/
//class CXJob : public CJob
//{
//	int i;
//public:
//	CXJob()
//	{
//		i = 0;
//	}
//
//	~CXJob()
//	{}
//
//	void Run(void* jobArgs)
//	{
//		Sleep(1000);
//		printf("CXJob  %d\n", (int)jobArgs);
//	}
//};
//class CYJob : public CJob
//{
//public:
//	int i;
//public:
//	CYJob()
//	{
//		i = 0;
//	}
//
//	~CYJob()
//	{
//	}
//
//	void Run(void* jobdata)
//	{
//		printf("CYJob  %d\n", (int)jobdata);
//		Sleep(1500);
//	}
//};

//
//ExtractJob::ExtractJob(string url, Crawler *pCrawler)
//{
//	m_url = url;
//	m_pCrawler = pCrawler;
//}
//ExtractJob ExtractJob::operator=(ExtractJob& a)
//{
//	return ExtractJob(a.m_url, a.m_pCrawler);
//}
//ExtractJob::~ExtractJob()
//{
//	printf("主动进入了析构函数--------------------------------------------------------\n");
//}
//
//void ExtractJob::Run(void *ptr)
//{
//	printf("正在爬取URL(%s)...\n", m_url.c_str());
//	Sleep(500);
//	int k;
//	vector<string>* pWebSiteLinks = (vector<string>*)ptr;
//	if (currentNum < urlNum/2)
//		k = 5;
//	else if (currentNum <urlNum)
//		k = 3;
//	else k = 0;
//	printf("爬取完毕,产生%d条URL\n", k);
//	m_pCrawler->m_webSiteLinksMutex.Lock();
//	for (int i = 0; i < k; i++)
//	{
//		if (currentNum < urlNum)
//		{
//			string tempStr = "I'm url:NO.";
//			char buff[4];
//			//_itoa_s(++currentNum, buff, 4);
//			sprintf_s(buff, 4, "%d", ++currentNum);
//			pWebSiteLinks->push_back(tempStr + buff);
//		}
//	}
//	m_pCrawler->m_webSiteLinksCondition.Signal();
//	m_pCrawler->m_webSiteLinksMutex.Unlock();
//}
//
//Crawler::Crawler(string oriUrl, CThreadManage *pManage)
//{
//	m_oriUrl = oriUrl;
//	m_webSiteLinks.push_back(m_oriUrl);
//	m_webSiteLinksMutex = CThreadMutex();
//	m_pManage = pManage;
//	m_webSiteLinksCondition = CCondition();
//}
//clock_t t_begin, t_end;
//void Crawler::Run(void *ptr)
//{
//	t_begin = clock();
//	string res;
//	bool flag = true;
//	while (flag)
//	{
//		m_webSiteLinksCondition.MyEnterCriticalSection();
//		while (m_webSiteLinks.size() <= 0)
//		{
//			if (m_pManage->getBusyNum() == 0)
//			{
//				false;
//			}
//			m_webSiteLinksCondition.Wait();
//		}
//		m_webSiteLinksCondition.MyLeaveCriticalSection();
//		m_webSiteLinksMutex.Lock();
//		res = m_webSiteLinks.back();
//		m_webSiteLinks.pop_back();
//		m_webSiteLinksMutex.Unlock();
//		ExtractJob *pTempExtract = new ExtractJob(res, this);	
//		pTempExtract->m_isDeleteAfterDone = true;	//动态申请，需要在任务完成后，由外部操作删除（此处为线程run）
//		m_pManage->Run(pTempExtract, (void*)&m_webSiteLinks);
//		//pTempExtract->Run( (void*)&m_webSiteLinks);
//		/*if (currentNum == urlNum)
//		{
//		m_pManage->TerminateAll();
//		flag = false;
//		}*/
//
//	}
//	t_end = clock();
//	printf("耗时：%ld\n", (t_end - t_begin) / CLOCKS_PER_SEC);
//
//} 
//
//
////问题是，如何停止呢？
//void testThreadPool()
//{
//	CThreadManage* pManage = new CThreadManage(1);
//	Crawler crawler = Crawler("oriGinUrl:No.0", pManage);
//	crawler.Run(NULL);
//	//CThreadManage* manage = new CThreadManage(5);
//	//for (int i = 0; i < 10; i++)
//	//{
//	//	CXJob * job = new CXJob();
//	//	manage->Run(job, (void*)i);
//	//}
//	//CXJob *xjob = new CXJob();
//	//xjob->Run((void*)200000);
//	////Sleep(2000);
//	//CYJob* job = new CYJob();
//	//manage->Run(job, NULL);
//	//manage->TerminateAll();
//}
/*< / pre><br>
CXJob和CYJob都是从Job类继承而来，其都实现了Run接口。
CXJob只是简单的打印一句”The Job comes from CXJob”，
CYJob也只打印”The Job comes from CYJob”，然后均休眠2秒钟。在主程序中我们初始创建10个工作线程。然后分别执行40次CXJob和一次CYJob。<br>
<p><strong><span style = "color:#3333ff">线程池适合场合
<p>事实上，线程池并不是万能的。它有其特定的使用场合。
线程池致力于减少线程本身的开销对应用所产生的影响，这是有前提的，前提就是线程本身开销与线程执行任务相比不可忽略。如果线程本身的开销相对于线程任务执行开销而言是可以忽略不计的（即<span style = "background-color:#ffff66">如果< / span><span style = "background-color:#ffff66">每个线程都有一个大型任务需要处理，则线程池优势不明显，甚至会因为线程切换而带来效率上的降低< / span>），那么此时线程池所带来的好处是不明显的，比如对于FTP服务器以及Telnet服务器，通常传送文件的时间较长，开销较大，那么此时，我们采用线程池未必是理想的方法，我们可以选择“即时创建，即时销毁”的策略。< / p>
<p>总之线程池通常<span style = "background-color:#66ff99">适合下面的几个场合< / span>：< / p>
<p>(1)  单位时间内处理任务频繁而且任务处理时间短< / p>
<p>(2)  对实时性要求较高。如果接受到任务后在创建线程，可能满足不了实时要求，因此必须采用线程池进行预创建。< / p>
<p>(3)  必须经常面对高突发性事件，比如Web服务器，如果有足球转播，则服务器将产生巨大的冲击。
此时如果采取传统方法，则必须不停的大量产生线程，销毁线程。此时采用动态线程池可以避免这种情况的发生。< / p>
结束语
<p>本文给出了一个简单的通用的与任务无关的线程池的实现，
通过该线程池能够极大的简化Linux下多线程的开发工作。
该线程池的进一步完善开发工作还在进行中，希望能够得到你的建议和支持。< / p>*/
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
		cout<<"�߳��ͷ�ʧ��---------------------\n";
	}
}
CThread::CThread()
{
	m_pHttpClient = new CHttpClient();
	m_pHttpClient->setHeaderOpt("", m_strHeader);		//Ϊʲô�������ڴ����߳�֮��ͻᵼ��ThreadFunction����Ĳ���ΪCThread������CWorkerThread???
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

		//����ִ����Ϻ�Ĵ���  
		m_Job->SetWorkThread(NULL);
		if (m_Job->m_isDeleteAfterDone)
		{
			if (m_Job != NULL)
				delete m_Job;
		}
		m_Job = NULL;
		m_ThreadPool->MoveToIdleList(this);

		//����̳߳��е��߳��������࣬��ɾ��һ�������Ŀ����߳�  
		if (m_ThreadPool->m_IdleList.size() > m_ThreadPool->GetAvailHighNum())
		{
			m_ThreadPool->DeleteIdleThread(m_ThreadPool->m_IdleList.size() - m_ThreadPool->GetInitNum());
		}
		m_WorkMutex.Unlock();
	}
}

//����Ҫִ�е�����  
void CWorkerThread::SetJob(CJob* job, void* jobArgs)
{
	m_VarMutex.Lock();
	m_Job = job;
	m_JobData = jobArgs;
	job->SetWorkThread(this);
	m_VarMutex.Unlock();
	m_JobCond.Signal();
}

//�����߳����ڵ��̳߳�  
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

	//������ʼ�����Ĺ����߳�  
	for (unsigned int i = 0; i < m_InitNum; i++)
	{
		CWorkerThread* workThread = new CWorkerThread();
		workThread->SetThreadPool(this);
		//��ӵ����ж���  
		AppendToIdleList(workThread);

	}
}

CThreadPool::CThreadPool(int initnum)
{
	//�ú�����ж�  
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

	//������ʼ�����Ĺ����߳�  
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

//��ֹ�����߳�   
void CThreadPool::TerminateAll()
{
	for (unsigned int i = 0; i < m_ThreadList.size(); i++)
	{
		CWorkerThread* workThread = m_ThreadList[i];
		//�ȴ��߳���ֹ  
		workThread->Join();
	}
}

//��ÿ����߳�   
CWorkerThread* CThreadPool::GetIdleThread(void)
{
	//�ж�����������ȡ��  
	while (m_IdleList.size() == 0)
		m_IdleCond.Wait();
	m_IdleMutex.Lock();

	if (m_IdleList.size() > 0)
	{
		//���ؿ��ж����ײ��ĳ�Ա  
		CWorkerThread * workThread = (CWorkerThread*)m_IdleList.front();
		_cprintf("Get Idle thread %d\n", workThread->GetThreadID());
		m_IdleMutex.Unlock();
		return workThread;
	}

	//��ȡʧ��  
	m_IdleMutex.Unlock();
	return NULL;
}

//����һ�������̵߳����ж���  
void CThreadPool::AppendToIdleList(CWorkerThread* jobthread)
{
	m_IdleMutex.Lock();
	//�����е��̶߳��кͿ��ж���β������һ���߳�  
	m_ThreadList.push_back(jobthread);  //ֻ����һ���ط����������̣߳�����ȫ���߳������Բ��ü�
	m_IdleList.push_back(jobthread);
	m_IdleMutex.Unlock();
}

//�ѿ����߳��ƶ���æµ������  
void CThreadPool::MoveToBusyList(CWorkerThread* idlethread)
{
	//��æµ����β������߳�  
	m_BusyMutex.Lock();
	m_BusyList.push_back(idlethread);
	m_AvailNum--;
	m_BusyMutex.Unlock();

	//�ڿ��ж�����Ѱ���̣߳���ɾ��  
	m_IdleMutex.Lock();
	vector<CWorkerThread*>::iterator pos;
	pos = find(m_IdleList.begin(), m_IdleList.end(), idlethread);
	if (pos != m_IdleList.end())
		m_IdleList.erase(pos);
	m_IdleMutex.Unlock();
}

//��æµ�߳��ƶ������ж�����  
void CThreadPool::MoveToIdleList(CWorkerThread* busythread)
{
	//�ڿ��ж���β������߳�  
	m_IdleMutex.Lock();
	m_IdleList.push_back(busythread);
	m_AvailNum++;
	m_IdleMutex.Unlock();

	//��æµ������Ѱ���̣߳���ɾ��  
	m_BusyMutex.Lock();
	vector<CWorkerThread*>::iterator pos;
	pos = find(m_BusyList.begin(), m_BusyList.end(), busythread);
	if (pos != m_BusyList.end())
		m_BusyList.erase(pos);
	m_BusyMutex.Unlock();

	m_IdleCond.Signal();
	m_MaxNumCond.Signal();
}

//����ָ���������̣߳�����ӵ����ж�����  
void CThreadPool::CreateIdleThread(int num)
{
	for (int i = 0; i < num; i++)
	{
		CWorkerThread* newThread = new CWorkerThread();
		newThread->SetThreadPool(this);
		//���̼߳��뵽���ж�����  
		AppendToIdleList(newThread);

		m_VarMutex.Lock();
		m_AvailNum++;
		m_VarMutex.Unlock();
	}
}

//ɾ��ָ���������߳�   //bug_1��û��ɾ��ȫ�����������
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

		//�ڿ��ж����в��ң���ɾ��  
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


//�ҳ������̣߳���ִ������  
void CThreadPool::Run(CJob* job, void* jobArgs)
{
	assert(job != NULL);

	//����Ѿ��ﵽ������߳�������ȴ�		//�����Դ��߳��ڲ�������������������ǵ������������������̶߳��ݵ����
	if (GetBusyNum() == m_MaxNum)
		m_MaxNumCond.Wait();

	//��������̺߳��٣��򴴽�һ�����������߳�  
	if (m_IdleList.size() < m_AvailLow)
	{
		if (GetAllNum() + m_InitNum - m_IdleList.size() < m_MaxNum)
			CreateIdleThread(m_InitNum - m_IdleList.size());
	}
	else
	{
		CreateIdleThread(m_MaxNum - GetAllNum());
	}
	//��ȡ�����߳�  
	CWorkerThread*  idleThread = GetIdleThread();
	if (idleThread != NULL)
	{
		idleThread->m_WorkMutex.Lock();
		//�ƶ���æµ����  
		MoveToBusyList(idleThread);
		//idleThread->SetThreadPool(this);  �̸߳տ�ʼ����ʱ���Ѿ����ù������ԡ�
		//��������߳̽���ִ��  
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

//���ò���ִ�е��߳�����  
void CThreadManage::SetParallelNum(int num)
{
	m_NumOfThread = num;
}

//�����ִ�н���ThreadPool����  
void CThreadManage::Run(CJob* job, void* jobArgs)
{
	m_Pool->Run(job, jobArgs);
}

//����ThreadPool�����ս����е��߳�  
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
/*< / pre><strong>�̳߳�ʹ��ʾ��< / strong><br>*/
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
//	printf("������������������--------------------------------------------------------\n");
//}
//
//void ExtractJob::Run(void *ptr)
//{
//	printf("������ȡURL(%s)...\n", m_url.c_str());
//	Sleep(500);
//	int k;
//	vector<string>* pWebSiteLinks = (vector<string>*)ptr;
//	if (currentNum < urlNum/2)
//		k = 5;
//	else if (currentNum <urlNum)
//		k = 3;
//	else k = 0;
//	printf("��ȡ���,����%d��URL\n", k);
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
//		pTempExtract->m_isDeleteAfterDone = true;	//��̬���룬��Ҫ��������ɺ����ⲿ����ɾ�����˴�Ϊ�߳�run��
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
//	printf("��ʱ��%ld\n", (t_end - t_begin) / CLOCKS_PER_SEC);
//
//} 
//
//
////�����ǣ����ֹͣ�أ�
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
CXJob��CYJob���Ǵ�Job��̳ж������䶼ʵ����Run�ӿڡ�
CXJobֻ�Ǽ򵥵Ĵ�ӡһ�䡱The Job comes from CXJob����
CYJobҲֻ��ӡ��The Job comes from CYJob����Ȼ�������2���ӡ��������������ǳ�ʼ����10�������̡߳�Ȼ��ֱ�ִ��40��CXJob��һ��CYJob��<br>
<p><strong><span style = "color:#3333ff">�̳߳��ʺϳ���
<p>��ʵ�ϣ��̳߳ز��������ܵġ��������ض���ʹ�ó��ϡ�
�̳߳������ڼ����̱߳���Ŀ�����Ӧ����������Ӱ�죬������ǰ��ģ�ǰ������̱߳��������߳�ִ��������Ȳ��ɺ��ԡ�����̱߳���Ŀ���������߳�����ִ�п��������ǿ��Ժ��Բ��Ƶģ���<span style = "background-color:#ffff66">���< / span><span style = "background-color:#ffff66">ÿ���̶߳���һ������������Ҫ�������̳߳����Ʋ����ԣ���������Ϊ�߳��л�������Ч���ϵĽ���< / span>������ô��ʱ�̳߳��������ĺô��ǲ����Եģ��������FTP�������Լ�Telnet��������ͨ�������ļ���ʱ��ϳ��������ϴ���ô��ʱ�����ǲ����̳߳�δ��������ķ��������ǿ���ѡ�񡰼�ʱ��������ʱ���١��Ĳ��ԡ�< / p>
<p>��֮�̳߳�ͨ��<span style = "background-color:#66ff99">�ʺ�����ļ�������< / span>��< / p>
<p>(1)  ��λʱ���ڴ�������Ƶ������������ʱ���< / p>
<p>(2)  ��ʵʱ��Ҫ��ϸߡ�������ܵ�������ڴ����̣߳��������㲻��ʵʱҪ����˱�������̳߳ؽ���Ԥ������< / p>
<p>(3)  ���뾭����Ը�ͻ�����¼�������Web�����������������ת������������������޴�ĳ����
��ʱ�����ȡ��ͳ����������벻ͣ�Ĵ��������̣߳������̡߳���ʱ���ö�̬�̳߳ؿ��Ա�����������ķ�����< / p>
������
<p>���ĸ�����һ���򵥵�ͨ�õ��������޹ص��̳߳ص�ʵ�֣�
ͨ�����̳߳��ܹ�����ļ�Linux�¶��̵߳Ŀ���������
���̳߳صĽ�һ�����ƿ����������ڽ����У�ϣ���ܹ��õ���Ľ����֧�֡�< / p>*/
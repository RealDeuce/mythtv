/////////////////////////////////////////////////////////////////////////////
// Program Name: threadpool.h
//                                                                            
// Purpose - Thread Pool Class
//                                                                            
// Created By  : David Blain                    Created On : Oct. 21, 2005
// Modified By :                                Modified On:                  
//                                                                            
/////////////////////////////////////////////////////////////////////////////

#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <deque>
using namespace std;

#include <QString> 
#include <QMutex> 
#include <QWaitCondition>
#include <QThread>
#include <QTimer>
#include <QObject>

class ThreadPool;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CEvent 
{
    private:

        QMutex          m_mutex;
        QWaitCondition  m_wait;
        bool            m_bSignaled;

    public:

                  CEvent( bool bInitiallyOwn = false );
         virtual ~CEvent();

        bool      SetEvent    ();
        bool      ResetEvent  ();
        bool      IsSignaled  ();
        bool      WaitForEvent( unsigned long time = ULONG_MAX  );
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// WorkerThread Class Declaration
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class WorkerThread : public QThread
{
    Q_OBJECT

    protected:

        QMutex              m_mutex;

        CEvent              m_Initialized;
        bool                m_bInitialized;
                         
        ThreadPool         *m_pThreadPool;

        volatile bool       m_bTermRequested;
        QString             m_sName;

        long                m_nIdleTimeoutMS;
        bool                m_bAllowTimeout;
        QTimer             *m_timer;


    protected:

        virtual void  run();
        virtual void  ProcessWork() = 0;


    public:

                 WorkerThread( ThreadPool *pThreadPool, const QString &sName );
        virtual ~WorkerThread();

        bool     WaitForInitialized( unsigned long msecs );
        void     SetTimeout        ( long nIdleTimeout );

    public slots:
        void     SignalWork();
        void     TimeOut();

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// CThreadPool Class Declaration
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

typedef deque<WorkerThread*> WorkerThreadList;

class ThreadPool
{
    friend class WorkerThread;

    protected:

        QString                     m_sName;

        QMutex                      m_mList;

        QWaitCondition              m_threadAvail;

        WorkerThreadList            m_lstThreads;
        WorkerThreadList            m_lstAvailableThreads;

        int                         m_nInitialThreadCount;
        int                         m_nMaxThreadCount;
        long                        m_nIdleTimeout;

    protected:

        void            InitializeThreads();

        WorkerThread   *AddWorkerThread  ( bool bMakeAvailable, long nTimeout );

        void            ThreadAvailable  ( WorkerThread *pThread );
        void            ThreadTerminating( WorkerThread *pThread );

        virtual WorkerThread *CreateWorkerThread( ThreadPool *, const QString &sName ) = 0;

    public:

                        ThreadPool( const QString &sName );
        virtual        ~ThreadPool( );

        WorkerThread   *GetWorkerThread();

};


#endif

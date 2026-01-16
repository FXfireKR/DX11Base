#include "pch.h"
#include "CDummyThread.h"

CDummyThread::CDummyThread()
{
	//t = jthread(&CDummyThread::_Run, this);
}

CDummyThread::~CDummyThread()
{
}

void CDummyThread::Start()
{
	m_bRunning.store(true);
}

void CDummyThread::Stop()
{

}

void CDummyThread::_Run()
{
	int value = 0;
	while (!st.stop_requested()) {
		// wait
		//cout << value << endl;
		++value;

		std::this_thread::yield;
	}

	//g_DeviceContext->Flush
}
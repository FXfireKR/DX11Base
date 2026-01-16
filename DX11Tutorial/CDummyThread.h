#pragma once

class CDummyThread
{
public:
	CDummyThread();
	~CDummyThread();

	void Start();
	void Stop();

	inline const bool& IsRunning() { return m_bRunning.load(); }

private:
	void _Run();

private:
	atomic<bool> m_bRunning;

	jthread t;
	stop_token st;
};
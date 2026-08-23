#pragma once
#include <Windows.h>

class CScopedCpuTimer
{
public:
    explicit CScopedCpuTimer(float& outMs)
        : m_outMs(outMs)
        , m_dFreqInv(_GetFrequencyInv())
    {
        QueryPerformanceCounter(&m_begin);
    }

    ~CScopedCpuTimer()
    {
        LARGE_INTEGER end{};
        QueryPerformanceCounter(&end);

        const LONGLONG delta =
            end.QuadPart - m_begin.QuadPart;

        m_outMs = static_cast<float>(static_cast<double>(delta) * m_dFreqInv);
    }

private:
    static double _GetFrequencyInv()
    {
        static const double value = []()
        {
            LARGE_INTEGER freq{};
            QueryPerformanceFrequency(&freq);

            return 1000.0 / static_cast<double>(freq.QuadPart);
        }();

        return value;
    }

private:
    float& m_outMs;
    LARGE_INTEGER m_begin{};
    double m_dFreqInv = 0.0;
};
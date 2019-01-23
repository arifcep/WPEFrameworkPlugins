#pragma once

#include "Module.h"

namespace WPEFramework {

class MemoryAllocation {
    private:
        MemoryAllocation(const MemoryAllocation&) = delete;
        MemoryAllocation& operator=(const MemoryAllocation&) = delete;

    public:
    MemoryAllocation()
            : _currentMemoryAllocation(0)
            , _lock()
            , _process()
        {
            DisableOOMKill();
            _startSize = static_cast<uint32_t>(_process.Allocated() >> 10);
            _startResident = static_cast<uint32_t>(_process.Resident() >> 10);
        }

        static MemoryAllocation& Instance()
        {
            static MemoryAllocation _singleton;
            return (_singleton);
        }

        ~MemoryAllocation() = default;

    private:
        // Memory Allocation methods
        bool Malloc(uint32_t size) // size in Kb
        {
            bool status = true;
            uint32_t noOfBlocks = 0;
            uint32_t blockSize = (32 * (getpagesize() >> 10)); // 128kB block size
            uint32_t runs = (uint32_t)size / blockSize;

            _lock.Lock();
            for (noOfBlocks = 0; noOfBlocks < runs; ++noOfBlocks)
            {
                _memory.push_back(malloc(static_cast<size_t>(blockSize << 10)));
                if (!_memory.back())
                {
                    SYSLOG(Trace::Fatal, (_T("*** Failed allocation !!! ***")));
                    status = false;
                    break;
                }

                for (uint32_t index = 0; index < (blockSize << 10); index++)
                {
                    static_cast<unsigned char*>(_memory.back())[index] = static_cast<unsigned char>(rand() & 0xFF);
                }
            }
            _currentMemoryAllocation += (noOfBlocks * blockSize);
            _lock.Unlock();

            return status;
        }

        void Statm(uint32_t &allocated, uint32_t &size, uint32_t &resident)
        {
            _lock.Lock();
            allocated = _currentMemoryAllocation;
            _lock.Unlock();

            size = static_cast<uint32_t>(_process.Allocated() >> 10);
            resident = static_cast<uint32_t>(_process.Resident() >> 10);

            LogMemoryUsage();
        }

        bool Free(void)
        {
            bool status = false;

            if (!_memory.empty())
            {
                for (auto const& memoryBlock : _memory)
                {
                    free(memoryBlock);
                }
                _memory.clear();
                status = true;
            }

            _lock.Lock();
            _currentMemoryAllocation = 0;
            _lock.Unlock();

            return status;
        }

        void DisableOOMKill(void)
        {
            int8_t oomNo = -17;
            _process.OOMAdjust(oomNo);
        }

        void LogMemoryUsage(void)
        {
            SYSLOG(Trace::Information, (_T("*** Current allocated: %lu Kb ***"), _currentMemoryAllocation));
            SYSLOG(Trace::Information, (_T("*** Initial Size:     %lu Kb ***"), _startSize));
            SYSLOG(Trace::Information, (_T("*** Initial Resident: %lu Kb ***"), _startResident));
            SYSLOG(Trace::Information, (_T("*** Size:     %lu Kb ***"), static_cast<uint32_t>(_process.Allocated() >> 10)));
            SYSLOG(Trace::Information, (_T("*** Resident: %lu Kb ***"), static_cast<uint32_t>(_process.Resident() >> 10)));
        }

    private:
        uint32_t _startSize;
        uint32_t _startResident;
        Core::CriticalSection _lock;
        Core::ProcessInfo _process;
        std::list<void*> _memory;
        uint32_t _currentMemoryAllocation; // size in Kb
};
} // namespace WPEFramework

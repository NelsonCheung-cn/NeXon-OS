#include "thread.h"
#include "cstdio.h"
#include "cstdlib.h"
#include "interrupt.h"
#include "process.h"

void kernelThread(ThreadFunction *function, void *arg)
{
    function(arg);
}

dword createThread(ThreadFunction func, void *arg, byte priority)
{
    PCB *thread = (PCB *)allocatePages(AddressPoolType::KERNEL, 1);
    if (!thread)
        return -1;

    memset((byte *)thread, 0, PAGE_SIZE);

    thread->status = ThreadStatus::READY;
    thread->priority = priority;
    thread->ticks = priority;
    thread->ticksPassedBy = 0;
    thread->pageDir = nullptr;
    thread->magic = 0x19870916;

    thread->pid = PID++;
    if (PID == -1)
        PID = 0;

    thread->stack = (dword *)((dword)thread + PAGE_SIZE - sizeof(ThreadInterruptStack) - sizeof(ThreadStack));

    ThreadStack *threadStack = (ThreadStack *)thread->stack;
    threadStack->eip = kernelThread;
    threadStack->function = func;
    threadStack->funcArg = arg;
    threadStack->ebp =
        threadStack->ebx =
            threadStack->esi =
                threadStack->edi = 0;

    bool interruptStatus = _interrupt_status();
    _disable_interrupt();
    _all_threads.push_back(&(thread->tagInAllList));
    _ready_threads.push_back(&(thread->tagInGeneralList));
    _set_interrupt(interruptStatus);

    return thread->pid;
}

void _schedule_thread()
{
    PCB *cur = (PCB *)_running_thread();
    if (_all_threads.find(&(cur->tagInAllList)) == -1)
        return;

    if (cur->status == ThreadStatus::RUNNING)
    {
        cur->status = ThreadStatus::READY;
        cur->ticks = cur->priority;
        _ready_threads.push_back(&(cur->tagInGeneralList));
    }
    else
    {
    }

    PCB *next = elem2entry(PCB, tagInGeneralList, _ready_threads.front());
    _ready_threads.pop_front();
    next->status = ThreadStatus::RUNNING;

    activatePageTab(next);
    _switch_thread_to(cur, next);
}

void _block_thread()
{
    bool intrStatus = _interrupt_status();
    _disable_interrupt();
    PCB *thread = (PCB *)_running_thread();
    thread->status = ThreadStatus::BLOCKED;
    _schedule_thread();
    _set_interrupt(intrStatus);
}

void _wake_up_thread(PCB *thread)
{
    bool intrStatus = _interrupt_status();
    _disable_interrupt();
    thread->status = ThreadStatus::READY;
    _ready_threads.push_front(&(thread->tagInGeneralList));
    // 必须要立即唤醒，否则可能永远也无法唤醒
    _schedule_thread();
    _set_interrupt(intrStatus);
}
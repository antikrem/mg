#include "thread_safe.h"

bool ThreadSafe::tryLock() {
	return threadLock.try_lock();
}

void ThreadSafe::lock() {
	threadLock.lock();
}

void ThreadSafe::unlock() {
	threadLock.unlock();
}
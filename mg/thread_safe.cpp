#include "thread_safe.h"

void ThreadSafe::lock() {
	threadLock.lock();
}

void ThreadSafe::unlock() {
	threadLock.unlock();
}
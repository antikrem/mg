#ifndef __THREAD_SAFE_H_INCLUDED__
#define __THREAD_SAFE_H_INCLUDED__

#include <mutex>

/*Inherit to have a generic lock for a class*/
class ThreadSafe {
private:
	std::mutex threadLock;
public:
	/*Must be unlocked in the same thread*/
	void lock();

	/*Must unlock a lock established in the same thread*/
	void unlock();

};

#endif
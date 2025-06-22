/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */



// thread-pool.cc

#include "thread-pool.h"

ThreadPool::ThreadPool(size_t numThreads)
    : workers(numThreads),
      workersAvailable(numThreads),
      tasksPending(0),
      done(false)
{
    // Lanza hilos de worker
    for (size_t i = 0; i < workers.size(); ++i) {
        workers[i].available = true;
        new(&workers[i].sem) Semaphore(0);
        workers[i].thread = std::thread(&ThreadPool::workerLoop, this, i);
    }
    // Lanza dispatcher
    dispatcherThread = std::thread(&ThreadPool::dispatcher, this);
}

ThreadPool::~ThreadPool() {
    // Esperar a que terminen todas las tareas
    wait();
    // Señalar cierre
    done = true;
    // Despertar dispatcher y workers
    taskAvailable.notify_one();
    for (auto &w : workers) {
        w.sem.signal();
    }
    // Unir hilos
    if (dispatcherThread.joinable()) dispatcherThread.join();
    for (auto &w : workers) {
        if (w.thread.joinable()) w.thread.join();
    }
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.push(std::move(thunk));
        tasksPending++;
    }
    taskAvailable.notify_one();
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(waitMutex);
    waitCv.wait(lock, [this]() { return tasksPending.load() == 0; });
}

void ThreadPool::dispatcher() {
    while (true) {
        std::function<void(void)> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            taskAvailable.wait(lock, [this]() { return !tasks.empty() || done.load(); });
            if (tasks.empty() && done.load()) break;
            task = std::move(tasks.front());
            tasks.pop();
        }
        // Esperar un worker libre (solo una vez)
        workersAvailable.wait();

        // Asignar al primer worker disponible
        for (auto &w : workers) {
            if (w.available) {
                w.available = false;
                w.task = std::move(task);
                w.sem.signal();
                break;
            }
        }

    }
}

void ThreadPool::workerLoop(size_t id) {
    auto &w = workers[id];
    while (true) {
        w.sem.wait();
        if (done.load() && tasksPending.load() == 0) break;
        // Ejecutar tarea
        w.task();
        // Tarea completada
        tasksPending--;
        if (tasksPending.load() == 0) {
            std::lock_guard<std::mutex> lock(waitMutex);
            waitCv.notify_all();
        }
        // Marcar worker libre
        w.available = true;
        workersAvailable.signal();
    }
}


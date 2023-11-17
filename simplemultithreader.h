#include <pthread.h>
#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>
#include <chrono>

int user_main(int argc, char **argv);

typedef struct {
    int low;
    int high;
    std::function<void(int)> lambda;
} thread_args1;

typedef struct {
    int low1, low2;
    int high1, high2;
    std::function<void(int, int)> lambda;
} thread_args2;

void *thread_func1(void *ptr) {
    thread_args1 *t = static_cast<thread_args1 *>(ptr);
    int i = t->low;
    while (i < t->high) {
        t->lambda(i);
        ++i;
    }
    return NULL;
}

void *thread_func2(void *ptr) {
    thread_args2 *t = static_cast<thread_args2 *>(ptr);
    int i = t->low1;
    while (i < t->high1) {
        int j = t->low2;
        while (j < t->high2) {
            t->lambda(i, j);
            ++j;
        }
        ++i;
    }
    return NULL;
}

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
    pthread_t tid[numThreads];
    thread_args1 args[numThreads];
    int chunk = (high - low) / numThreads;

    auto start_time = std::chrono::steady_clock::now();

    int i = 0;
    while (i < numThreads) {
        args[i].low = i * chunk;
        args[i].high = (i + 1) * chunk;
        args[i].lambda = lambda;
        if (i == numThreads - 1) {
            args[i].high = high;
        }
        if (pthread_create(&tid[i], nullptr, thread_func1, &args[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ++i;
    }

    i = 0;
    while (i < numThreads) {
        if (pthread_join(tid[i], nullptr) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ++i;
    }

    auto end_time = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Execution time for 1D parallel_for : " << elapsed_time.count() << " seconds\n";
}

void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads) {
    pthread_t tid[numThreads];
    thread_args2 args[numThreads];
    int chunk1 = (high1 - low1) / numThreads;
    auto start_time = std::chrono::steady_clock::now();
    
    int i = 0;
    while (i < numThreads) {
        args[i].low1 = i * chunk1;
        args[i].high1 = (i + 1) * chunk1;
        args[i].low2 = low2;
        args[i].high2 = high2;
        args[i].lambda = lambda;
        if (i == numThreads - 1) {
            args[i].high1 = high1;
        }
        if (pthread_create(&tid[i], nullptr, thread_func2, &args[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ++i;
    }

    i = 0;
    while (i < numThreads) {
        if (pthread_join(tid[i], nullptr) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
        ++i;
    }

    auto end_time = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Execution time for 2D parallel_for : " << elapsed_time.count() << " seconds\n";
}

int main(int argc, char **argv) {
    int rc = user_main(argc, argv);
    return rc;
}

#define main user_main

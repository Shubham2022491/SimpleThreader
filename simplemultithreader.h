#include <pthread.h>
#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>

int user_main(int argc, char **argv);



typedef struct {
    int low;
    int high;
    std::function<void(int)> lambda;
} thread_args1;

typedef struct {
    int low1,low2;
    int high1,high2;
    std::function<void(int,int)> lambda;
} thread_args2;





void *thread_func1(void *ptr) {
    thread_args1 * t = ((thread_args1 *) ptr);    
    for (int i = t->low ; i < t->high ; i++){
        t->lambda(i);
    }
return NULL;
}

void *thread_func2(void *ptr) {
    thread_args2 * t = ((thread_args2 *) ptr);    
    for (int i = t->low1 ; i < t->high1 ; i++){
        for (int j = t->low2 ; j < t->high2 ; j++){
            t->lambda(i,j);
        }
    }
return NULL;
}


void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads){
    pthread_t tid[numThreads];
    thread_args1 args[numThreads];
    int chunk = (high-low)/numThreads;
    for (int i=0; i<numThreads; i++) {
        args[i].low=i*chunk; 
        args[i].high=(i+1)*chunk;
        args[i].lambda = lambda;  // Lambda capture by value
        if (i == numThreads -1 ){
            args[i].high = high ;
        } 
        if (pthread_create(&tid[i], NULL, thread_func1, &args[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    for (int i=0; i<numThreads; i++) {
        if (pthread_join(tid[i], NULL) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}


void parallel_for(int low1, int high1, int low2, int high2,std::function<void(int, int)> &&lambda, int numThreads){
    pthread_t tid[numThreads];
    thread_args2 args[numThreads];
    int chunk1 = (high1 - low1) / numThreads;
    int chunk2 = (high2 - low2) / numThreads;
    for (int i=0; i<numThreads; i++) {
        args[i].low1 = i * chunk1;
        args[i].high1 = (i + 1) * chunk1;
        args[i].low2 = low2;
        args[i].high2 = high2;
        args[i].lambda = lambda;
        if (i == numThreads -1 ) {
            args[i].high1 = high1;
        }
        if (pthread_create(&tid[i], NULL, thread_func2, &args[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    for (int i=0; i<numThreads; i++) {
        if (pthread_join(tid[i], NULL) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv) {
  
  int rc = user_main(argc, argv);
 

  return rc;
}

#define main user_main


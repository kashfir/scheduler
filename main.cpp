#include <iostream>
#include <time.h>
#include <thread>
using namespace std;
#define MAX_TASKS 5
#define NSEC_PER_SEC 1000000000L

typedef struct {
  int period;
  int elapsed;
  int ready;
  int enabled;
  void (*task)(void);
} task_t;

task_t tasks[MAX_TASKS];

void tick_timer_intr(void){
  static int i;
  for (i=0 ; i<MAX_TASKS ; i++){
    if ((tasks[i].task != NULL) && (tasks[i].enabled == 1) && (tasks[i].elapsed != 0)){
      tasks[i].elapsed--;
      if (tasks[i].elapsed == 0){
        tasks[i].ready = 1;
        tasks[i].elapsed = tasks[i].period;
      }
    }
  }
}

void interrupt(){
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC ,&t);
   while(1)
   {
     clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
     tick_timer_intr();
     t.tv_nsec += 1000000L; //1ms
       if (t.tv_nsec>(NSEC_PER_SEC-1)) { // avoid overflow
         t.tv_sec+= 1;
         t.tv_nsec-=NSEC_PER_SEC;
       }
    }
}

void Run_RTC_Scheduler(void){
  int i;
  while (1){
    for (i=0 ; i<MAX_TASKS ; i++){
      if((tasks[i].task != NULL) && (tasks[i].enabled == 1) && (tasks[i].ready == 1)){
        tasks[i].task(); //executa tarefa RTC
        tasks[i].ready = 0;
      }
    }
  }
}


int addTask(void (*task)(void), int time, int priority){
  if (priority >= MAX_TASKS || priority < 0) return 0;
  if (tasks[priority].task != NULL) return 0;
  tasks[priority].task = task;
  tasks[priority].ready = 0;
  tasks[priority].elapsed = time;
  tasks[priority].period = time;
  tasks[priority].enabled = 1;
  return 1;
}

void removeTask(void (* task)(void)){
  int i;
  for (i=0 ; i<MAX_TASKS ; i++){
    if (tasks[i].task == task){
      tasks[i].task = NULL;
      tasks[i].elapsed = 0;
      tasks[i].period = 0;
      tasks[i].ready = 0;
      tasks[i].enabled = 0;
      return;
    }
  }
}

void enableTask(int task_number){
  tasks[task_number].enabled = 1;
}
void disableTask(int task_number){
  tasks[task_number].enabled = 0;
}

void rescheduleTask(int task_number, int new_period){
  tasks[task_number].period = new_period;
  tasks[task_number].elapsed = new_period;
}

void task1(){
  std::cout << "[Task1]" << '\n';
}
void task2(){
  std::cout << "[Task2]" << '\n';
}

int main(){
  int i;
  for (i=0 ; i<MAX_TASKS ; i++){
    tasks[i].elapsed = 0;
    tasks[i].ready = 0;
    tasks[i].period = 0;
    tasks[i].enabled = 0;
    tasks[i].task = NULL;
  }

  thread t(&interrupt);
  t.detach();

  if (addTask(task1, 100, 3))
  std::cout << "Tarefa task1 adicionada com sucesso!" << '\n';

  if (addTask(task2, 500, 4))
  std::cout << "Tarefa task2 adicionada com sucesso!" << '\n';


  Run_RTC_Scheduler();
  return 0;
}

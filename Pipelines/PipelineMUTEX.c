#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define N_THREADS 3
#define BUFFER_SIZE 200
#define N_DATA 100000
#define WORKLOAD1 100000
#define WORKLOAD2 50000
#define WORKLOAD3 50000
#define OUTPUT 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
 **
 ** Here, the buffer implementation:
 **
 ******************************************************************************/

typedef struct buffer buffer_t;

struct buffer {
  volatile int head;
  volatile int tail;
  int size;
  volatile int *elems;
};

buffer_t *createBuffer( int size)
{
  buffer_t *buf;

  buf = (buffer_t *)malloc( sizeof(buffer_t));
  buf->head = 0;
  buf->tail = 0;
  buf->size = size+1;
  buf->elems = (int *)malloc( (size+1)*sizeof(int));

  return( buf);
}

int pop( buffer_t* buf, int *data)
{
  int res;

  pthread_mutex_lock(&mutex);

  if(buf->head == buf->tail)
  {
    res = 0;
  }
  else
  {
    *data = buf->elems[buf->head];
    buf->head = (buf->head+1) % buf->size;
    res = 1;
  }

  pthread_mutex_unlock(&mutex);

  return( res);
}


int push( buffer_t* buf, int data)
{
  int nextTail;
  int res;

  pthread_mutex_lock(&mutex);

  nextTail = (buf->tail + 1) % buf->size;
  if(nextTail != buf->head)
  {
    buf->elems[buf->tail] = data;
    buf->tail = nextTail;    
    res = 1;
  }
    else
    {
      res = 0;
    }

    pthread_mutex_unlock(&mutex);

  return( res);
}

/*******************************************************************************
 **
 ** Now, the thread functions for the pipelining:
 **
 ******************************************************************************/

typedef struct threadArgs threadArgs_t;

struct threadArgs {
  int tid;
  buffer_t *in_buf;
  buffer_t *out_buf;
  int workload;
};

int workUnit( int data)
{
  if(data < 0)
    data++;

  return( data);
}

int process( int tid, int data, int  workload)
{
  int i;

#ifdef OUTPUT
    printf( "[%d] processing item %d!\n", tid, data);
#endif

  for( i=0; i<workload; i++)
    data = workUnit( data);

#ifdef OUTPUT
    printf( "[%d] item %d done!\n", tid, data);
#endif

  return( data);
}

void * pipeline( void *arg)
{
  int data;
  int item;
  int workload;
  int suc;
  int tid;

  buffer_t *in;
  buffer_t *out;

  in = ((threadArgs_t *)arg)->in_buf;
  out = ((threadArgs_t *)arg)->out_buf;
  tid = ((threadArgs_t *)arg)->tid;
  workload = ((threadArgs_t *)arg)->workload;
  tid=pthread_self();
  
  if(pop(in,&data) == 0) {
    perror("Failed to pop from the input buffer\n");
    exit(1);
  }
  
  suc=process(tid,data,workload);


  pop(in,&data);
  if(push(out,data) == 0) {
    perror("Failed to push to the output buffer\n");
    exit(1);
  }

  pop(in,&data);
  push(out,data);
  push(out,suc);


//  Here, your input is required :-)

}

/*******************************************************************************
 **
 ** main
 **
 ******************************************************************************/

int main()
{
    int i, suc;
    int data;
    int temp;
    long minLatencyNanoSeconds = 99999;
    long maxLatencyNanoSeconds = -1;
    long avgLatency = 0;
    long msLatency = 0;
    struct timespec startTime, endTime;
    threadArgs_t args[N_THREADS];
    pthread_t         threads[N_THREADS];
    buffer_t *in, *inter1, *inter2, *out;
    
    in = createBuffer( N_DATA+1);
    inter1 = createBuffer( BUFFER_SIZE);
    inter2 = createBuffer( BUFFER_SIZE);
    out = createBuffer( N_DATA+1);

  /**
   *
   * First, we start our threads:
   */
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  args[0].in_buf=in;
  args[0].out_buf=inter1;
  args[1].in_buf=inter1;
  args[1].out_buf=inter2;
  args[2].in_buf=inter2;
  args[2].out_buf=out;
  pthread_create(&threads[0], NULL, pipeline, (void*)&args[0]);
  pthread_create(&threads[1], NULL,pipeline,(void*)&args[1]);
  pthread_create(&threads[2],NULL,pipeline,(void*)&args[2]);

  // Your input please!
  /**
   * Then, we fill the input buffer:
   */
 // data=WORKLOAD3;
  push(in,WORKLOAD1);
  printf("1st workload:%d\n",WORKLOAD1);
  //data=WORKLOAD2;
  push(in,WORKLOAD2);
  printf("2nd workload:%d\n",WORKLOAD2);
  //data=WORKLOAD1;
  push(in,WORKLOAD3);
  printf("3rd workload:%d\n",WORKLOAD3);
 
  
  // Your input please!

  /**
   * Finally, we observe the output in the buffer "out":
   */
  clock_gettime(CLOCK_REALTIME, &startTime);
  pthread_join(threads[0], NULL);
  clock_gettime(CLOCK_REALTIME, &endTime);

  //calculate latency
  msLatency = endTime.tv_nsec - startTime.tv_nsec;


  if(minLatencyNanoSeconds > msLatency)
    minLatencyNanoSeconds = msLatency;
  if(maxLatencyNanoSeconds < msLatency)
    maxLatencyNanoSeconds = msLatency;

    avgLatency += msLatency;

  clock_gettime(CLOCK_REALTIME, &startTime);
  pthread_join(threads[1],NULL);
  clock_gettime(CLOCK_REALTIME, &endTime);

  
  //calculate latency
  msLatency = endTime.tv_nsec - startTime.tv_nsec;

  if(minLatencyNanoSeconds > msLatency)
    minLatencyNanoSeconds = msLatency;
  if(maxLatencyNanoSeconds < msLatency)
    maxLatencyNanoSeconds = msLatency;

  avgLatency += msLatency;

  clock_gettime(CLOCK_REALTIME, &startTime);
  pthread_join(threads[2],NULL);
  clock_gettime(CLOCK_REALTIME, &endTime);

  
  //calculate latency
  msLatency = endTime.tv_nsec - startTime.tv_nsec;

  if(minLatencyNanoSeconds > msLatency)
    minLatencyNanoSeconds = msLatency;
  if(maxLatencyNanoSeconds < msLatency)
    maxLatencyNanoSeconds = msLatency;

    avgLatency += msLatency;

  int el;
  pop(out, &el);
    printf("Output 1: %d\n", el);
    pop(out, &el);
    printf("Output 2: %d\n", el);
    pop(out, &el);
    printf("Output 3: %d\n", el);

    //for(int t =0; t < 1; t++) {sleep(1);}
    //printf("END: %d, START: %ld\n", endTime.tv_sec, startTime.tv_sec);

    avgLatency = avgLatency / 3;

    //print latency
    printf("Max Latency: %ldns\n", maxLatencyNanoSeconds);
    printf("Min Latency: %ldns\n", minLatencyNanoSeconds);
    printf("Avg. Latency: %ldns\n", avgLatency);
    printf("Throughput: %9.6f\n", (1 / maxLatencyNanoSeconds));


  // Your input please!

  return(0);
}

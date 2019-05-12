 //Matthew St. Louis (mastlouis), Fay Whittall (fxwhittall)
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//a linked list of car threads
typedef struct CarNode_{
  pthread_t* carThread;
  struct CarNode_* next;
}CarNode;

//a struct holding the intersection. It contain a lock used to give/restrict access all other variables
typedef struct Intersection_{
  pthread_mutex_t* lock;
  /*
  * trafficQueues[0] = north
  * trafficQueues[1] = west
  * trafficQueues[2] = south
  * trafficQueues[3] = east
  */
  CarNode** trafficQueues;
  int* isBlocked;
  /*
  * 03
  * 12
  * quadrants[0] = northwest quadrant (0 is free, 1 if taken)
  * quadrants[1] = southwest
  * quadrants[2] = southeast
  * quadrants[3] = northeast
  */
  int quadrants[4];
}Intersection;

//struct to be passed into the drive() function. Allows access to the intersection struct
typedef struct CarAndRoads_{
  int carNum; //number of the car
  pthread_t* carThread;
  Intersection* theIntersection;
}CarAndRoads;

//prints the direction represented by the number
void printDirection(int direction){
  switch(direction){
    case 0: printf("north. ");
    break;
    case 1: printf("west. ");
    break;
    case 2: printf("south. ");
    break;
    case 3: printf("east. ");
    break;
  }
}

/*
* Adds a car to the queue of cars. If the queue is
* currently empty, this will give data to the head
* node's thread field. If the queue is not empty,
* this function will create a new node at the end
* of the queue.
*/
void addToCarQueue(pthread_t* carToAdd, CarNode* headNode){
  CarNode* nodeToAdd = (CarNode*) malloc(sizeof(CarNode));
  CarNode* placeholder = NULL;
  nodeToAdd->carThread = carToAdd;
  nodeToAdd->next = NULL;
  //if the queue is empty, make the upcoming car the head
  if(!headNode->carThread){
    headNode->carThread = carToAdd;
  }
  //if not, add it to the end of the queue
  else{
    placeholder = headNode;
    //While placeholder is not the last car
    while(placeholder->next){
      //Go to the next car
      placeholder = placeholder->next;
    }
    placeholder->next = nodeToAdd;
  }
}

/*
* Consumes a pointer to a the head of a queue of
* car nodes. Returns the pointer to the thread
* held by the head node and overwrites the data
* of the head node with the data of the second node.
* Deletes the second node.
*
* If the queue has at least one thread, returns the
* pointer to the thread at the head of the queue.
* If the queue is empty, returns null.
*/
pthread_t* removeFromCarQueue(CarNode* headNode){
  CarNode* secondNode = NULL;
  pthread_t* headThread = headNode->carThread;
  //headThread may be null if the queue is empty

  if(headNode->next){
    secondNode = headNode->next;
    headNode->carThread = headNode->next->carThread;
    headNode->next = headNode->next->next;
    //If there were only two cars in line, this will
    //Give the head node a null pointer
  }
  else{
    headNode->carThread = NULL;
  }

  if(secondNode)
    free(secondNode);

  return headThread;
}

void drive(CarAndRoads* car){
  Intersection* theRoads = car->theIntersection;
  int* quads = theRoads->quadrants;

  while(1){
    sleep(rand()%10); //if this necessary?
    int direction = rand()%4;
    int turn = rand()%3;
    int isFirst = 0;
    int roadClear = 0;
    int canGo = 0;
    int timeWaited = 0;
    int isBlocker = 0;

    //Atomically add yourself to the queue for your direction
    pthread_mutex_lock(theRoads->lock);
    addToCarQueue(car->carThread, theRoads->trafficQueues[direction]);
    pthread_mutex_unlock(theRoads->lock);

    printf("Car %2d has approached from the ", car->carNum);
    printDirection(direction);
    printf("It is going to ");

    switch(turn){
      case 0: printf("turn to the left due ");
      printDirection((direction+3)%4);
      printf("\n");
      break;
      case 1: printf("go straight due ");
      printDirection((direction+2)%4);
      printf("\n");
      break;
      case 2: printf("turn to the right due ");
      printDirection((direction+1)%4);
      printf("\n");
      break;
    }

    while(!isFirst){
      // printf("\nPointer to the front car of array %d: %p\n", direction, (theRoads->trafficQueues[direction]->carThread));
      // printf("\t\tPointer to this car (car number %d): %p\n",car->carNum, car->carThread);
      if(theRoads->trafficQueues[direction]->carThread == car->carThread){
        isFirst = 1;
      }
      sleep(1);
    }

      //cars that turn right only enter one quadrant
    while(!canGo){
      timeWaited++;
      pthread_mutex_lock(theRoads->lock);
      roadClear = !theRoads->isBlocked[direction];

      //Check the directions you need
      //Purposeful fall-through
      switch(turn){
        /*left turn*/  case(0): roadClear &= !quads[(direction+2)%4];
        /*go straight*/case(1): roadClear &= !quads[(direction + 1)%4];
        /*right Turn*/ case(2): roadClear &= !quads[direction];
      }

      //If the car can make a left turn
      if(roadClear){

        //Claim quadrants
        //Deliberate fall-through
        switch(turn){
          /*left turn*/  case(0): quads[(direction + 2)%4] = 1;
          /*go straight*/case(1): quads[(direction+1) % 4] = 1;
          /*right turn*/ case(2): quads[direction] = 1;
        }

        canGo = 1;
        printf("+++Car %2d has entered the intersection from the ", car->carNum);
        printDirection(direction);
        printf("It is approaching the ");
        printDirection((direction + 3)%4);
        printf("\n");
        removeFromCarQueue(theRoads->trafficQueues[direction]);
      }
      else{
        //If the car has been waiting longer than 4 left turns
        //And if no other car is blocking
        if(timeWaited > 12 
            && !theRoads->isBlocked[0]
            && !theRoads->isBlocked[1]
            && !theRoads->isBlocked[2]
            && !theRoads->isBlocked[3]){
          isBlocker = 1;
          theRoads->isBlocked[(direction+1)%4] = 1;
          theRoads->isBlocked[(direction+2)%4] = 1;
          theRoads->isBlocked[(direction+3)%4] = 1;
        }
      }
      pthread_mutex_unlock(theRoads->lock);

      //Time to wait between check and to wait in the intersection
      if(canGo) sleep((turn + 3)%4);
      else sleep(1);
    }

    //leave the intersection
    pthread_mutex_lock(theRoads->lock);

    //Free quadrants
    //Deliberate fall-through
    switch(turn){
      case(0): quads[(direction + 2)%4] = 0;
      case(1): quads[(direction + 1)%4] = 0;
      case(2): quads[direction] = 0;
    }

    //if they blocked cars, unblock them
    if(isBlocker){
      theRoads->isBlocked[(direction+1)%4] = 0;
      theRoads->isBlocked[(direction+2)%4] = 0;
      theRoads->isBlocked[(direction+3)%4] = 0;
    }
    //pthread_cond_broadcast(theRoads->nextInLine[direction]);
    pthread_mutex_unlock(theRoads->lock);

    printf("---Car %2d has left the intersection. It entered from the ", car->carNum);
    printDirection(direction);
    printf("It left the intersection heading ");
    printDirection((direction + 3)%4);
    printf("\n");
  }
  pthread_exit(NULL);
}

int main(){
  srand(time(0));
  Intersection* theRoads;
  CarAndRoads** traffic;

  //allocate a CarAndRoads for each thread, and the shared struct
  traffic = (CarAndRoads**) calloc(20, sizeof(CarAndRoads*));
  theRoads = (Intersection*) malloc(sizeof(Intersection));

  //allocate the fields in the Intersection struct
  theRoads->lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
  theRoads->trafficQueues = (CarNode**) calloc(4, sizeof(CarNode*));
  theRoads->isBlocked = (int*) calloc(4, sizeof(CarNode*));

  //initialize fields in the Intersection struct
  pthread_mutex_init(theRoads->lock, NULL);
  for(int i = 0; i < 4; i++){
    theRoads->quadrants[i] = 0;
    theRoads->trafficQueues[i] = (CarNode*) malloc(sizeof(CarNode));
    theRoads->trafficQueues[i]->carThread = NULL;
    theRoads->trafficQueues[i]->next = NULL;
  }

  //create all threads and allocate the fields in each of its CarAndRoads struct. Add each CarAndRoads struct to the traffic
  for(int i = 0; i < 20; i++){
    CarAndRoads* car = (CarAndRoads*) malloc(sizeof(CarAndRoads));
    car->carNum = i;
    car->carThread = (pthread_t*) malloc(sizeof(pthread_t));
    car->theIntersection = theRoads;
    traffic[i] = car;
    pthread_create(car->carThread, NULL, (void*) &drive, car);
  }

  //free threads, all allocated variables
  for(int i = 0; i < 20; i++){
    pthread_join(*traffic[i]->carThread, NULL);
    free(traffic[i]->carThread);
    free(traffic[i]);
  }
  for(int i = 0; i < 4; i++){
    free(theRoads->trafficQueues[i]);
  }
  free(theRoads->trafficQueues);
  free(theRoads->lock);
  free(theRoads);
  free(traffic);
  return 0;
}

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>



typedef struct TaskManager
{
    char taskName[50];
    char description[1000];
    char priority[10];
    int taskId;
    bool status; 
} taskManager;




const char* setPriority(char* selectPriority){
    static char* priority[] ={"HIGH", "MEDIUM", "LOW"}; // The static is used to retain the value of priority even after the function execute finish

    while (1)
    {
        printf("How will you prioritize this Task: Choose between HIGH, MEDIUM or LOW: ");
        scanf("%9s", selectPriority); // The %9s ensure at most 9 character are save to the selectPriority to prevent buffer overflow
        getchar();


        for (int i = 0; i < 3; i++)
        {
            if (strcasecmp(priority[i], selectPriority) == 0)
            {
                return priority[i];
            }
            
        }
        printf("Kindly choose between these available options: (HIGH, MEDIUM, or LOW)\n");

    }
    
}




// ***************************************
// THIS SECTION CREATE TASK
void createTask(taskManager **tasks, int* counter, char* taskName, char* description, const char* priority, int taskId, bool status);
// ***************************************


// TODO Creating function to list/ find task
// void listAvailableTask(taskManager task, int counter);





// ***************************************
// THIS SECTION PRINT THE AVAILABLE TASK
static void printTask(taskManager task);
// ***************************************


// TODO Creating function to Update Task
void updateTask(char *name);

// TODO Creating function to Delete Task 
void DeleteTask(char *name);


int main (){
    
    char taskName[50];
    char description[1000];
    char selectPriority[10];
    int taskId;
    int counter = 0;
    bool status; 
    char option[4] = "yes";
    char userOption[4];
    
    taskManager *tasks = NULL;

    printf("Do you want to create task? ");
    scanf("%3s", userOption);
    getchar();
    while (strcasecmp(option, userOption) == 0)
    {
        
        printf("Enter your Task Name: ");
        fgets(taskName, sizeof(taskName), stdin);
        taskName[strcspn(taskName, "\n")] = 0;
    
        printf("How will you describe this task: ");
        fgets(description, sizeof(description), stdin);
        description[strcspn(description, "\n")] = 0;
    
    
        const char* userPriority = setPriority(selectPriority);
    
    
        
           
    
        printf("Enter Task Number: ");
        scanf("%d", &taskId);
    
        status = false;
       
        
        createTask(&tasks, &counter, taskName, description, userPriority, taskId, status);
        
        printf("Do you want to add another? (y/n)");
        scanf(" %3s", userOption);

        
        
    
    
    }

    printf("\n\n");

    for (int i = 0; i < counter; i++)
    {
        /* code */
        printTask(tasks[i]);
    }
    


    free(tasks);
}



static void printTask(taskManager task){
    printf("-----------------------------------------------------------\n");
    printf("\tTask ID: %d\t", task.taskId);
    printf("\tTask Name: %s\n", task.taskName);
    printf("\tTask Description\t :%s\n", task.description);
    printf("\tPriority\t\t :%s\n", (task.priority));
    printf("\tStatus\t\t :%s\n", task.status ? "Completed" : "Not Completed");
    printf("\n------------------------------------------------------\n");
}
void createTask(taskManager **tasks, int* counter, char* taskName, char* description, const char* priority, int taskId, bool status){

    *tasks = realloc(*tasks, (*counter + 1) * sizeof(taskManager));

    if(*tasks == NULL){
        perror("Memmory could not be allocated, Kindly try again");
        exit(1);
    }

    strcpy((*tasks)[*counter].taskName, taskName);
    strcpy((*tasks)[*counter].description, description);
    strcpy((*tasks)[*counter].priority, priority);
    (*tasks)[*counter].taskId = taskId;
    (*tasks)[*counter].status = status;


    (*counter)++;
}


// void listAvailableTask(taskManager task, int counter){
//    for (int i = 0; i < counter; i++)
//    {
//     printTask([i]);
//    }
   
    
// }
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(dir) _mkdir(dir)
    
#else 
    #include <sys/stat.h>
    #include <sys/types.h>

#endif 

#define MAX_TASK_NAME 100
#define MAX_DESCRIPTION 1000
#define MAX_PRIORITY 10
#define TASK_FOLDER "Task_Directory" 
#define FILE_PATH_SIZE 256



typedef struct TaskManager
{
    char taskName[MAX_TASK_NAME];
    char description[MAX_DESCRIPTION];
    char priority[MAX_PRIORITY];
    int taskId;
    bool status; 
    char filePath[FILE_PATH_SIZE];
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
// THIS SECTION CONTAIN TASK CREATION PROTOTYPE
void taskPrototype(taskManager *tasks, char* taskName, char* description, const char* priority, int taskId, bool status);
// ***************************************


// ***************************************
// THIS SECTION CREATE TASK
void createTask();

// ***************************************


// TODO Creating function to list/ find task
// void listAvailableTask(taskManager task, int counter);





// ***************************************
// THIS SECTION PRINT THE AVAILABLE TASK
// static void printTask(taskManager task);
// ***************************************


// TODO Creating function to Update Task
void updateTask(char *name);

// TODO Creating function to Delete Task 
void DeleteTask(char *name);



// *****************************************************************************
// THIS SECTION CREATE DIRECTORY IF NOT EXIST
void createDirectory(){
    int result;

    #ifdef _WIN32
        result = mkdir(TASK_FOLDER);
    #else 
        result = mkdir(TASK_FOLDER, 0700);

    #endif 

    if (result == - 1){
        if (errno == EEXIST){
            printf("This directory already exist!\n"); // To be remove later
            return;
        }else{
            perror("Unable to create this directory!\n");
            exit(1);
        }
        
    }else{
        printf("Directory Created Successfully!\n");
    }

}
// *****************************************************************************

// *****************************************************************************
// THIS SECTION GENERATE VALID FILENAME
void filenameCorrector(char *dest, const char *src, size_t maxLen){
    const char *notAllowedChar = " /\\,|!@#$%^&*()+=<>?";

    size_t i = 0;
    while (src[i] && i < maxLen -1){
        if (strchr(notAllowedChar, src[i])){
            dest[i] = '_';
        } else{
            dest[i] = src[i];
        }
        i++;
    }
    dest[i] = '\0';
    
}
// *****************************************************************************

// CREATING FILE PATH
void createFilePath(taskManager *tasks, const char *directory, char *extension){
    char filename[MAX_TASK_NAME];
    filenameCorrector(filename, tasks->taskName, sizeof(filename));

    snprintf(tasks->filePath, sizeof(tasks->filePath), "%s/%s%s", directory, filename, extension);
}

// *****************************************************************************
// THIS SECTION SAVE THE TASK CREATED TO FILE
void saveNewTask(taskManager *tasks){
    createDirectory();
    createFilePath(tasks, TASK_FOLDER, ".txt");

    FILE *file = fopen(tasks->filePath, "r");
    if(file != NULL){
        fclose(file);
        printf("Sorry %s already available in this directory!\n", tasks->taskName);
        return;


    }else{
        file = fopen(tasks->filePath, "w");
        if (file == NULL){
            perror("File creation error!\n");
            return;
        }
        
        fprintf(file, "-----------------------------------------------------------\n");
        fprintf(file, "\tTask ID: %d\t", tasks->taskId);
        fprintf(file, "\tTask Name: %s\n", tasks->taskName);
        fprintf(file, "\tTask Description:\t %s\n", tasks->description);
        fprintf(file, "\tPriority:\t\t %s\n", (tasks->priority));
        fprintf(file, "\tStatus:\t\t %s\n", tasks->status ? "Completed" : "Not Completed");
        fprintf(file, "\n------------------------------------------------------\n");
        
        fclose(file);

    }

}
// *****************************************************************************

// Saving the Binary version for the task
void saveBinaryVersion(taskManager *tasks){
    // createDirectory();  
    createFilePath(tasks, TASK_FOLDER, ".bin");

    FILE *file = fopen(tasks->filePath, "rb");
    if (file != NULL){
        fclose(file);
        printf("A name with %s already exist in this directory", tasks->taskName);
        return;

    }else{
        file = fopen(tasks->filePath, "wb");
        if (file == NULL){
            perror("Unable to create this file");
            exit(1);
        }

        fwrite(&tasks->taskId, sizeof(tasks->taskId), 1, file);
        fwrite(tasks->taskName, sizeof(tasks->taskName), 1, file);
        fwrite(tasks->description, sizeof(tasks->description), 1, file);
        fwrite(tasks->priority, strlen(tasks->priority) + 1, 1, file);
        fwrite(&tasks->status, sizeof(tasks->status), 1, file);

        fclose(file);
    }

    

}


// THIS SECTION LOAD THE TASK FROM FILE
int loadSaveTask(taskManager *tasks, const char *filename){
    FILE *file;
    file = fopen(filename, "r");

    if(file == NULL){
        perror("Sorry Cannot open File");
        exit(1);
    }

    char content[1000];
    int count = 0;

    while (fgets(content, sizeof(content), file)){
        if(sscanf(content, "\tTask ID: %d\t \tTask Name: %s\n", &tasks[count].taskId, tasks[count].taskName) == 2){
            
            if (fgets(content, sizeof(content), file)){
                sscanf(content, "\tTask Description:\t %[^\n]", tasks[count].description);

                if (fgets(content, sizeof(content), file)){
                    sscanf(content, "\tPriority:\t\t %s", (tasks[count].priority));
                    
                    if (fgets(content, sizeof(content), file)){
                        sscanf(content, "\tStatus:\t\t %s", tasks[count].status ? "Completed" : "Not Completed");
                    }
                }
                

            }
            fgets(content, sizeof(content), file);
            count++;
        }
    }
    


    fclose(file);
    return count;
}
// ***************************************



int main (){


    
    // char taskName[100];
    // char description[1000];
    // char selectPriority[10];
    // int taskId;
    // int counter = 0;
    // bool status; 
    // char option[4] = "yes";
    // char userOption[4];
    
    int menuOption;




    // taskManager *tasks = NULL;

    
    // printf("Do you want to create task? ");
    // scanf("%3s", userOption);
    // getchar();

    
    while (1)
    {
        printf("What would you like to do: 1 to create Task -1 to quit: ");
        scanf("%d", &menuOption);
        getchar();
        
        if (menuOption <= 0){
            printf("quiting the program");
            break;
        }
        switch (menuOption)
        {
        case 1:
            createTask();
            break;
        case 2:
            break;
        
        default:
            printf("Invalid input");
            break;
        }
    }
    

   

    return 0;
}



void createTask(){

    taskManager tasks;

    char taskName[MAX_TASK_NAME];
    char description[MAX_DESCRIPTION];
    char selectPriority[MAX_PRIORITY];
    int taskId;
    bool status;
    const char option[4] = "yes";
    char userOption[4];
    
    
    do{
        printf("Enter Task Name: ");
        fgets(taskName, sizeof(taskName), stdin);
        taskName[strcspn(taskName, "\n")] = 0;

        printf("\nGive your description for this task: ");
        fgets(description, sizeof(description), stdin);
        description[strcspn(description, "\n")] = 0;

        const char *userPriority = setPriority(selectPriority);

        printf("\nEnter TaskID: ");
        scanf("%d", &taskId);

        status = false;

        taskPrototype(&tasks, taskName, description, userPriority, taskId, status);

        saveNewTask(&tasks);
        saveBinaryVersion(&tasks);
    

        printf("Do you want to add another task? (yes/no)");
        scanf("%3s", userOption);
        getchar();
        
    } while (strcasecmp(option, userOption)== 0);
    

}


// static void printTask(taskManager task){
//     printf("-----------------------------------------------------------\n");
//     printf("\tTask ID: %d\t", task.taskId);
//     printf("\tTask Name: %s\n", task.taskName);
//     printf("\tTask Description\t :%s\n", task.description);
//     printf("\tPriority\t\t :%s\n", (task.priority));
//     printf("\tStatus\t\t :%s\n", task.status ? "Completed" : "Not Completed");
//     printf("\n------------------------------------------------------\n");
// }

void taskPrototype(taskManager *tasks, char* taskName, char* description, const char* priority, int taskId, bool status){


    strcpy(tasks->taskName, taskName);
    strcpy(tasks->description, description);
    strcpy(tasks->priority, priority);
    tasks->taskId = taskId;
    tasks->status = status;

}


// void listAvailableTask(taskManager task, int counter){
//    for (int i = 0; i < counter; i++)
//    {
//     printTask([i]);
//    }
   
    
// }
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>

#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
    #define mkdir(dir) _mkdir(dir)
    
#else 
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>

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
// FUNCTION PROTOTYPES
void createTask();
void listAvailableTask(const char *directory);
void readTask(taskManager *tasks);
void readTaskBinary(taskManager *tasks);
void updateTask(taskManager tasks);
void deleteTask(taskManager task);
// ***************************************


// ****************************************************************************

// ****************************************************************************


void readTaskT(){
    int choice = 0;
    taskManager tasks;
    printf("Where do you want to read from? 1 for Text file, 2 for Binary file: ");
    scanf("%d", &choice);
    getchar();

    switch (choice) {
    case 1:
        readTask(&tasks);
        break;
    case 2:
        readTaskBinary(&tasks);
        break;
    
    default:
        printf("invalid choice");
        return;
    }
}


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

        if(fwrite(tasks, sizeof(taskManager), 1, file) != 1){
            perror("Unable to save task!\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }

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

    taskManager tasks;

    
    printf("Offset of 'priority': %zu bytes\n", offsetof(taskManager, priority));
    int menuOption;

    while (1)
    {
        printf("What would you like to do:\n 1 to create Task,\n 2 to show available task,\n 3 to display Task contents,\n 4 to Update Task\n -1 to quit: ");
        scanf("%d", &menuOption);
        getchar();
        
        if (menuOption <= 0){
            printf("quiting the program\n");
            break;
        }
        switch (menuOption)
        {
        case 1:
            createTask();
            break;

        case 2:
            printf("........Loading Available Task......\n");
            listAvailableTask(TASK_FOLDER);
            break;

        case 3:
            readTaskT();
            break;
        
        case 4:
            updateTask(tasks);
            break;
        
        case 5: 
            deleteTask(tasks);
            break;
        
        default:
            printf("Invalid input\n");
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


void taskPrototype(taskManager *tasks, char* taskName, char* description, const char* priority, int taskId, bool status){


    strcpy(tasks->taskName, taskName);
    strcpy(tasks->description, description);
    strcpy(tasks->priority, priority);
    tasks->taskId = taskId;
    tasks->status = status;

}


void listAvailableTask(const char *directory){

    #ifdef _WIN32
        WIN32_FIND_DATA findFileData;
        HANDLE hFind;
        char path[FILE_PATH_SIZE];

        snprintf(path, sizeof(path), "%s\\*", directory);
        
        hFind = FindFirstFile(path, &findFileData);

        if (hFind == INVALID_HANDLE_VALUE){
            fprintf(stderr, "Error opening directory! %d\n", GetLastError());
            return;
        }

        do{
            if (strcmp(findFileData.cFileName, ".") != 0 &&
                strcmp(findFileData.cFileName, "..") ! = 0){

                if (strstr(findFileData.cfilename, ".txt") || strstr(findFileData.cFileName, ".bin")){
                    
                    printf("%s\n", findFileData.cfileName);
                }
            }
        
        } while (FindNextFile(hFind, &findFileData) != 0);
        
        Findclose(hFind);


    #else
        DIR *dir;
        struct dirent *entry; 
        

        dir = opendir(directory);
        if (dir == NULL){
            perror ("Unable to open this directory:\n");
            exit(EXIT_FAILURE);
        }
        while((entry = readdir(dir)) != NULL){
            if(strstr(entry->d_name, ".txt") || strstr(entry->d_name, ".bin")){
                printf("%s\n", entry->d_name);
            }
        }
        closedir(dir);

    #endif 
   
    
}


void readTask(taskManager *tasks){
    char filename[MAX_TASK_NAME];

    char buffer[1024];
    
    printf("Enter Task Name: ");
    scanf("%[^\n]", tasks->taskName);
    getchar();

    printf("name entered: %s\n", tasks->taskName);


    filenameCorrector(filename, tasks->taskName, sizeof(filename));

    printf("name after name corrector function \"%s\"\n", filename);
    
    snprintf(tasks->filePath, sizeof(tasks->filePath), "%s/%s.txt", TASK_FOLDER, filename);


    FILE *file = fopen(tasks->filePath, "r");
    if (file == NULL){
        perror("Unable to open Task File");
        exit(EXIT_FAILURE);
    }
    
    printf("..........Printing the Text Form...............\n");
    while (fgets(buffer, sizeof(buffer), file)){
        printf("%s", buffer);
    }
    

    fclose(file);
   
    
   
}


void readTaskBinary(taskManager *tasks){
    char filename[MAX_TASK_NAME];

    printf("Enter the Task name: ");
    scanf("%[^\n]", tasks->taskName);
    getchar();


    filenameCorrector(filename, tasks->taskName, sizeof(filename));
    printf("Name after correcting %s\n", filename);

    snprintf(tasks->filePath, sizeof(tasks->filePath), "%s/%s.bin", TASK_FOLDER, filename);


    FILE *file = fopen(tasks->filePath, "rb");
    if (file == NULL){

        perror("Unable to open Task file\n");
        exit(EXIT_FAILURE);

    }

    if (fread(tasks, sizeof(taskManager), 1, file) != 1){
        perror("Unable to open task file\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    printf("-------------Printing the Binary Form--------------\n");

    printf("Task ID: %d\n", tasks->taskId);
    printf("Task Name: %s\n", tasks->taskName);
    printf("Task Description: %s\n", tasks->description);
    printf("Priority: %s\n", tasks->priority);
    printf("Status: %s\n", tasks->status ? "Completed" : "Not Completed");

    fclose(file); 
}

void updateTask(taskManager tasks){
    char filename[MAX_TASK_NAME];
    int option = 0;
    char selectPriority[MAX_PRIORITY];
    const char statusOption[4] = "yes";
    char selectStatusOption[4]; 
    
  

    printf("Enter Task name you want to Update: ");
    scanf("%[^\n]", tasks.taskName);
    getchar();

    printf("What field will like to update? 1 for task priority, 2 for task status\n");
    scanf("%d", &option);
    getchar();

    filenameCorrector(filename, tasks.taskName, sizeof(filename));

    snprintf(tasks.filePath, sizeof(tasks.filePath), "%s/%s.bin", TASK_FOLDER, filename);



    FILE *file = fopen(tasks.filePath, "rb+");
    if (!file){
        perror("Unable to open task file\n");
        exit(EXIT_FAILURE);
    }


    if (option == 1){
        const char *userPriority = setPriority(selectPriority);
        fseek(file, offsetof(taskManager, priority), SEEK_SET);
        fwrite(userPriority, MAX_PRIORITY, 1, file);

        printf("Have you completed this task? ");
        scanf("%3s", selectStatusOption);
        getchar();


        if (strcasecmp(statusOption, selectStatusOption) == 0){
            tasks.status = true;
            fseek(file, offsetof(taskManager, status), SEEK_SET);
            fwrite(&tasks.status, sizeof(bool), 1, file);
        }


    } else if (option == 2){
        tasks.status = true;
        fseek(file, offsetof(taskManager, status), SEEK_SET);
        fwrite(&tasks.status, sizeof(bool), 1, file);
            
    }else{
        
        printf("You must select from the option listed not %d", option);

    }

    fclose(file);
}

void deleteTask(taskManager tasks){
    char filename[MAX_TASK_NAME];

    printf("What task would you like to remove? ");
    scanf("%[^\n]", tasks.taskName);
    getchar();


    filenameCorrector(filename, tasks.taskName, sizeof(filename));

    snprintf(tasks.filePath, sizeof(tasks.filePath), "%s/%s.bin", TASK_FOLDER, filename);

    if (remove(tasks.filePath) != 0){
        perror("Unable to delete task\n");
        exit(EXIT_FAILURE);

    }else{
        printf("Task deleted successfuly!\n");
    }
    
}
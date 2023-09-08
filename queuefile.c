#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "queuefile.h"

typedef struct FileNode {
    char filename[100]; // Adjust the size as needed
    struct FileNode* next;
} FileNode;

typedef struct FileQueue {
    int cantfiles;
    FileNode* front;
    FileNode* rear;
} FileQueue;

Fqueue newQueue() {
    Fqueue queue;
    if((queue=malloc(sizeof(FileQueue)))==NULL){
        perror("malloc machine broke \n");
        exit(1);
    }
    queue->cantfiles=0;
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Fqueue queue, const char* path) {
    if (queue == NULL || path == NULL )
        return;
    struct dirent* entry;
    DIR* dir = opendir(path);
    if (dir == NULL) {
        FileNode* newNode = malloc(sizeof(FileNode));
        if (newNode == NULL) {
            perror("malloc machine broke\n");
            exit(EXIT_FAILURE);
        }
        strcpy(newNode->filename, path);
        newNode->next = NULL;

        if (queue->rear == NULL)
            queue->front = queue->rear = newNode;
        else {
            queue->rear->next = newNode;
            queue->rear = newNode;
        }
        queue->cantfiles++;
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char filePath[512]; // Adjust the size as needed
        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);
        enqueue(queue, filePath);
    }
    closedir(dir);
}


void dequeue(Fqueue queue,char* buff) {
    if (queue->front == NULL){
        buff=NULL;
        return;
    }
    FileNode* temp = queue->front;
    strcpy(buff,temp->filename);

    queue->front = queue->front->next;
    free(temp);
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

}



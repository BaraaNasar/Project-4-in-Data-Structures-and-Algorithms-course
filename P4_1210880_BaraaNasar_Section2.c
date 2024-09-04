//COMP2421 - Data Structures and Algorithms
//Second Semester - Spring 2023/2024
//Project#4
//Baraa Fahmi Sabri Nasar - 1210880 - Section.2


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100
#define INF INT_MAX


int graph[MAX][MAX];
char buildings[MAX][20];
int buildingCount = 0;

typedef struct Node {
    int courseIndex;
    struct Node* next;
} Node;


typedef struct MinHeapNode {
    int v;
    int dist;
} MinHeapNode;

typedef struct MinHeap {
    int size;
    int capacity;
    int *pos;
    MinHeapNode **array;
} MinHeap;


Node* adjList[MAX];
char courses[MAX][20];
int courseCount = 0;
int indegree[MAX];



void loadBuildings();
void loadCourses();
int getBuildingIndex(char *name);
int getCourseIndex(char *course);
MinHeapNode* newMinHeapNode(int vertex, int distance);
MinHeap* createMinHeap(int capacity);
void swapMinHeapNode(MinHeapNode** x, MinHeapNode** y);
void minHeapify(MinHeap* minHeap, int index);
int isEmpty(MinHeap* minHeap);
MinHeapNode* extractMin(MinHeap* minHeap);
void decreaseKey(MinHeap* minHeap, int vertex, int distance);
int isInMinHeap(MinHeap *minHeap, int vertex);
void printPath(int parent[], int j);
void dijkstra(int src, int dest);
void addEdge(int from, int to);
void topologicalSort(int sortedCourses[], int *sortedIndex);
void printSortedCourses(int sortedCourses[], int sortedIndex);


int main() {
    int sortedCoursesGlobal[MAX];
    int sortedIndexGlobal = 0;
    printf("\n\nWelcome!!!!\n");

    int choice;
    while (1) {
        printf("\n-----------------------------------\n");
        printf("\nMenu:\n");
        printf("1. Load the two files.\n");
        printf("2. Calculate the shortest distance between two buildings.\n");
        printf("3. Print the shortest route between two buildings and the total distance.\n");
        printf("4. Sort the courses using topological sort.\n");
        printf("5. Print the sorted courses.\n");
        printf("6. Exit the application.\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\n-----------------------------------\n");

/////////////////////////////////////////////////////////////////////

        if (choice == 1) {
            loadBuildings();
            loadCourses();
            printf("\nDone..\n");
        }

/////////////////////////////////////////////////////////////////////


       // Handle the choice for calculating the shortest distance between two buildings
        else if (choice == 2) {
            char start[20], end[20];
            printf("\nEnter start building: ");
            scanf("%s", start);
            printf("\nEnter end building: ");
            scanf("%s", end);
            int startIdx = getBuildingIndex(start);
            int endIdx = getBuildingIndex(end);
            dijkstra(startIdx, endIdx);
        }

///////////////////////////////////////////////////////////////////////////

        // Handle the choice for printing the shortest route between two buildings and the total distance
        else if (choice == 3) {

            char start[20], end[20];
            printf("\nEnter start building: ");
            scanf("%s", start);
            printf("\nEnter end building: ");
            scanf("%s", end);
            int startIdx = getBuildingIndex(start);
            int endIdx = getBuildingIndex(end);
            if (startIdx >= buildingCount || endIdx >= buildingCount) {
                printf("\nOne or both of the entered buildings do not exist.\n");
                continue; // Go back to the menu
            }
            dijkstra(startIdx, endIdx);
        }



//////////////////////////////////////////////////////////////////////////


        // Handle the choice for sorting the courses using topological sort
        else if (choice == 4) {
            int sortedCourses[MAX];
            int sortedIndex;
            topologicalSort(sortedCourses, &sortedIndex);
            sortedIndexGlobal = sortedIndex;
            for (int i = 0; i < sortedIndex; i++) {
                sortedCoursesGlobal[i] = sortedCourses[i];
            }
            printf("\nCourses sorted.\n");
        }


////////////////////////////////////////////////////////////////////////////


        // Handle the choice for printing the sorted courses
        else if (choice == 5) {
            printSortedCourses(sortedCoursesGlobal, sortedIndexGlobal);
        }


//////////////////////////////////////////////////////////////////////////

        else if (choice == 6) {
            printf("\nEnd...\n");
            break;
        }

//////////////////////////////////////////////////////////////////////////

        else {
             printf("\nInvalid choice. Please enter a number between 1 and 6.\n");
        }
    }
    return 0;
}

// Function to load building data from the file
void loadBuildings() {
    FILE *file = fopen("input_buildings.txt", "r");
    if (file == NULL) {
        printf("Error: Unable to open input_buildings.txt\n");
        return;
    }
    char line[100];
    char building1[20], building2[20];
    int distance;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^#]#%[^#]#%d", building1, building2, &distance);
        int index1 = getBuildingIndex(building1);
        int index2 = getBuildingIndex(building2);
        graph[index1][index2] = distance;
        graph[index2][index1] = distance;
    }
    fclose(file);
}


// Function to load course data from the file
void loadCourses() {
    FILE *file = fopen("input_courses.txt", "r");
    if (file == NULL) {
        printf("\nError: Unable to open input_courses.txt\n");
        return;
    }
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, "#\n");
        int courseIndex = getCourseIndex(token);
        while ((token = strtok(NULL, "#\n")) != NULL) {
            int prereqIndex = getCourseIndex(token);
            addEdge(prereqIndex, courseIndex);
            indegree[courseIndex]++;
        }
    }
    fclose(file);
}


// Function to get the index of a building, adding it if it doesn't exist
int getBuildingIndex(char *name) {
    char lowercaseName[20];
    strcpy(lowercaseName, name);
    for (int i = 0; lowercaseName[i]; i++) {
        lowercaseName[i] = tolower(lowercaseName[i]);
    }
    for (int i = 0; i < buildingCount; i++) {
        char lowercaseBuilding[20];
        strcpy(lowercaseBuilding, buildings[i]);
        for (int j = 0; lowercaseBuilding[j]; j++) {
            lowercaseBuilding[j] = tolower(lowercaseBuilding[j]);
        }
        if (strcasecmp(lowercaseBuilding, lowercaseName) == 0) {
            return i;
        }
    }
    strcpy(buildings[buildingCount], name);
    return buildingCount++;
}


// Function to get the index of a course, adding it if it doesn't exist
int getCourseIndex(char *course) {
    char lowercaseCourse[20];
    strcpy(lowercaseCourse, course);
    for (int i = 0; lowercaseCourse[i]; i++) {
        lowercaseCourse[i] = tolower(lowercaseCourse[i]);
    }
    for (int i = 0; i < courseCount; i++) {
        char lowercaseCourseInDatabase[20];
        strcpy(lowercaseCourseInDatabase, courses[i]);
        for (int j = 0; lowercaseCourseInDatabase[j]; j++) {
            lowercaseCourseInDatabase[j] = tolower(lowercaseCourseInDatabase[j]);
        }
        if (strcasecmp(lowercaseCourseInDatabase, lowercaseCourse) == 0) {
            return i;
        }
    }
    strcpy(courses[courseCount], course);
    return courseCount++;
}


// Function to create a new MinHeapNode with given vertex and distance
MinHeapNode* newMinHeapNode(int vertex, int distance) {
    MinHeapNode* minHeapNode = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    minHeapNode->v = vertex;
    minHeapNode->dist = distance;
    return minHeapNode;
}


// Function to create a new MinHeap with the given capacity
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->pos = (int*)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}


// Function to swap two MinHeapNode pointers
void swapMinHeapNode(MinHeapNode** x, MinHeapNode** y) {
    MinHeapNode* temp = *x;
    *x = *y;
    *y = temp;
}


// Function to maintain the Min Heap property starting from the given index
void minHeapify(MinHeap* minHeap, int index) {
    int smallest, left, right;
    smallest = index;
    left = 2 * index + 1;
    right = 2 * index + 2;
    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;
    if (smallest != index) {
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* indexNode = minHeap->array[index];
        minHeap->pos[smallestNode->v] = index;
        minHeap->pos[indexNode->v] = smallest;
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[index]);
        minHeapify(minHeap, smallest);
    }
}


// Function to check if the Min Heap is empty
int isEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}


// Function to extract the minimum node from the Min Heap
MinHeapNode* extractMin(MinHeap* minHeap) {
    if (isEmpty(minHeap))
        return NULL;
    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;
    --minHeap->size;
    minHeapify(minHeap, 0);
    return root;
}


// Function to decrease the key value of a vertex in the Min Heap
void decreaseKey(MinHeap* minHeap, int vertex, int distance) {
    int index = minHeap->pos[vertex];
    minHeap->array[index]->dist = distance;
    while (index && minHeap->array[index]->dist < minHeap->array[(index - 1) / 2]->dist) {
        minHeap->pos[minHeap->array[index]->v] = (index - 1) / 2;
        minHeap->pos[minHeap->array[(index - 1) / 2]->v] = index;
        swapMinHeapNode(&minHeap->array[index], &minHeap->array[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}


// Function to check if a vertex is in the Min Heap
int isInMinHeap(MinHeap *minHeap, int vertex) {
    return minHeap->pos[vertex] < minHeap->size;
}


// Function to print the shortest path from the source to the destination node
void printPath(int parent[], int currentNodeIndex) {
    if (parent[currentNodeIndex] == -1)
        return;
    printPath(parent, parent[currentNodeIndex]);
    printf(" -> %s", buildings[currentNodeIndex]);
}


// Function to find the shortest path between two buildings using Dijkstra's algorithm
void dijkstra(int sourceIndex, int destinationIndex) {
    if (sourceIndex >= buildingCount || destinationIndex >= buildingCount) {
        printf("\nOne or both of the entered buildings do not exist.\n");
        return;
    }
    int distances[MAX];
    int parents[MAX];
    MinHeap* minHeap = createMinHeap(buildingCount);
    for (int vertex = 0; vertex < buildingCount; ++vertex) {
        distances[vertex] = INF;
        parents[vertex] = -1;
        minHeap->array[vertex] = newMinHeapNode(vertex, distances[vertex]);
        minHeap->pos[vertex] = vertex;
    }
    minHeap->array[sourceIndex] = newMinHeapNode(sourceIndex, distances[sourceIndex]);
    minHeap->pos[sourceIndex] = sourceIndex;
    distances[sourceIndex] = 0;
    decreaseKey(minHeap, sourceIndex, distances[sourceIndex]);
    minHeap->size = buildingCount;
    while (!isEmpty(minHeap)) {
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int currentVertex = minHeapNode->v;
        for (int adjacentVertex = 0; adjacentVertex < buildingCount; ++adjacentVertex) {
            if (graph[currentVertex][adjacentVertex] && isInMinHeap(minHeap, adjacentVertex) && distances[currentVertex] != INF &&
                distances[currentVertex] + graph[currentVertex][adjacentVertex] < distances[adjacentVertex]) {
                distances[adjacentVertex] = distances[currentVertex] + graph[currentVertex][adjacentVertex];
                parents[adjacentVertex] = currentVertex;
                decreaseKey(minHeap, adjacentVertex, distances[adjacentVertex]);
            }
        }
    }
    if (distances[destinationIndex] == INF) {
        printf("\nThere is no path between the entered buildings.\n");
        return;
    }

    // Print the shortest distance and path
    printf("\nShortest distance: %d\n", distances[destinationIndex]);
    printf("\nPath: %s", buildings[sourceIndex]);
    printPath(parents, destinationIndex);
    printf("\n");
}


// Function to add a directed edge from one course to another in the adjacency list
void addEdge(int fromCourse, int toCourse) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->courseIndex = toCourse;
    newNode->next = adjList[fromCourse];
    adjList[fromCourse] = newNode;
}


// Function to perform topological sort on the courses
void topologicalSort(int sortedCourses[], int *sortedIndex) {
    int queue[MAX], front = 0, rear = 0;
    for (int i = 0; i < courseCount; i++) {
        if (indegree[i] == 0) {
            queue[rear++] = i;
        }
    }
    *sortedIndex = 0;
    while (front < rear) {
        int currentCourse = queue[front++];
        sortedCourses[(*sortedIndex)++] = currentCourse;
        Node* node = adjList[currentCourse];
        while (node != NULL) {
            int adjacentCourse = node->courseIndex;
            indegree[adjacentCourse]--;
            if (indegree[adjacentCourse] == 0) {
                queue[rear++] = adjacentCourse;
            }
            node = node->next;
        }
    }
}


// Function to print the sorted list of courses
void printSortedCourses(int sortedCourses[], int sortedIndex) {

    printf("\nSorted courses:\n\n");
    for (int i = 0; i < sortedIndex; i++) {
        printf("%s\n", courses[sortedCourses[i]]);
    }
}

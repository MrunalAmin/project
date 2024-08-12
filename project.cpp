#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable: 4996)

#define HASH_TABLE_SIZE 127
#define MAX_INPUT_LENGTH 256

typedef struct Parcel {
    char* destination;
    int weight;
    float value;
} Parcel;

typedef struct Node {
    Parcel parcel;
    struct Node* left;
    struct Node* right;
} Node;

Node* hashTable[HASH_TABLE_SIZE];

Node* createNode(Parcel parcel) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode) {
        newNode->parcel.destination = (char*)malloc(strlen(parcel.destination) + 1);
        if (newNode->parcel.destination) {
            strcpy(newNode->parcel.destination, parcel.destination);
        }
        else {
            free(newNode);
            return NULL;
        }
        newNode->parcel.weight = parcel.weight;
        newNode->parcel.value = parcel.value;
        newNode->left = newNode->right = NULL;
    }
    return newNode;
}

unsigned long djb2(const char* str) 
{

    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) 
    {

        hash = ((hash << 5) + hash) + c; 

    }
    return hash % HASH_TABLE_SIZE;
}

Node* insertNode(Node* root, Parcel parcel) 
{

    if (!root) 
    {

        return createNode(parcel);

    }
    if (parcel.weight < root->parcel.weight) 
    {

        root->left = insertNode(root->left, parcel);

    }
    else if (parcel.weight > root->parcel.weight) 
    { 

        root->right = insertNode(root->right, parcel);

    }
    return root;
}

void loadData(const char* filename) 
{

    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0) 
    {

        printf("Error in opening the file.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) 
    {

        Parcel parcel = { NULL, 0, 0.0f };
        char destinationBuffer[21]; 
        if (sscanf_s(line, "%20[^,], %d, %f", destinationBuffer, (unsigned)sizeof(destinationBuffer), &parcel.weight, &parcel.value) == 3) 
        {

            
            if (parcel.weight < 100 || parcel.weight > 50000) 
            {

                printf("Invalid weight for parcel to %s: %d grams\n", destinationBuffer, parcel.weight);
                continue; 
            }

            
            if (parcel.value < 10.0f || parcel.value > 2000.0f) 
            {

                printf("Invalid valuation for parcel to %s: $%.2f\n", destinationBuffer, parcel.value);
                continue; 
            }

            
            parcel.destination = (char*)malloc(strlen(destinationBuffer) + 1);
            if (parcel.destination) 
            {

                strcpy(parcel.destination, destinationBuffer);
                
                for (char* p = parcel.destination; *p; ++p) *p = tolower(*p);

                unsigned long index = djb2(parcel.destination);
                hashTable[index] = insertNode(hashTable[index], parcel);
            }
            else 
            {

                printf("Mwmory  not located for destination.\n");
            }
        }
        else 
        {

            printf("Invalid line format: %s", line);
        }
    }
    fclose(file);
}

void freeTree(Node* root) 
{

    if (root) 
    {
        
        freeTree(root->left);
        freeTree(root->right);
        free(root->parcel.destination); 
        free(root);
    }
}

void displayParcels(Node* root, const char* country) {
    if (root) {
        displayParcels(root->left, country);
        if (strcmp(root->parcel.destination, country) == 0) {
            printf("Destination: %s, Weight: %d, Value: $%.2f\n", root->parcel.destination, root->parcel.weight, root->parcel.value);
        }
        displayParcels(root->right, country);
    }
}

void displayParcelsByWeight(Node* root, const char* country, int weight, int greater) {
    if (root) {
        displayParcelsByWeight(root->left, country, weight, greater);
        if (strcmp(root->parcel.destination, country) == 0 &&
            ((greater && root->parcel.weight > weight) || (!greater && root->parcel.weight < weight))) {
            printf("Destination: %s, Weight: %d, Value: $%.2f\n", root->parcel.destination, root->parcel.weight, root->parcel.value);
        }
        displayParcelsByWeight(root->right, country, weight, greater);
    }
}

void calculateTotalLoadAndValuation(Node* root, const char* country, int* totalWeight, float* totalValue)
{

    if (root) 
    {

        if (strcmp(root->parcel.destination, country) == 0) 
        {

            *totalWeight += root->parcel.weight;
            *totalValue += root->parcel.value;
        }
        calculateTotalLoadAndValuation(root->left, country, totalWeight, totalValue);
        calculateTotalLoadAndValuation(root->right, country, totalWeight, totalValue);
    }
}


void findCheapestAndMostExpensive(Node* root, const char* country, Parcel* cheapest, Parcel* mostExpensive) 
{

    if (root) 
    {

        if (strcmp(root->parcel.destination, country) == 0) 
        {

            if (root->parcel.value < cheapest->value) 
            {

                *cheapest = root->parcel;
            }
            if (root->parcel.value > mostExpensive->value) 
            {

                *mostExpensive = root->parcel;
            }
        }
        findCheapestAndMostExpensive(root->left, country, cheapest, mostExpensive);
        findCheapestAndMostExpensive(root->right, country, cheapest, mostExpensive);
    }
}

void findLightestAndHeaviest(Node* root, const char* country, Parcel* lightest, Parcel* heaviest) 
{

    if (root) 
    {

        if (strcmp(root->parcel.destination, country) == 0) 
        {

            if (root->parcel.weight < lightest->weight) 
            {

                *lightest = root->parcel;
            }
            if (root->parcel.weight > heaviest->weight) 
            {

                *heaviest = root->parcel;
            }
        }
        findLightestAndHeaviest(root->left, country, lightest, heaviest);
        findLightestAndHeaviest(root->right, country, lightest, heaviest);
    }
}
void displayMenu() {
    printf("Menu:\n");
    printf("1. Enter country name and display all parcels\n");
    printf("2. Enter country and weight to display parcels\n");
    printf("3. Display total parcel load and valuation for the country\n");
    printf("4. Display cheapest and most expensive parcel\n");
    printf("5. Display lightest and heaviest parcel\n");
    printf("6. Exit\n");
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
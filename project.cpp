#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable: 4996)

#define HASH_TABLE_SIZE 127

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
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
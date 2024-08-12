/*
 * FILE: project.cpp
 * PROJECT: project
 * PROGRAMMER: Mrunal Jayeshbhai Amin, Manan Kishorbhai Dholiya
 *  Student ID: 8961581, 8910311
 * FIRST VERSION: 12-08-2024
 * DESCRIPTION:This program uses a hash table and binary search trees to maintain parcel information.
 *             It enables users to input parcel data from a file, show parcels by nation, filter parcels by weight,
 *             calculate total load and valuation, and determine the cheapest, most costly, lightest,
 *             and heaviest packages for a certain country.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma warning(disable: 4996)

// Define constants
#define HASH_TABLE_SIZE 127
#define MAX_INPUT_LENGTH 256

typedef struct Parcel 
{
    // Use dynamic allocation for destination
    char* destination;
    int weight;
    float value;
} Parcel;

// Node structure for BST
typedef struct Node 
{
    Parcel parcel;
    struct Node* left;
    struct Node* right;
} Node;

// Hash Table
Node* hashTable[HASH_TABLE_SIZE];

// Function prototypes
Node* createNode(Parcel parcel);
unsigned long djb2(const char* str);
Node* insertNode(Node* root, Parcel parcel);
void loadData(const char* filename);
void freeTree(Node* root);
void displayParcels(Node* root, const char* country);
void displayParcelsByWeight(Node* root, const char* country, int weight, int greater);
void calculateTotalLoadAndValuation(Node* root, const char* country, int* totalWeight, float* totalValue);
void findCheapestAndMostExpensive(Node* root, const char* country, Parcel* cheapest, Parcel* mostExpensive);
void findLightestAndHeaviest(Node* root, const char* country, Parcel* lightest, Parcel* heaviest);
void displayMenu();
void clearInputBuffer();
void menuAction();


int main() {
    // Intialize hash table
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        hashTable[i] = NULL;
    }
    // Load data from file
    loadData("couriers.txt");
    // Perform menu actions
    menuAction();

    return 0;
}

/*
 * Function: createNode
 *
 * Description: Creates a new node with the specified parcel data.
 *
 * Parameters: Parcel parcel : The parcel data to be stored in the node.
 *
 * Returns: Node* : A pointer to the newly created node, or NULL if memory allocation fails.
 */
Node* createNode(Parcel parcel) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode) {
        newNode->parcel.destination = (char*)malloc(strlen(parcel.destination) + 1);
        if (newNode->parcel.destination) {
            strcpy(newNode->parcel.destination, parcel.destination);
        }
        else {
            // Handle memory allocation failure for destination
            free(newNode);
            return NULL;
        }
        newNode->parcel.weight = parcel.weight;
        newNode->parcel.value = parcel.value;
        newNode->left = newNode->right = NULL;
    }
    return newNode;
}

/*
 * Function: djb2
 *
 * Description: Computes a hash value for a given string using the djb2 algorithm.
 *
 * Parameters: const char* str : The string to be hashed.
 *
 * Returns: unsigned long : The hash value of the string modulo HASH_TABLE_SIZE.
 */
unsigned long djb2(const char* str) 
{

    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) 
    {

        hash = ((hash << 5) + hash) + c; // Hash * 33 + c

    }
    return hash % HASH_TABLE_SIZE;
}

/*
 * Function: insertNode
 *
 * Description: Inserts a parcel into the binary search tree (BST) based on its weight.
 *
 * Parameters: Node* root : The root of the BST.
 *             Parcel parcel : The parcel data to be inserted.
 *
 * Returns: Node* : The root of the BST after insertion.
 */
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
    // Ensure unique weight for the same destination
    else if (parcel.weight > root->parcel.weight) 
    { 

        root->right = insertNode(root->right, parcel);

    }
    return root;
}

/*
 * Function: loadData
 *
 * Description: Loads parcel data from a specified file and inserts it into the hash table.
 *
 * Parameters: const char* filename : The name of the file to load data from.
 *
 * Returns: void
 */
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

            // Validate weight
            if (parcel.weight < 100 || parcel.weight > 50000) 
            {

                printf("Invalid weight for parcel to %s: %d grams\n", destinationBuffer, parcel.weight);                
                continue; // Skip this parcel
            }

            // Validate Valuation
            if (parcel.value < 10.0f || parcel.value > 2000.0f) 
            {

                printf("Invalid valuation for parcel to %s: $%.2f\n", destinationBuffer, parcel.value);
                continue; 
            }

            // Allocated memory for destination dynamically
            parcel.destination = (char*)malloc(strlen(destinationBuffer) + 1);
            if (parcel.destination) 
            {

                strcpy(parcel.destination, destinationBuffer);
                
                // Covert destination to lowercase for constistent hashing
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

/*
 * Function: freeTree
 *
 * Description: Frees the memory allocated for the nodes in a binary search tree (BST).
 *
 * Parameters: Node* root : The root of the BST.
 *
 * Returns: void
 */
void freeTree(Node* root) 
{

    if (root) 
    {
        
        freeTree(root->left);
        freeTree(root->right);
        // Free dynamically allocated destination
        free(root->parcel.destination); 
        free(root);
    }
}

/*
 * Function: displayParcels
 *
 * Description: Displays all parcels with a specified destination from a binary search tree (BST).
 *
 * Parameters: Node* root : The root of the BST.
 *             const char* country : The destination country to filter parcels by.
 *
 * Returns: void
 */
void displayParcels(Node* root, const char* country) {
    if (root) {
        displayParcels(root->left, country);
        if (strcmp(root->parcel.destination, country) == 0) // Compare string to ensure user match
        {
            printf("Destination: %s, Weight: %d, Value: $%.2f\n", root->parcel.destination, root->parcel.weight, root->parcel.value);
        }
        displayParcels(root->right, country);
    }
}

/*
 * Function: displayParcelsByWeight
 *
 * Description: Displays parcels from a BST for a specific country with weight criteria.
 *
 * Parameters: Node* root : The root of the BST.
 *             const char* country : The destination country to filter parcels by.
 *             int weight : The weight threshold.
 *             int greater : If 1, display parcels with weight greater than the threshold. Otherwise, display parcels with weight less.
 *
 * Returns: void
 */
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

/*
 * Function: calculateTotalLoadAndValuation
 *
 * Description: Calculates the total weight and value of parcels for a specified country.
 *
 * Parameters: Node* root : The root of the BST.
 *             const char* country : The destination country to filter parcels by.
 *             int* totalWeight : Pointer to store the total weight.
 *             float* totalValue : Pointer to store the total value.
 *
 * Returns: void
 */
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

/*
 * Function: findCheapestAndMostExpensive
 *
 * Description: Finds the cheapest and most expensive parcels for a specific country.
 *
 * Parameters: Node* root : The root of the BST.
 *             const char* country : The destination country to filter parcels by.
 *             Parcel* cheapest : Pointer to store the cheapest parcel.
 *             Parcel* mostExpensive : Pointer to store the most expensive parcel.
 *
 * Returns: void
 */
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

/*
 * Function: findLightestAndHeaviest
 *
 * Description: Finds the lightest and heaviest parcels for a specific country.
 *
 * Parameters: Node* root : The root of the BST.
 *             const char* country : The destination country to filter parcels by.
 *             Parcel* lightest : Pointer to store the lightest parcel.
 *             Parcel* heaviest : Pointer to store the heaviest parcel.
 *
 * Returns: void
 */
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

/*
 * Function: displayMenu
 *
 * Description: Displays the menu options to the user.
 *
 * Parameters: void
 *
 * Returns: void
 */
void displayMenu() {
    printf("Menu:\n");
    printf("1. Enter country name and display all parcels\n");
    printf("2. Enter country and weight to display parcels\n");
    printf("3. Display total parcel load and valuation for the country\n");
    printf("4. Display cheapest and most expensive parcel\n");
    printf("5. Display lightest and heaviest parcel\n");
    printf("6. Exit\n");
}

/*
 * Function: clearInputBuffer
 *
 * Description: Clears the input buffer to handle any leftover input characters.
 *
 * Parameters: void
 *
 * Returns: void
 */
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * Function: menuAction
 *
 * Description: Executes user-selected menu actions, managing parcel display and analysis.
 *
 * Parameters: void
 *
 * Returns: void
 */
void menuAction() {
    int choice;
    char country[MAX_INPUT_LENGTH];
    int weight;

    while (1) {
        displayMenu();
        printf("Enter choice: ");
        // Check if the input was successful
        if (scanf_s("%d", &choice) != 1) {
            printf("Invalid input. Please enter a valid choice.\n");
            clearInputBuffer();
            continue;
        }
        // Clear any remaining new line character
        clearInputBuffer();

        switch (choice) {
        case 1:
            printf("Enter country name: ");
            if (fgets(country, sizeof(country), stdin) == NULL) {
                printf("Error reading country name.\n");
                break;
            }
            // Remove new line character
            country[strcspn(country, "\n")] = 0;
            // Convert input to lowercase for consistent hashing
            for (char* p = country; *p; ++p) *p = tolower(*p);
            {
                unsigned long index = djb2(country);
                if (hashTable[index]) {
                    printf("Parcels for %s:\n", country);
                    displayParcels(hashTable[index], country);
                }
                else {
                    printf("No parcels found for %s.\n", country);
                }
            }
            break;
        case 2:
            printf("Enter country name: ");
            if (fgets(country, sizeof(country), stdin) == NULL) {
                printf("Error reading country name.\n");
                break;
            }
            country[strcspn(country, "\n")] = 0;

            printf("Enter weight: ");
            if (scanf_s("%d", &weight) != 1) {
                printf("Invalid weight input.\n");
                clearInputBuffer();
                break;
            }
            clearInputBuffer();

            printf("Select (1) for greater and (2) for less than %d grams:", weight);
            int comparisonChoice;
            if (scanf_s("%d", &comparisonChoice) != 1) {
                printf("Invalid input.\n");
                clearInputBuffer();
                break;
            }
            clearInputBuffer();
            for (char* p = country; *p; ++p) *p = tolower(*p);
            {
                unsigned long index = djb2(country);
                if (hashTable[index]) {
                    if (comparisonChoice == 1) {
                        printf("Parcels for %s with weight greater than %d:\n", country, weight);
                        displayParcelsByWeight(hashTable[index], country, weight, 1);
                    }
                    else if (comparisonChoice == 2) {
                        printf("Parcels for %s with weight less than %d:\n", country, weight);
                        displayParcelsByWeight(hashTable[index], country, weight, 0);
                    }
                    else {
                        printf("Invalid choice. Please enter 1 for greater or 2 for less.\n");
                    }
                }
                else {
                    printf("No parcels found for %s.\n", country);
                }
            }
            break;
        case 3:
            printf("Enter country name: ");
            if (fgets(country, sizeof(country), stdin) == NULL) {
                printf("Error reading country name.\n");
                break;
            }
            country[strcspn(country, "\n")] = 0;
            for (char* p = country; *p; ++p) *p = tolower(*p);
            {
                unsigned long index = djb2(country);
                if (hashTable[index]) {
                    int totalWeight = 0;
                    float totalValue = 0;
                    calculateTotalLoadAndValuation(hashTable[index], country, &totalWeight, &totalValue);
                    printf("Total load for %s: %d g\n", country, totalWeight);
                    printf("Total valuation for %s: $%.2f\n", country, totalValue);
                }
                else {
                    printf("No parcels found for %s.\n", country);
                }
            }
            break;
        case 4:
            printf("Enter country name: ");
            if (fgets(country, sizeof(country), stdin) == NULL) {
                printf("Error reading country name.\n");
                break;
            }
            country[strcspn(country, "\n")] = 0;
            for (char* p = country; *p; ++p) *p = tolower(*p);
            {
                unsigned long index = djb2(country);
                if (hashTable[index]) {
                    // Extreamely big value for intialization
                    Parcel cheapest = { NULL, 0, 999999.0f };
                    // Smallest possible value for the intialization
                    Parcel mostExpensive = { NULL, 0, 0.0f };
                    findCheapestAndMostExpensive(hashTable[index], country, &cheapest, &mostExpensive);
                    printf("Cheapest parcel for %s: Weight: %d, Value: $%.2f\n", country, cheapest.weight, cheapest.value);
                    printf("Most expensive parcel for %s: Weight: %d, Value: $%.2f\n", country, mostExpensive.weight, mostExpensive.value);
                }
                else {
                    printf("No parcels found for %s.\n", country);
                }
            }
            break;
        case 5:
            printf("Enter country name: ");
            if (fgets(country, sizeof(country), stdin) == NULL) {
                printf("Error reading country name.\n");
                break;
            }
            country[strcspn(country, "\n")] = 0;
            for (char* p = country; *p; ++p) *p = tolower(*p);
            {
                unsigned long index = djb2(country);
                if (hashTable[index]) {
                    // Extreamely big value for intialization
                    Parcel lightest = { NULL, 999999, 0.0f };
                    // Smallest possible value for the intialization
                    Parcel heaviest = { NULL, 0, 0.0f };
                    findLightestAndHeaviest(hashTable[index], country, &lightest, &heaviest);
                    printf("Lightest parcel for %s: Weight: %d, Value: $%.2f\n", country, lightest.weight, lightest.value);
                    printf("Heaviest parcel for %s: Weight: %d, Value: $%.2f\n", country, heaviest.weight, heaviest.value);
                }
                else {
                    printf("No parcels found for %s.\n", country);
                }
            }
            break;
        case 6:
            printf("Exiting...\n");
            for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
                freeTree(hashTable[i]);
            }
            return;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }
}

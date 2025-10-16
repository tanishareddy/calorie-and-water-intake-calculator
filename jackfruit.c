#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FOODS 1000
#define MAX_LOG 500

typedef struct 
{
    char name[50];
    float calories_per_100g;
    char category[30];
}Food;

typedef struct 
{
    char name[50];
    float quantity; // in grams
    float calories;
}FoodLog;

Food foodDB[MAX_FOODS];
int foodCount = 0;
FoodLog dailyLog[MAX_LOG];
int logCount = 0;
float dailyWater = 0;

void loadFoodDatabase(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (!file) 
    {
        printf("Error opening file.\n");
        return;
    }

    char line[200];
    fgets(line, sizeof(line), file); // skip header

    while (fgets(line, sizeof(line), file)) 
    {
        char *token = strtok(line, ",");
        if (!token) continue;
        strcpy(foodDB[foodCount].name, token);

        token = strtok(NULL, ",");
        if (!token) continue;
        foodDB[foodCount].calories_per_100g = atof(token);

        token = strtok(NULL, ",\n");
        if (!token) continue;
        strcpy(foodDB[foodCount].category, token);

        foodCount++;
    }

    fclose(file);
}

void listFoodsByCategory(const char *category) 
{
    printf("\nFoods in category '%s':\n", category);
    for (int i = 0; i < foodCount; i++) 
    {
        if (strcmp(foodDB[i].category, category) == 0) 
        {
            printf("%-30s %6.2f kcal/100g\n", foodDB[i].name, foodDB[i].calories_per_100g);
        }
    }
}

void logFoodIntake() 
{
    char name[50];
    float quantity;
    printf("Enter food name: ");
    scanf("%s", name);
    printf("Enter quantity in grams: ");
    scanf("%f", &quantity);

    for (int i = 0; i < foodCount; i++) 
    {
        if (strcmp(foodDB[i].name, name) == 0) 
        {
            strcpy(dailyLog[logCount].name, name);
            dailyLog[logCount].quantity = quantity;
            dailyLog[logCount].calories = (foodDB[i].calories_per_100g * quantity) / 100.0;
            logCount++;
            printf("Logged %s (%0.2f g): %0.2f kcal\n", name, quantity, (foodDB[i].calories_per_100g * quantity) / 100.0);
            return;
        }
    }
    printf("Food not found.\n");
}

void viewDailyLog() {
    float total = 0;
    printf("\nDaily Food Log:\n");
    for (int i = 0; i < logCount; i++) 
    {
        printf("%-30s %6.2f g %6.2f kcal\n", dailyLog[i].name, dailyLog[i].quantity, dailyLog[i].calories);
        total += dailyLog[i].calories;
    }
    printf("Total Calories: %.2f kcal\n", total);
    printf("Total Water Intake: %.2f L\n", dailyWater);
}

void addNewFoodToDatabase(const char *filename) 
{
    Food newFood;
    printf("Enter food name: ");
    scanf("%s", newFood.name);
    printf("Enter calories per 100g: ");
    scanf("%f", &newFood.calories_per_100g);
    printf("Enter category: ");
    scanf("%s", newFood.category);

    FILE *file = fopen(filename, "a");
    if (!file) 
    {
        printf("Error opening file to append.\n");
        return;
    }

    fprintf(file, "%s,%.2f,%s\n", newFood.name, newFood.calories_per_100g, newFood.category);
    fclose(file);
    printf("Food item added successfully. Please restart to reload the database.\n");
}

void logWaterIntake() 
{
    float liters;
    printf("Enter water consumed (in liters): ");
    scanf("%f", &liters);
    dailyWater += liters;
    printf("Logged %.2f L of water. Total today: %.2f L\n", liters, dailyWater);
}

void saveDailyLog(const char *filename) 
{
    FILE *file = fopen(filename, "w");
    if (!file) 
    {
        printf("Error saving log.\n");
        return;
    }

    fprintf(file, "Food,Quantity(g),Calories\n");
    for (int i = 0; i < logCount; i++) 
    {
        fprintf(file, "%s,%.2f,%.2f\n", dailyLog[i].name, dailyLog[i].quantity, dailyLog[i].calories);
    }
    fprintf(file, "Water Intake,%.2f L\n", dailyWater);
    fclose(file);
    printf("Daily log saved to %s\n", filename);
}

void showCalorieRecommendation() 
{
    int age;
    char activity[20];
    float weight;

    printf("Enter your age: ");
    scanf("%d", &age);
    printf("Enter your weight in kg: ");
    scanf("%f", &weight);
    printf("Activity level (low/medium/high): ");
    scanf("%s", activity);

    float base = weight * 22; // Rough BMR estimate

    if (strcmp(activity, "low") == 0) base *= 1.2;
    else if (strcmp(activity, "medium") == 0) base *= 1.55;
    else if (strcmp(activity, "high") == 0) base *= 1.9;

    printf("Recommended daily calorie intake: %.0f kcal\n", base);
    printf("Recommended water intake: %.1f L\n", weight * 0.033);
}

int main() 
{
    const char *filename = "foods_with_categories.csv";
    const char *logFile = "daily_log.csv";
    loadFoodDatabase(filename);

    int choice;
    char category[30];

    do 
    {
        printf("\nCalorie and Water Intake Tracker\n");
        printf("1. View food by category\n");
        printf("2. Log food intake\n");
        printf("3. View daily log\n");
        printf("4. Add new food to database\n");
        printf("5. Log water intake\n");
        printf("6. Save daily log\n");
        printf("7. Calorie & water recommendation\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter category (e.g., Indian, Fruit, Chinese): ");
                scanf("%s", category);
                listFoodsByCategory(category);
                break;
            case 2:
                logFoodIntake();
                break;
            case 3:
                viewDailyLog();
                break;
            case 4:
                addNewFoodToDatabase(filename);
                break;
            case 5:
                logWaterIntake();
                break;
            case 6:
                saveDailyLog(logFile);
                break;
            case 7:
                showCalorieRecommendation();
                break;
            case 8:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } 
    while (choice != 8);

    return 0;
}

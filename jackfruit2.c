#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    float calories_per_100g;
    char category[30];
} food;

food fooddb[1000];
int foodcount = 0;

void loadfooddb(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }

    char line[200];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (!token) continue;
        strcpy(fooddb[foodcount].name, token);

        token = strtok(NULL, ",");
        if (!token) continue;
        fooddb[foodcount].calories_per_100g = atof(token);

        token = strtok(NULL, ",\n");
        if (!token) continue;
        strcpy(fooddb[foodcount].category, token);

        foodcount++;
    }
    fclose(file);
}

void categorylist(const char *category) {
    printf("\nFoods in category '%s':\n", category);
    for (int i = 0; i < foodcount; i++) {
        if (strcmp(fooddb[i].category, category) == 0) {
            printf("%-30s %6.2f kcal/100g\n", fooddb[i].name, fooddb[i].calories_per_100g);
        }
    }
}

void logfoods() {
    char name[50], date[15];
    float quantity;
    int found = 0;

    printf("Enter date (dd/mm/yyyy): ");
    scanf("%14s", date);
    printf("Enter food name: ");
    scanf("%49s", name);
    printf("Enter quantity in grams: ");
    scanf("%f", &quantity);

    for (int i = 0; i < foodcount; i++) {
        if (strcmp(fooddb[i].name, name) == 0) {
            float calories = (fooddb[i].calories_per_100g * quantity) / 100.0;

            FILE *file = fopen("foodlog.csv", "a");
            if (!file) {
                printf("Failed to open foodlog.csv\n");
                return;
            }

            fprintf(file, "%s,%s,%.2f,%.2f\n", date, name, quantity, calories);
            fclose(file);

            printf("Logged %s (%.2f g): %.2f kcal\n", name, quantity, calories);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Food not found.\n");
    }
}

void logwater() {
    char date[15];
    float liters;
    printf("Enter date (dd/mm/yyyy): ");
    scanf("%14s", date);
    printf("Enter water consumed (in liters): ");
    scanf("%f", &liters);

    FILE *file = fopen("waterlog.csv", "a");
    if (!file) {
        printf("Failed to open waterlog.csv\n");
        return;
    }

    fprintf(file, "%s,%.2f\n", date, liters);
    fclose(file);
    printf("Logged %.2f L of water for %s.\n", liters, date);
}

void viewlogbydate() {
    char date[15];
    char line[200];
    float total = 0;
    int found = 0;

    printf("Enter date to view log (dd/mm/yyyy): ");
    scanf("%14s", date);
    printf("\nLog for %s:\n", date);

    FILE *file = fopen("foodlog.csv", "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            char entry_date[15], name[50];
            float qty, cal;
            sscanf(line, "%14[^,],%49[^,],%f,%f", entry_date, name, &qty, &cal);
            if (strcmp(entry_date, date) == 0) {
                printf("%-30s %6.2f g %6.2f kcal\n", name, qty, cal);
                total += cal;
                found = 1;
            }
        }
        fclose(file);
    }

    if (!found) {
        printf("No food entries found.\n");
    } else {
        printf("Total Calories: %.2f kcal\n", total);
    }

    float water = 0;
    found = 0;
    file = fopen("waterlog.csv", "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            char entry_date[15];
            float liters;
            sscanf(line, "%14[^,],%f", entry_date, &liters);
            if (strcmp(entry_date, date) == 0) {
                water = liters;
                found = 1;
                break;
            }
        }
        fclose(file);
    }

    if (found)
        printf("Water Intake: %.2f L\n", water);
    else
        printf("No water logged.\n");
}

void editentrybydate() {
    char date[15], name[50];
    float new_qty;
    int edited = 0;

    printf("Enter date of entry to edit (dd/mm/yyyy): ");
    scanf("%14s", date);
    printf("Enter food name to edit: ");
    scanf("%49s", name);

    FILE *file = fopen("foodlog.csv", "r");
    FILE *temp = fopen("temp.csv", "w");

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char entry_date[15], entry_name[50];
        float qty, cal;
        sscanf(line, "%14[^,],%49[^,],%f,%f", entry_date, entry_name, &qty, &cal);

        if (strcmp(entry_date, date) == 0 && strcmp(entry_name, name) == 0 && !edited) {
            printf("Current quantity: %.2f g, calories: %.2f kcal\n", qty, cal);
            printf("Enter new quantity in grams: ");
            scanf("%f", &new_qty);

            for (int j = 0; j < foodcount; j++) {
                if (strcmp(fooddb[j].name, name) == 0) {
                    float new_cal = (fooddb[j].calories_per_100g * new_qty) / 100.0;
                    fprintf(temp, "%s,%s,%.2f,%.2f\n", date, name, new_qty, new_cal);
                    edited = 1;
                    break;
                }
            }
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);
    remove("foodlog.csv");
    rename("temp.csv", "foodlog.csv");

    if (edited)
        printf("Entry updated.\n");
    else
        printf("Entry not found.\n");
}

void deleteentrybydate() {
    char date[15], name[50];
    int deleted = 0;

    printf("Enter date of entry to delete (dd/mm/yyyy): ");
    scanf("%14s", date);
    printf("Enter food name to delete: ");
    scanf("%49s", name);

    FILE *file = fopen("foodlog.csv", "r");
    FILE *temp = fopen("temp.csv", "w");

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char entry_date[15], entry_name[50];
        float qty, cal;
        sscanf(line, "%14[^,],%49[^,],%f,%f", entry_date, entry_name, &qty, &cal);

        if (strcmp(entry_date, date) == 0 && strcmp(entry_name, name) == 0) {
            deleted = 1;
            continue;
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);
    remove("foodlog.csv");
    rename("temp.csv", "foodlog.csv");

    if (deleted)
        printf("Entry deleted.\n");
    else
        printf("Entry not found.\n");
}

void editwaterbydate() {
    char date[15];
    float new_liters;
    int edited = 0;

    printf("Enter date of water log to edit (dd/mm/yyyy): ");
    scanf("%14s", date);

    FILE *file = fopen("waterlog.csv", "r");
    FILE *temp = fopen("temp.csv", "w");

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char entry_date[15];
        float liters;
        sscanf(line, "%14[^,],%f", entry_date, &liters);

        if (strcmp(entry_date, date) == 0 && !edited) {
            printf("Current water intake: %.2f L\n", liters);
            printf("Enter new water intake: ");
            scanf("%f", &new_liters);
            fprintf(temp, "%s,%.2f\n", date, new_liters);
            edited = 1;
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);
    remove("waterlog.csv");
    rename("temp.csv", "waterlog.csv");

    if (edited)
        printf("Water entry updated.\n");
    else
        printf("Water entry not found.\n");
}

void deletewaterbydate() {
    char date[15];
    int deleted = 0;

    printf("Enter date of water log to delete (dd/mm/yyyy): ");
    scanf("%14s", date);

    FILE *file = fopen("waterlog.csv", "r");
    FILE *temp = fopen("temp.csv", "w");

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char entry_date[15];
        float liters;
        sscanf(line, "%14[^,],%f", entry_date, &liters);

        if (strcmp(entry_date, date) == 0) {
            deleted = 1;
            continue;
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);
    remove("waterlog.csv");
    rename("temp.csv", "waterlog.csv");

    if (deleted)
        printf("Water log deleted.\n");
    else
        printf("Water log not found.\n");
}

void displayalllogs() {
    char line[200];

    printf("\n=== All Food Logs ===\n");
    FILE *file = fopen("foodlog.csv", "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            char date[15], name[50];
            float qty, cal;
            sscanf(line, "%14[^,],%49[^,],%f,%f", date, name, &qty, &cal);
            printf("%s: %-20s %6.2f g %6.2f kcal\n", date, name, qty, cal);
        }
        fclose(file);
    }

    printf("\n=== All Water Logs ===\n");
    file = fopen("waterlog.csv", "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            char date[15];
            float liters;
            sscanf(line, "%14[^,],%f", date, &liters);
            printf("%s: %.2f L\n", date, liters);
        }
        fclose(file);
    }
}

int main() {
    const char *filename = "foods_with_categories.csv";
    loadfooddb(filename);

    int choice;
    char category[30];

    do {
        printf("\nCalorie and Water Intake Tracker\n");
        printf("1. View food by category\n");
        printf("2. Log food intake\n");
        printf("3. Log water intake\n");
        printf("4. View log by date\n");
        printf("5. Edit food entry\n");
        printf("6. Delete food entry\n");
        printf("7. Edit water entry\n");
        printf("8. Delete water entry\n");
        printf("9. Display all logs\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter category: ");
                scanf("%s", category);
                categorylist(category);
                break;
            case 2:
                logfoods();
                break;
            case 3:
                logwater();
                break;
            case 4:
                viewlogbydate();
                break;
            case 5:
                editentrybydate();
                break;
            case 6:
                deleteentrybydate();
                break;
            case 7:
                editwaterbydate();
                break;
            case 8:
                deletewaterbydate();
                break;
            case 9:
                displayalllogs();
                break;
            case 10:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 10);

    return 0;
}

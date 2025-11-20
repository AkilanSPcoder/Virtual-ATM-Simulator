#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_TRANSACTIONS 5
#define MAX_USERS 4
#define TRANSACTION_FILE "transactions.txt"

typedef struct {
    char accountNumber[20];
    int pin;
    float balance;
    char transactions[MAX_TRANSACTIONS][50];
    int transactionCount;
} Account;

int login(Account accounts[], int totalUsers);
void balanceInquiry(Account *account);
void cashWithdrawal(Account *account);
void depositMoney(Account *account);
void miniStatement(Account *account);
void recordTransaction(Account *account, const char *transaction);
void writeTransactionToFile(const char *accNo, const char *transaction, float balance);
int isvalid(const char *input);

int main() {
    Account accounts[MAX_USERS] = {
        {"1234567890123456", 1234, 1000.0, {}, 0},
        {"8122089700123456", 4321, 2000.0, {}, 0},
        {"9442289700123456", 1111, 1500.0, {}, 0},
        {"1122334455667788", 2222, 1000.0, {}, 0}
    };

    printf("Welcome to the Virtual ATM!\n");
    int userIndex = login(accounts, MAX_USERS);
    if (userIndex == -1) {
        printf("Access denied. Exiting...\n");
        return 1;
    }

    Account *user = &accounts[userIndex];
    int choice;

    do {
        printf("\n--- ATM Menu ---\n");
        printf("1. Balance Inquiry\n");
        printf("2. Cash Withdrawal\n");
        printf("3. Deposit Money\n");
        printf("4. Mini-Statement\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                balanceInquiry(user);
                break;
            case 2:
                cashWithdrawal(user);
                break;
            case 3:
                depositMoney(user);
                break;
            case 4:
                miniStatement(user);
                break;
            case 5:
                printf("Thank you for using the Virtual ATM. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);

    return 0;
}

int isvalid(const char *input) {
    if (strlen(input) != 16) return 0;
    for (int i = 0; i < 16; i++) {
        if (!isdigit(input[i])) return 0;
    }
    return 1;
}

int login(Account accounts[], int totalUsers) {
    char inputAccNo[20];
    int inputPin, res;

    while (1) {
        printf("\nEnter Account Number: ");
        scanf("%s", inputAccNo);
        res = isvalid(inputAccNo);

        if (res == 0) {
            printf("Invalid Account Number! Please enter a 16-digit number.\n");
            continue;
        }

        printf("Enter PIN: ");
        scanf("%d", &inputPin);

        for (int i = 0; i < totalUsers; i++) {
            if (strcmp(accounts[i].accountNumber, inputAccNo) == 0 && accounts[i].pin == inputPin) {
                printf("Login successful!\n");
                return i;
            }
        }

        printf("Invalid Account Number or PIN.\n");
        return -1;
    }
}

void balanceInquiry(Account *account) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("\n-------- Balance Enquiry Receipt --------\n");
    printf("Account No : %s\n", account->accountNumber);
    printf("Date       : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    printf("Time       : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
    printf("Available Balance : $%.2f\n", account->balance);
    printf("-----------------------------------------\n");

    FILE *file = fopen(TRANSACTION_FILE, "a");
    if (file != NULL) {
        fprintf(file, "\n-------- Balance Enquiry Receipt --------\n");
        fprintf(file, "Account No : %s\n", account->accountNumber);
        fprintf(file, "Date       : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        fprintf(file, "Time       : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        fprintf(file, "Available Balance : $%.2f\n", account->balance);
        fprintf(file, "-----------------------------------------\n");
        fclose(file);
    }
}

void cashWithdrawal(Account *account) {
    float amount;
    printf("Enter amount to withdraw (Limit: $50000): $");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }
    if (amount > 50000) {
        printf("Amount exceeds withdrawal limit of $50000.\n");
        return;
    }
    if (amount > account->balance) {
        printf("Insufficient balance. Current balance: $%.2f\n", account->balance);
        return;
    }

    account->balance -= amount;
    printf("Withdrawal successful. New balance: $%.2f\n", account->balance);

    char transaction[50];
    sprintf(transaction, "Withdrawal: -$%.2f", amount);
    recordTransaction(account, transaction);

    char choice;
    printf("Do you want a receipt? (Y/N): ");
    scanf(" %c", &choice);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (choice == 'Y' || choice == 'y') {
        printf("\n-------- Withdrawal Receipt --------\n");
        printf("Account No : %s\n", account->accountNumber);
        printf("Amount     : $%.2f\n", amount);
        printf("Balance    : $%.2f\n", account->balance);
        printf("Date       : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        printf("Time       : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        printf("------------------------------------\n");
    }

    writeTransactionToFile(account->accountNumber, transaction, account->balance);
}

void depositMoney(Account *account) {
    float amount;
    printf("Enter amount to deposit: $");
    scanf("%f", &amount);

    if (amount > 0) {
        account->balance += amount;

        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        char transaction[50];
        sprintf(transaction, "Deposit: +$%.2f", amount);
        recordTransaction(account, transaction);

        printf("\n-------- Deposit Receipt --------\n");
        printf("Account No : %s\n", account->accountNumber);
        printf("Amount     : $%.2f\n", amount);
        printf("New Balance: $%.2f\n", account->balance);
        printf("Date       : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        printf("Time       : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        printf("----------------------------------\n");

        FILE *file = fopen(TRANSACTION_FILE, "a");
        if (file != NULL) {
            fprintf(file, "\n-------- Deposit Receipt --------\n");
            fprintf(file, "Account No : %s\n", account->accountNumber);
            fprintf(file, "Amount     : $%.2f\n", amount);
            fprintf(file, "New Balance: $%.2f\n", account->balance);
            fprintf(file, "Date       : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
            fprintf(file, "Time       : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
            fprintf(file, "----------------------------------\n");
            fclose(file);
        }

        writeTransactionToFile(account->accountNumber, transaction, account->balance);
    } else {
        printf("Invalid deposit amount.\n");
    }
}

void miniStatement(Account *account) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("\n=====================================\n");
    printf("          MINI STATEMENT\n");
    printf("Account Number : %s\n", account->accountNumber);
    printf("Date           : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    printf("Time           : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
    printf("-------------------------------------\n");
    printf("  Type        Amount\n");
    printf("-------------------------------------\n");

    if (account->transactionCount == 0) {
        printf("  No transactions available.\n");
    } else {
        for (int i = 0; i < account->transactionCount; i++) {
            printf("  %s\n", account->transactions[i]);
        }
    }

    printf("-------------------------------------\n");
    printf("Available Balance : $%.2f\n", account->balance);
    printf("=====================================\n");

    FILE *file = fopen(TRANSACTION_FILE, "a");
    if (file != NULL) {
        fprintf(file, "\n=====================================\n");
        fprintf(file, "          MINI STATEMENT\n");
        fprintf(file, "Account Number : %s\n", account->accountNumber);
        fprintf(file, "Date           : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        fprintf(file, "Time           : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
        fprintf(file, "-------------------------------------\n");
        fprintf(file, "  Type        Amount\n");
        fprintf(file, "-------------------------------------\n");

        for (int i = 0; i < account->transactionCount; i++) {
            fprintf(file, "  %s\n", account->transactions[i]);
        }

        fprintf(file, "-------------------------------------\n");
        fprintf(file, "Available Balance : $%.2f\n", account->balance);
        fprintf(file, "=====================================\n");
        fclose(file);
    }
}

void recordTransaction(Account *account, const char *transaction) {
    if (account->transactionCount < MAX_TRANSACTIONS) {
        strcpy(account->transactions[account->transactionCount], transaction);
        account->transactionCount++;
    } else {
        for (int i = 1; i < MAX_TRANSACTIONS; i++) {
            strcpy(account->transactions[i - 1], account->transactions[i]);
        }
        strcpy(account->transactions[MAX_TRANSACTIONS - 1], transaction);
    }
}

void writeTransactionToFile(const char *accNo, const char *transaction, float balance) {
    FILE *file = fopen(TRANSACTION_FILE, "a");
    if (file == NULL) {
        printf("Error writing to transaction file.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(file, "\n-------- Receipt --------\n");
    fprintf(file, "Account No : %s\n", accNo);
    fprintf(file, "%s\n", transaction);
    fprintf(file, "Balance    : $%.2f\n", balance);
    fprintf(file, "Date       : %02d-%02d-%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    fprintf(file, "Time       : %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(file, "--------------------------\n");

    fclose(file);
}

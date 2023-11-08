/*
	Register
	Log In
	Deposit Money
	Withdraw
	Add Contacts
	View Transaction History
	Make Transfers
*/
/*-------------------------------------------------------------------------------------------------------------------------*/
//LIBRARIES
#include <stdio.h>
#include <locale.h>
#include <stdbool.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
/*-------------------------------------------------------------------------------------------------------------------------*/
//STRUCTURES
typedef struct {
	int AccountNumber;
} ContactRecord;

typedef struct {
	int SenderAccount;
	int ReceiverAccount;
	float Amount;
} TransactionRecord;

typedef struct {
	int AccountNumber;
	char *Name;
	char *Password;
	float Balance;
	int NumFriends;
	int NumTransactions;
	ContactRecord *Friends;
	TransactionRecord *TransactionHistory;
} User;

/*-------------------------------------------------------------------------------------------------------------------------*/
//FUNCTIONS AND GLOBAL VARIABLES DECLARATION
void MainMenu();
void Register();
void RegisterUser(char *, char *, int);
void ViewUsers();
void UserMenu();

void MakeTransfer();
void Transfer(int, int, float);
void ViewTransactions();
void DepositMoney();
void Deposit(float);
void WithdrawMoney();
void Withdraw(float);

char *GetText(char *, int);
int GetNumber(char *);
float GetFloatNumber(char *);
int GenerateAccountNumber();
void AddContact();
bool ContactExists(int, User *);
void ViewContacts();

bool Login();

User *UserDatabase;
int NumberOfUsers = 0;
int CurrentUserNumber;

/*-------------------------------------------------------------------------------------------------------------------------*/
int main()
{
	setlocale(LC_ALL, "");

	UserDatabase = (User *)malloc(sizeof(User));
	RegisterUser("Juan", "Password", 1);

	MainMenu();

	return 0;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void MainMenu()
{
	bool exit = false;
	char option[2];
	int choice;
	do {
		system("CLS");
		printf("Choose an option:\n");
		printf("1: Register\n2: Log In\n3: View Users\n4: Exit\n");
		// VALIDATE USER INPUT
		scanf("%s", option);
		choice = atoi(option);
		switch (choice) {
		case 1:
			Register();
			break;
		case 2: {
			if (Login() == true)
				UserMenu();
		}
				break;
		case 3:
			ViewUsers();
			break;
		case 4:
			exit = true;
			break;
		default:
			printf("Invalid parameter\n");
		}
	} while (exit == false);
	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void UserMenu()
{
	bool exit = false;
	int choice;
	char option[2];
	do {
		system("CLS");
		printf("Welcome %s, Available balance: $%f\nChoose an option:\n", UserDatabase[CurrentUserNumber - 1].Name, UserDatabase[CurrentUserNumber - 1].Balance);
		printf("1: Make a Transfer\n2: View Contacts\n3: Add Contact\n4: Transaction History\n5: Deposit Money\n6: Withdraw Money\n7: Logout\n");
		scanf("%s", option);
		choice = atoi(option);

		switch (choice) {
		case 1:
			MakeTransfer();
			break;
		case 2:
			ViewContacts();
			break;
		case 3:
			AddContact();
			break;
		case 4:
			ViewTransactions();
			break;
		case 5:
			DepositMoney();
			break;
		case 6:
			WithdrawMoney();
			break;
		case 7:
			exit = true;
			break;
		default: {
			printf("Invalid parameters\n");
			system("PAUSE");
		}
		}
	} while (exit == false);

}
/*-------------------------------------------------------------------------------------------------------------------------*/
void AddContact()
{
	int accountNumber = GetNumber("Enter the account number");
	if (accountNumber <= NumberOfUsers && accountNumber != CurrentUserNumber) {
		User *user = &UserDatabase[CurrentUserNumber - 1];
		if (user->NumFriends == 0) {
			user->Friends = (ContactRecord *)malloc(sizeof(ContactRecord));
			user->Friends[0].AccountNumber = accountNumber;
			user->NumFriends = 1;
			printf("%s has been added to your contacts\n", UserDatabase[accountNumber - 1].Name);
		}
		else if (!ContactExists(accountNumber, user)) {
			int lastIndex = user->NumFriends;
			user->Friends = (ContactRecord *)realloc(user->Friends, sizeof(ContactRecord) * (lastIndex + 1));
			user->Friends[lastIndex].AccountNumber = accountNumber;
			user->NumFriends++;
			printf("%s has been added to your contacts\n", UserDatabase[accountNumber - 1].Name);
		}
		else
			printf("User %s is already in your contacts\n", UserDatabase[accountNumber - 1].Name);
	}
	else
		printf("Invalid account\n");

	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void ViewContacts()
{
	int i;
	int accountNumber;
	User *user = &UserDatabase[CurrentUserNumber - 1];
	if (user->NumFriends > 0) {
		for (i = 0; i < user->NumFriends; i++) {
			accountNumber = user->Friends[i].AccountNumber;
			printf("Name: %s \t Account Number: %d\n", UserDatabase[accountNumber - 1].Name, accountNumber);
		}
	}
	else
		printf("You don't have any contacts yet :(\n");

	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
bool ContactExists(int accountNumber, User *user)
{
	int i;
	for (i = 0; i < user->NumFriends; i++) {
		if (user->Friends[i].AccountNumber == accountNumber) {
			return true;
		}
	}
	return false;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void DepositMoney()
{
	float amount = GetFloatNumber("Enter the amount you want to deposit into your account");
	Deposit(amount);
	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void Deposit(float amount)
{
	Transfer(CurrentUserNumber, CurrentUserNumber, amount);
	UserDatabase[CurrentUserNumber - 1].Balance += amount;
	printf("$%f has been deposited into your account\n", amount);
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void WithdrawMoney()
{
	float amount = GetFloatNumber("Enter the amount you want to withdraw from your account");
	if (amount <= UserDatabase[CurrentUserNumber - 1].Balance) {
		Withdraw(amount);
	}
	else
		printf("You don't have enough funds to make that withdrawal\n");

	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void Withdraw(float amount)
{
	Transfer(CurrentUserNumber, CurrentUserNumber, -amount);
	UserDatabase[CurrentUserNumber - 1].Balance -= amount;
	printf("$%f has been withdrawn from your account\n", amount);
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void MakeTransfer()
{
	TransactionRecord newTransaction;
	int destinationAccount = GetNumber("Enter the destination account number");
	int senderAccount = CurrentUserNumber;
	float transferAmount = GetFloatNumber("Enter the amount to be transferred");
	if (destinationAccount <= NumberOfUsers) {
		if (transferAmount <= UserDatabase[CurrentUserNumber - 1].Balance) {
			Transfer(destinationAccount, senderAccount, transferAmount);
		}
		else
			printf("You don't have sufficient funds to perform this transaction\n");
	}
	else
		printf("Invalid account\n");
	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void Transfer(int destination, int sender, float amount)
{
	bool isDestinationValid;
	User *user = &UserDatabase[CurrentUserNumber - 1];
	isDestinationValid = (destination <= NumberOfUsers && destination != sender) ? true : false;
	if (isDestinationValid) {
		TransactionRecord transaction;
		transaction.ReceiverAccount = destination;
		transaction.SenderAccount = sender;
		transaction.Amount = amount;

		User *destinationUser = &UserDatabase[destination - 1];

		user->Balance -= amount;
		destinationUser->Balance += amount;

		user->TransactionHistory = (user->NumTransactions == 0) ? (TransactionRecord *)malloc(sizeof(TransactionRecord)) : user->TransactionHistory;
		destinationUser->TransactionHistory = (destinationUser->NumTransactions == 0) ? (TransactionRecord *)malloc(sizeof(TransactionRecord)) : destinationUser->TransactionHistory;

		user->NumTransactions++;
		destinationUser->NumTransactions++;

		user->TransactionHistory = (TransactionRecord *)realloc(user->TransactionHistory, sizeof(TransactionRecord) * (user->NumTransactions));
		user->TransactionHistory[user->NumTransactions - 1] = transaction;

		destinationUser->TransactionHistory = (TransactionRecord *)realloc(destinationUser->TransactionHistory, sizeof(TransactionRecord) * (destinationUser->NumTransactions));
		destinationUser->TransactionHistory[destinationUser->NumTransactions - 1] = transaction;
	}
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void ViewTransactions()
{
	int i, destinationIndex, senderIndex;
	User *user = &UserDatabase[CurrentUserNumber - 1];
	if (user->NumTransactions > 0) {
		float amount;
		for (i = 0; i < user->NumTransactions; i++) {
			destinationIndex = user->TransactionHistory[i].ReceiverAccount - 1;
			senderIndex = user->TransactionHistory[i].SenderAccount - 1;
			amount = user->TransactionHistory[i].Amount;
			printf("From %s to %s: $%f\n", UserDatabase[senderIndex].Name, UserDatabase[destinationIndex].Name, amount);
		}
	}
	else
		printf("You haven't made any transactions yet\n");

	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
bool Login()
{
	bool loggedIn = false;
	int accountNumber = GetNumber("Enter your account number");
	if (accountNumber <= NumberOfUsers) {
		char *password = GetText("Enter the password", 0);
		loggedIn = (strcmp(password, UserDatabase[accountNumber - 1].Password) == 0) ? true : false;
		if (loggedIn == true) {
			printf("Login successful\n");
			CurrentUserNumber = accountNumber;
		}
		else
			printf("Incorrect password\n");
	}
	else
		printf("Invalid account number\n");

	system("PAUSE");
	return loggedIn;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void Register()
{
	char *name = GetText("Enter the name", 0);
	char *password = GetText("Enter the password (Minimum 8 Characters)", 8);
	int accountNumber = GenerateAccountNumber();

	RegisterUser(name, password, accountNumber);

	printf("Your account number is: %d\n", accountNumber);
	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void RegisterUser(char *name, char *password, int accountNumber)
{
	UserDatabase[accountNumber - 1].Name = (char *)malloc(sizeof(char));
	UserDatabase[accountNumber - 1].Password = (char *)malloc(sizeof(char));

	User *user = &UserDatabase[accountNumber - 1];

	strcpy(user->Name, name);
	strcpy(user->Password, password);

	user->AccountNumber = accountNumber;
	user->Balance = 0;
	user->NumFriends = 0;
	user->NumTransactions = 0;
	NumberOfUsers++;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
void ViewUsers()
{
	int i;

	if (NumberOfUsers > 0) {
		for (i = 0; i < NumberOfUsers; i++) {
			printf("Name: %s \t Account Number: %d\n", UserDatabase[i].Name, UserDatabase[i].AccountNumber);
		}
	}
	else
		printf("There are no registered users yet\n");

	system("PAUSE");
}
/*-------------------------------------------------------------------------------------------------------------------------*/
char *GetText(char *prompt, int length)
{
	char *text;
	bool valid = false;
	text = (char *)malloc(sizeof(char));
	do {
		fflush(stdin);
		system("CLS");
		printf("%s: ", prompt);
		scanf("%[^\n]", text);

		if (strcmp(text, "")) {
			valid = (length <= 0) ? true : false;
			if (!valid) {
				valid = (strlen(text) >= length) ? true : false;
			}
		}
	} while (!valid);
	return text;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
int GetNumber(char *prompt)
{
	char *number;
	int result;
	number = (char *)malloc(sizeof(char));
	do {
		fflush(stdin);
		printf("%s: ", prompt);
		scanf("%s", number);
		result = atoi(number);
	} while (result <= 0);
	return result;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
float GetFloatNumber(char *prompt)
{
	char *number;
	float result;
	number = (char *)malloc(sizeof(char));
	do {
		fflush(stdin);
		printf("%s: ", prompt);
		scanf("%s", number);
		result = atof(number);
	} while (result <= 0);
	return result;
}
/*-------------------------------------------------------------------------------------------------------------------------*/
int GenerateAccountNumber()
{
	if (NumberOfUsers == 0) {
		UserDatabase = (User *)malloc(sizeof(User));
		return NumberOfUsers + 1;
	}
	else {
		UserDatabase = (User *)realloc(UserDatabase, sizeof(User) * (NumberOfUsers + 1));
		return NumberOfUsers + 1;
	}
}

#include <iostream>
#include <string>
#include <fstream>
#define MAX_ACCOUNT 100
using namespace std;

struct Account
{
    char name[50];
    char username[50];
    char phoneNumber[20];
    char password[20];
    int balance;

    void modifyData()
    {
        cout << "Masukkan Nama (" << name << ") : ";
        cin >> name;
        cout << "Masukkan Username (" << username << ") : ";
        cin >> username;
        cout << "Masukkan Nomor Telepon (" << phoneNumber << ") : ";
        cin >> phoneNumber;
    }
};

struct FileHandler
{
    static void appendToFile(string FILE_NAME, Account data)
    {
        ofstream outFile;
        outFile.open(FILE_NAME, ios::out | ios::binary | ios::app);
        if (!outFile.good())
        {
            throw invalid_argument("Cannot Open File!");
        }

        outFile.write((char *)(&data), sizeof(data));
        outFile.close();
    }

    static void readFromFile(string FILE_NAME, Account (&userAccount)[MAX_ACCOUNT], int &lengthUser)
    {
        ifstream inFile;
        inFile.open(FILE_NAME, ios::binary);
        if (!inFile.good())
        {
            throw invalid_argument("Cannot Open File!");
        }
        while (inFile.read((char *)(&userAccount[lengthUser]), sizeof(userAccount[lengthUser])))
        {
            if (inFile.eof())
            {
                break;
            }

            lengthUser++;
        }
        inFile.close();
    }

    static void saveToFile(string FILE_NAME, Account userAccount[MAX_ACCOUNT], int length)
    {
        for (size_t i = 0; i < length; i++)
        {
            FileHandler::appendToFile("temp.dat", userAccount[i]);
        }
        remove(FILE_NAME.c_str());
        rename("temp.dat", FILE_NAME.c_str());
    }
};

struct SortHandler
{
    static void merge(Account arrAccount[MAX_ACCOUNT], int first, int middle, int last)
    {
        int lengthArrayLeft = middle - first + 1;
        int lengthArrayRight = last - middle;

        Account *leftArray = (Account *)malloc(lengthArrayLeft * sizeof(Account));
        Account *rightArray = (Account *)malloc(lengthArrayRight * sizeof(Account));

        for (size_t i = 0; i < lengthArrayLeft; i++)
        {
            leftArray[i] = arrAccount[first + i];
        }

        for (size_t i = 0; i < lengthArrayRight; i++)
        {
            rightArray[i] = arrAccount[middle + 1 + i];
        }

        int indexLeftArray = 0;
        int indexRightArray = 0;
        int indexMergeArray = first;

        while (indexLeftArray < lengthArrayLeft && indexRightArray < lengthArrayRight)
        {
            string leftName(leftArray[indexLeftArray].name);
            string rightName(rightArray[indexRightArray].name);

            leftName = tolower(leftName[0]);
            rightName = tolower(rightName[0]);

            if (strcmp(leftName.c_str(), rightName.c_str()) < 0)
            {
                arrAccount[indexMergeArray] = leftArray[indexLeftArray];
                indexLeftArray++;
            }
            else
            {
                arrAccount[indexMergeArray] = rightArray[indexRightArray];
                indexRightArray++;
            }

            indexMergeArray++;
        }

        while (indexLeftArray < lengthArrayLeft)
        {
            arrAccount[indexMergeArray] = leftArray[indexLeftArray];
            indexLeftArray++;
            indexMergeArray++;
        }

        while (indexRightArray < lengthArrayRight)
        {
            arrAccount[indexMergeArray] = rightArray[indexRightArray];
            indexRightArray++;
            indexMergeArray++;
        }
    }

    static void mergeSort(Account arrAccount[MAX_ACCOUNT], int first, int last)
    {
        if (first >= last)
        {
            return;
        }

        int middle = (first + last) / 2;
        mergeSort(arrAccount, first, middle);
        mergeSort(arrAccount, middle + 1, last);
        merge(arrAccount, first, middle, last);
    }
};

class BankManagement
{
private:
    Account userAccount[MAX_ACCOUNT];
    int lengthUser = 0;
    string FILE_NAME;

public:
    BankManagement(string fileName)
    {
        FILE_NAME = fileName;

        importFromFiles(FILE_NAME);
    }

    void importFromFiles(string FILE_NAME)
    {
        try
        {
            FileHandler::readFromFile(FILE_NAME, userAccount, lengthUser);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            std::cerr << "Belum ada data yang di simpan!" << '\n';
        }
    }

    int menu()
    {
        int result;
        cout << "1. Create an Account" << endl
             << "2. Deposit Account" << endl
             << "3. Withdraw Account" << endl
             << "4. Search Account" << endl
             << "5. List Account" << endl
             << "6. Close an Account" << endl
             << "7. Modify an Account" << endl
             << "8. Exit" << endl
             << "Pilih Menu [1-8] : ";
        cin >> result;
        return result;
    }

    void createAccount()
    {
        Account &userCreate = userAccount[lengthUser];

        cout << "Masukkan Nama : ";
        cin >> userCreate.name;

        do
        {
            cout << "Masukkan Username : ";
            cin >> userCreate.username;
        } while (!(findIndexByUsername(userCreate.username) == -1) && cout << "Username Sudah digunakan!" << endl);

        cout << "Masukkan Password : ";
        cin >> userCreate.password;

        do
        {
            cout << "Masukkan Nomor Telepon : ";
            cin >> userCreate.phoneNumber;
        } while (!(findIndexByPhoneNumber(userCreate.phoneNumber) == -1) && cout << "Nomor HP Sudah digunakan!" << endl);
        cout << "Masukkan Deposit Awal : ";
        cin >> userCreate.balance;

        FileHandler::appendToFile(FILE_NAME, userCreate);

        lengthUser++;
    }

    int findIndexByUsername(string username)
    {
        for (int i = 0; i < lengthUser; i++)
        {
            if (userAccount[i].username == username)
            {
                return i;
            }
        }
        return -1;
    }

    int findIndexByPhoneNumber(string phoneNumber)
    {
        return -1;
        for (int i = 0; i < lengthUser; i++)
        {
            if (userAccount[i].phoneNumber == phoneNumber)
            {
                return i;
            }
        }
        return -1;
    }

    void depositAccount(string depositUsername, int amountDeposit)
    {
        int index = findIndexByUsername(depositUsername);

        if (index == -1)
        {
            cout << "Username Tidak Ditemukan!" << endl;
            return;
        }

        userAccount[index].balance += amountDeposit;
        FileHandler::saveToFile(FILE_NAME, userAccount, lengthUser);
        cout << "Deposit Berhasil!" << endl;
        cout << "Saldo Akhir : " << userAccount[index].balance << endl;
    }

    void withdrawAccount(string withdrawUsername, int amountWithdraw)
    {
        int index = findIndexByUsername(withdrawUsername);
        if (index == -1)
        {
            cout << "Username Tidak Ditemukan!" << endl;
            return;
        }

        if (userAccount[index].balance > amountWithdraw)
        {
            userAccount[index].balance -= amountWithdraw;
            FileHandler::saveToFile(FILE_NAME, userAccount, lengthUser);
            cout << "Penarikan Berhasil!" << endl;
            cout << "Saldo Akhir : " << userAccount[index].balance << endl;
            return;
        }

        cout << "Saldo tidak mencukupi!" << endl;
    }

    void showDetail(Account user)
    {
        cout << "Nama       : " << user.name << endl;
        cout << "Username   : " << user.username << endl;
        cout << "Nomor HP   : " << user.phoneNumber << endl;
        cout << "Saldo      : " << user.balance << endl;
    }

    void searchUserByUsername(string username)
    {
        int index = findIndexByUsername(username);
        if (index == -1)
        {
            cout << "Username Tidak Ditemukan!" << endl;
            return;
        }

        Account &userFind = userAccount[index];
        showDetail(userFind);
        return;
    }

    void searchUserByPhoneNumber(string phoneNumber)
    {
        int index = findIndexByPhoneNumber(phoneNumber);
        if (index == -1)
        {
            cout << "Nomor HP Tidak Ditemukan!" << endl;
            return;
        }

        Account &userFind = userAccount[index];
        showDetail(userFind);
    }

    void listAllAccount()
    {
        for (size_t i = 0; i < lengthUser; i++)
        {
            Account &user = userAccount[i];
            showDetail(user);
            cout << endl;
        }
    }

    void listAllAccountFilterUsername(string usernameFilter)
    {
        for (size_t i = 0; i < lengthUser; i++)
        {
            Account &user = userAccount[i];
            string username(user.username);
            if (username.find(usernameFilter) != string::npos)
            {
                showDetail(user);
                cout << endl;
            }
        }
    }

    void listAllAccountFilterPhoneNumber(string phoneNumberFilter)
    {
        for (size_t i = 0; i < lengthUser; i++)
        {
            Account &user = userAccount[i];
            string phoneNumber(user.phoneNumber);
            if (phoneNumber.find(phoneNumberFilter) != string::npos)
            {
                showDetail(user);
                cout << endl;
            }
        }
    }

    void listAllAccountSortByName()
    {
        Account *sortAccount = (Account *)malloc(100 * sizeof(Account));
        memcpy(sortAccount, userAccount, 100 * sizeof(Account));

        SortHandler::mergeSort(sortAccount, 0, lengthUser - 1);
        for (size_t i = 0; i < lengthUser; i++)
        {
            Account &user = sortAccount[i];
            showDetail(user);
            cout << endl;
        }
    }

    void modifyAccount(string username)
    {
        int index = findIndexByUsername(username);

        if (index == -1)
        {
            cout << "Username Tidak Ditemukan!" << endl;
            return;
        }

        Account &userModify = userAccount[index];
        userModify.modifyData();

        FileHandler::saveToFile(FILE_NAME, userAccount, lengthUser);
        cout << "Data Berhasil Diubah!" << endl;
    }

    void deleteAccountByUsername(string username)
    {
        int index = findIndexByUsername(username);

        if (index == -1)
        {
            cout << "Username Tidak Ditemukan!" << endl;
            return;
        }

        for (size_t i = index; i < lengthUser - 1; i++)
        {
            userAccount[i] = userAccount[i + 1];
        }
        lengthUser--;

        FileHandler::saveToFile(FILE_NAME, userAccount, lengthUser);
        cout << "Data Berhasil Dihapus!" << endl;
    }
};

int main()
{

    BankManagement bankManagement("accounts.dat");
    int select;
    string username;
    string name;
    string phoneNumber;
    system("cls");

    cout << "Digital Bank Management" << endl
         << endl;
    do
    {
        system("cls");
        cout << "Digital Bank Management" << endl
             << endl;
        select = bankManagement.menu();
        if (select == 1)
        {
            system("cls");
            bankManagement.createAccount();
            cout << "Akun berhasil dibuat!" << endl;

            cin.ignore(2);
        }
        else if (select == 2)
        {
            system("cls");
            int amountDeposit;
            cout << "Masukkan Username Untuk Deposit : ";
            cin >> username;
            cout << "Masukkan Jumlah Deposit : ";
            cin >> amountDeposit;

            bankManagement.depositAccount(username, amountDeposit);
            cin.ignore(2);
        }
        else if (select == 3)
        {
            system("cls");
            int amountWithdraw;
            cout << "Masukkan Username Untuk Withdraw : ";
            cin >> username;
            cout << "Masukkan Jumlah Deposit : ";
            cin >> amountWithdraw;

            bankManagement.withdrawAccount(username, amountWithdraw);
            cin.ignore(2);
        }
        else if (select == 4)
        {
            system("cls");
            int selectMenu;
            cout << "1. Cari menggunakan Username" << endl;
            cout << "2. Cari menggunakan Nomor HP" << endl;
            cout << "Pilih Menu [1-2] : ";
            cin >> selectMenu;

            if (selectMenu == 1)
            {
                system("cls");
                cout << "Masukkan Username : ";
                cin >> username;
                bankManagement.searchUserByUsername(username);
            }
            else if (selectMenu == 2)
            {
                system("cls");
                cout << "Masukkan Nomor HP : ";
                cin >> phoneNumber;
                bankManagement.searchUserByPhoneNumber(phoneNumber);
            }
            else
            {
                system("cls");
                cout << "Masukkan Pilihan Benar!" << endl;
            }
            cin.ignore(2);
        }
        else if (select == 5)
        {
            system("cls");
            int selectMenu;
            bankManagement.listAllAccount();

            cout << "1. List Akun dengan Filter Username" << endl;
            cout << "2. List Akun dengan Filter Nomor HP" << endl;
            cout << "3. List Akun dengan Nama urut" << endl;
            cout << "Pilih Menu [1-3] : ";
            cin >> selectMenu;

            if (selectMenu == 1)
            {
                cout << "Masukkan Username : ";
                cin >> username;
                bankManagement.listAllAccountFilterUsername(username);
            }
            else if (selectMenu == 2)
            {
                cout << "Masukkan Nomor HP : ";
                cin >> phoneNumber;
                bankManagement.listAllAccountFilterPhoneNumber(phoneNumber);
            }
            else if (selectMenu == 3)
            {
                bankManagement.listAllAccountSortByName();
            }
            cin.ignore(2);
        }
        else if (select == 6)
        {
            system("cls");
            cout << "Masukkan Username : ";
            cin >> username;
            bankManagement.deleteAccountByUsername(username);
            cin.ignore(2);
        }
        else if (select == 7)
        {
            system("cls");
            cout << "Masukkan Username : ";
            cin >> username;
            bankManagement.modifyAccount(username);
            cin.ignore(2);
        }

    } while (select != 8);

    return 0;
}

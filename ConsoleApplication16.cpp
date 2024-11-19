#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring> 

using namespace std;

class Contact {
private:
    char* name;              
    string homePhone;       
    string workPhone;
    string mobilePhone;     
    string extraInfo;        

public:

    Contact(const char* name, const string& homePhone, const string& workPhone,
        const string& mobilePhone, const string& extraInfo)
        : homePhone(homePhone), workPhone(workPhone), mobilePhone(mobilePhone), extraInfo(extraInfo) {
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    }

    Contact(const Contact& other)
        : homePhone(other.homePhone), workPhone(other.workPhone),
        mobilePhone(other.mobilePhone), extraInfo(other.extraInfo) {
        this->name = new char[strlen(other.name) + 1];
        strcpy(this->name, other.name);
    }

    Contact& operator=(const Contact& other) {
        if (this == &other) return *this;
        delete[] name;
        this->name = new char[strlen(other.name) + 1];
        strcpy(this->name, other.name);
        homePhone = other.homePhone;
        workPhone = other.workPhone;
        mobilePhone = other.mobilePhone;
        extraInfo = other.extraInfo;
        return *this;
    }

    ~Contact() {
        delete[] name;
    }

    const char* getName() const {
        return name;
    }

    void print() const {
        cout << "Full name: " << name << "\n"
            << "Home phone:  " << homePhone << "\n"
            << "Work phone: " << workPhone << "\n"
            << "Mobile phone: " << mobilePhone << "\n"
            << "Extra information:" << extraInfo << "\n";
    }

    void saveToFile(ofstream& file) const {
        size_t nameLen = strlen(name);
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        file.write(name, nameLen);
        file << homePhone << '\n' << workPhone << '\n' << mobilePhone << '\n' << extraInfo << '\n';
    }

    static Contact loadFromFile(ifstream& file) {
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        char* name = new char[nameLen + 1];
        file.read(name, nameLen);
        name[nameLen] = '\0';

        string homePhone, workPhone, mobilePhone, extraInfo;
        getline(file, homePhone);
        getline(file, workPhone);
        getline(file, mobilePhone);
        getline(file, extraInfo);

        Contact contact(name, homePhone, workPhone, mobilePhone, extraInfo);
        delete[] name;
        return contact;
    }
};

class PhoneBook {
private:
    vector<Contact> contacts;

public:
    void addContact(const Contact& contact) {
        contacts.push_back(contact);
    }

    void removeContact(const string& name) {
        contacts.erase(remove_if(contacts.begin(), contacts.end(),
            [&name](const Contact& contact) {
                return name == contact.getName();
            }),
            contacts.end());
    }

    void searchContact(const string& name) const {
        for (const auto& contact : contacts) {
            if (name == contact.getName()) {
                contact.print();
                return;
            }
        }
        cout << "Subscriber with the name \"" << name << "\"not found\n";
    }

    void showAllContacts() const {
        for (const auto& contact : contacts) {
            contact.print();
            cout << "---------------------\n";
        }
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        size_t size = contacts.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& contact : contacts) {
            contact.saveToFile(file);
        }
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cerr << "Error opening file for reading.\n";
            return;
        }
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        contacts.clear();
        for (size_t i = 0; i < size; ++i) {
            contacts.push_back(Contact::loadFromFile(file));
        }
    }
};

int main() {
    PhoneBook phoneBook;
    try {
        phoneBook.addContact(Contact(""));
        phoneBook.addContact(Contact(""));

        cout << "All contacts:\n";
        phoneBook.showAllContacts();

        cout << "\nContact search \"\":\n";
        phoneBook.searchContact("");

        cout << "\nSaving the book to a file...\n";
        phoneBook.saveToFile("phonebook.dat");

        cout << "Loading book from file...\n";
        PhoneBook newPhoneBook;
        newPhoneBook.loadFromFile("phonebook.dat");
        newPhoneBook.showAllContacts();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

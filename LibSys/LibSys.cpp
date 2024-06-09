#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>
#include <ctime>

// Class representing a book in the library
class Book {
public:
    // Constructor to initialize a book with title, author, ISBN, and publication year
    Book(const std::string& title, const std::string& author, const std::string& isbn, int publicationYear)
        : title(title), author(author), isbn(isbn), publicationYear(publicationYear), isAvailable(true), dueDate(0) {}

    // Getters for book details

    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getISBN() const { return isbn; }
    int getPublicationYear() const { return publicationYear; }
    bool getAvailability() const { return isAvailable; }
    std::time_t getDueDate() const { return dueDate; }

    // Setters for availability and due date
    void setAvailability(bool availability) { isAvailable = availability; }
    void setDueDate(std::time_t date) { dueDate = date; }

    // Print the details of the book
    void printDetails() const {
        std::cout << "Title: " << title << "\n"
            << "Author: " << author << "\n"
            << "ISBN: " << isbn << "\n"
            << "Publication Year: " << publicationYear << "\n"
            << "Available: " << (isAvailable ? "Yes" : "No") << std::endl;
        if (!isAvailable) {
#ifdef _WIN32
            // On Windows, use ctime_s for safe conversion
            char buffer[26];
            ctime_s(buffer, sizeof(buffer), &dueDate);
            std::cout << "Due Date: " << buffer;
#else
            // On other systems, use ctime and localtime
            std::cout << "Due Date: " << std::asctime(std::localtime(&dueDate));
#endif
        }
    }

private:
    std::string title;      // Title of the book
    std::string author;     // Author of the book
    std::string isbn;       // ISBN of the book 
    int publicationYear;     // Publication year of the book
    bool isAvailable;       // Availability status of the book
    std::time_t dueDate;    // due date for borrowed book
};

// Class representing the library
class Library {
public:
    // Add a new book to the library
    int addBook(const std::string& title, const std::string& author, const std::string& isbn, int publicationYear) {
        int id = nextId++;
        books[id] = std::make_shared<Book>(title, author, isbn, publicationYear);
        return id;
    }

    // Remove a book from the library by ID, return true if successful
    bool removeBook(int id) {
        return books.erase(id) > 0;
    }

    // Find a book by ID, return a shared pointer to the book if found, otherwise return nullptr
    std::shared_ptr<Book> findBook(int id) const {
        auto it = books.find(id);
        if (it != books.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Print details of all books in the library
    void printAllBooks() const {
        for (const auto& pair : books) {
            std::cout << "Book ID: " << pair.first << std::endl;
            pair.second->printDetails();
            std::cout << std::endl;
        }
    }

    // Print details of all available books in the library
    void printAvailableBooks() const {
        for (const auto& pair : books) {
            if (pair.second->getAvailability()) {
                std::cout << "Book ID: " << pair.first << std::endl;
                pair.second->printDetails();
                std::cout << std::endl;
            }
        }
    }

    // Search for books by title, author, ISBN, ID, or publication year
    void searchBooks(const std::string& query) const {
        std::string q = query;
        std::transform(q.begin(), q.end(), q.begin(), ::tolower); // Convert query to lowercase for case-insensitive search
        bool found = false;

        for (const auto& pair : books) {
            std::shared_ptr<Book> book = pair.second;
            std::string title = book->getTitle();
            std::string author = book->getAuthor();
            std::string isbn = book->getISBN();
            std::string id = std::to_string(pair.first);
            std::string year = std::to_string(book->getPublicationYear());

            // Convert all strings to lowercase for case-insensitive comparison
            std::transform(title.begin(), title.end(), title.begin(), ::tolower);
            std::transform(author.begin(), author.end(), author.begin(), ::tolower);
            std::transform(isbn.begin(), isbn.end(), isbn.begin(), ::tolower);

            // If query matches any book attribute, print book details
            if (title.find(q) != std::string::npos || author.find(q) != std::string::npos ||
                isbn.find(q) != std::string::npos || id == q || year == q) {
                std::cout << "Book ID: " << pair.first << std::endl;
                book->printDetails();
                std::cout << std::endl;
                found = true;
            }
        }

        if (!found) {
            std::cout << "No books found matching the query.\n";
        }
    }

    // Borrow a book by ID for a specific borrower
    bool borrowBook(int id, const std::string& borrower) {
        auto book = findBook(id);
        if (book && book->getAvailability()) {
            book->setAvailability(false);
            std::time_t now = std::time(nullptr);
            std::time_t dueDate = now + 14 * 24 * 60 * 60; // 14 days from now
            book->setDueDate(dueDate);
            borrowedBooks[borrower].push_back(id);
            return true;
        }
        return false;
    }

    // Return a book by ID
    bool returnBook(int id, const std::string& borrower) {
        auto book = findBook(id);
        if (book && !book->getAvailability()) {
            book->setAvailability(true);
            auto& books = borrowedBooks[borrower];
            books.erase(std::remove(books.begin(), books.end(), id), books.end());
            return true;
        }
        return false;
    }

    // Print details of books borrowed by a specific borrower
    void printBorrowedBooks(const std::string& borrower) const {
        auto it = borrowedBooks.find(borrower);
        if (it != borrowedBooks.end()) {
            for (int id : it->second) {
                auto book = findBook(id);
                if (book) {
                    std::cout << "Book ID: " << id << std::endl;
                    book->printDetails();
                    std::cout << std::endl;
                }
            }
        }
        else {
            std::cout << "No books borrowed by " << borrower << ".\n";
        }
    }

private:
    std::unordered_map<int, std::shared_ptr<Book>> books;            // Map of book ID to Book object
    std::unordered_map<std::string, std::vector<int>> borrowedBooks; // Map of borrower to list of borrowed book IDs
    int nextId = 1;                                                  // ID generator for books
};

// Clear the console screen
void clearConsole() {
#ifdef _WIN32
    system("cls");      // Command to clear screen in Windows
#else
    system("clear");    // Command to clear screen in Unix-based systems
#endif
}
    
// Print the employee menu
void printEmployeeMenu() {
    clearConsole();
    std::cout << "\nLibrary Management System\n";
    std::cout << "1. Add a new book\n";
    std::cout << "2. Remove a book by ID\n";
    std::cout << "3. Find a book by ID\n";
    std::cout << "4. Print all books\n";
    std::cout << "5. Exit\n";
    std::cout << "Enter your choice: ";
}

// Handle employee menu actions
void handleEmployeeMenu(Library& library) {
    int choice;
    while (true) {
        printEmployeeMenu();
        std::cin >> choice;

        if (choice == 1) {
            std::string title, author, isbn;
            int publicationYear;
            std::cout << "Enter title: ";
            std::cin.ignore();
            std::getline(std::cin, title); // Use getline to handle spaces in input
            std::cout << "Enter author: ";
            std::getline(std::cin, author); // Use getline to handle spaces in input
            std::cout << "Enter ISBN: ";
            std::getline(std::cin, isbn); // Use getline to handle spaces in input
            std::cout << "Enter publication year: ";
            std::cin >> publicationYear;

            int id = library.addBook(title, author, isbn, publicationYear);
            std::cout << "Book added with ID: " << id << std::endl;

        }
        else if (choice == 2) {
            int id;
            std::cout << "Enter book ID to remove: ";
            std::cin >> id;
            if (library.removeBook(id)) {
                std::cout << "Book removed.\n";
            }
            else {
                std::cout << "Book not found.\n";
            }

        }
        else if (choice == 3) {
            int id;
            std::cout << "Enter book ID to find: ";
            std::cin >> id;
            auto book = library.findBook(id);
            if (book) {
                book->printDetails();
            }
            else {
                std::cout << "Book not found.\n";
            }

        }
        else if (choice == 4) {
            library.printAllBooks();

        }
        else if (choice == 5) {
            std::cout << "Exiting the library system.\n";
            break;

        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }

        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();     // Wait for user to press enter
    }
}

// Print the borrower menu
void printBorrowerMenu() {
    clearConsole();
    std::cout << "\nBorrower System\n";
    std::cout << "1. Search for a book\n";
    std::cout << "2. Borrow a book\n";
    std::cout << "3. Return a book\n";
    std::cout << "4. List all available books\n";
    std::cout << "5. List my borrowed books\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: ";
}

// Handle borrower menu actions
void handleBorrowerMenu(Library& library) {
    int choice;
    std::string borrower;

    std::cout << "Enter your name: ";
    std::cin.ignore();
    std::getline(std::cin, borrower);

    while (true) {
        printBorrowerMenu();
        std::cin >> choice;

        if (choice == 1) {
            std::string query;
            std::cout << "Enter search query (title, author, ISBN, ID, or publication year): ";
            std::cin.ignore();
            std::getline(std::cin, query);
            library.searchBooks(query);

        }
        else if (choice == 2) {
            int id;
            std::cout << "Enter book ID to borrow: ";
            std::cin >> id;
            if (library.borrowBook(id, borrower)) {
                std::cout << "Book borrowed successfully.\n";
            }
            else {
                std::cout << "Book is not available or not found.\n";
            }

        }
        else if (choice == 3) {
            int id;
            std::cout << "Enter book ID to return: ";
            std::cin >> id;
            if (library.returnBook(id, borrower)) {
                std::cout << "Book returned successfully.\n";
            }
            else {
                std::cout << "Book was not borrowed or not found.\n";
            }

        }
        else if (choice == 4) {
            library.printAvailableBooks();

        }
        else if (choice == 5) {
            library.printBorrowedBooks(borrower);

        }
        else if (choice == 6) {
            std::cout << "Exiting the borrowing system.\n";
            break;

        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }

        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
}

// Print the main menu
void printMainMenu() {
    clearConsole();
    std::cout << "\nWelcome to the Library\n";
    std::cout << "1. Enter as a person who wants to borrow a book\n";
    std::cout << "2. Enter as an employee\n";
    std::cout << "Enter your choice: ";
}

// Preload some books into the library for testing
// with predefined titles, authors, ISBNs, and publication years
void preloadBooks(Library& library) {
    library.addBook("The Great Gatsby", "F. Scott Fitzgerald", "9780743273565", 1925);
    library.addBook("To Kill a Mockingbird", "Harper Lee", "9780060935467", 1960);
    library.addBook("1984", "George Orwell", "9780451524935", 1949);
    library.addBook("Pride and Prejudice", "Jane Austen", "9781503290563", 1813);
    library.addBook("The Catcher in the Rye", "J.D. Salinger", "9780316769488", 1951);
    library.addBook("The Hobbit", "J.R.R. Tolkien", "9780547928227", 1937);
    library.addBook("Fahrenheit 451", "Ray Bradbury", "9781451673319", 1953);
    library.addBook("Moby Dick", "Herman Melville", "9781503280786", 1851);
    library.addBook("War and Peace", "Leo Tolstoy", "9781400079988", 1869);
    library.addBook("The Odyssey", "Homer", "9780140268867", -800); // approximate year
}

int main() {
    Library library;
    preloadBooks(library); // Preload books for testing
    int choice;

    while (true) {
        printMainMenu();
        std::cin >> choice;

        if (choice == 1) {
            handleBorrowerMenu(library);
        }
        else if (choice == 2) {
            handleEmployeeMenu(library);
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
    }

    return 0;
}

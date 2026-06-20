//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jeri Mabuti
// Version     : 1.0
// Description : 7-1 Submit Project Two
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// define structure for a course object
struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;
};

// define node structure for the hash table chain
struct Node {
    Course course;
    Node* next;

    Node() {
        next = nullptr;
    }

    Node(Course aCourse) {
        course = aCourse;
        next = nullptr;
    }
};

// hash table with a fixed-size array of buckets
class HashTable {
private:
    static const unsigned int DEFAULT_SIZE = 179;
    vector<Node*> table;

    // convert courseNumber string to an integer key in range [0, TABLE_SIZE)
    unsigned int hash(const string& courseNumber) const {
        unsigned int value = 0;

        for (char ch : courseNumber) {
            if (isdigit(ch)) {
                value = value * 10 + (ch - '0');
            }
        }

        return value % table.size();
    }

public:
    HashTable() {
        // initialize each bucket as empty
        table.resize(DEFAULT_SIZE, nullptr);
    }

    ~HashTable() {
        // free all nodes in each chain
        for (Node* head : table) {
            while (head != nullptr) {
                Node* temp = head;
                head = head->next;
                delete temp;
            }
        }
    }

    // insert one course into the hash table using chaining
    void insert(Course course) {
        unsigned int key = hash(course.courseNumber);
        Node* newNode = new Node(course);

        // empty bucket: place node at this bucket
        if (table[key] == nullptr) {
            table[key] = newNode;
        }
        else {
            // otherwise walk to the end of the chain and append a new node
            Node* current = table[key];

            while (current->next != nullptr) {
                current = current->next;
            }

            current->next = newNode;
        }
    }

    // find a course by courseNumber in the hash table
    Course* search(const string& courseNumber) {
        unsigned int key = hash(courseNumber);
        Node* current = table[key];

        // walk the chain to look for a matching courseNumber
        while (current != nullptr) {
            if (current->course.courseNumber == courseNumber) {
                return &(current->course);
            }

            current = current->next;
        }

        return nullptr;
    }

    // collect all courses from the hash table
    vector<Course> getAllCourses() const {
        vector<Course> courses;

        for (Node* head : table) {
            Node* current = head;

            while (current != nullptr) {
                courses.push_back(current->course);
                current = current->next;
            }
        }

        return courses;
    }
};

// remove leading and trailing whitespace
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

// split one line into comma-separated tokens
vector<string> split(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// open the file, read and parse lines, and build course objects
bool loadCoursesFromFile(const string& fileName, HashTable& courseTable) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: could not open file." << endl;
        return false;
    }

    string line;

    // read each line from the input file
    while (getline(file, line)) {
        // skip empty lines
        if (trim(line).empty()) {
            continue;
        }

        // split the line into comma-separated tokens
        vector<string> tokens = split(line);

        // must have at least courseNumber and title
        if (tokens.size() < 2) {
            cout << "Error: invalid line format." << endl;
            continue;
        }

        // create new Course object
        Course course;
        course.courseNumber = tokens[0];
        course.title = tokens[1];

        // remaining tokens (if any) are prerequisites
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(tokens[i]);
            }
        }

        // insert course into hash table
        courseTable.insert(course);
    }

    file.close();
    return true;
}

// sort and print a list of all courses in alphanumeric order
void printCourseList(HashTable& courseTable) {
    vector<Course> courses = courseTable.getAllCourses();

    // sort the course information alphanumerically from lowest to highest
    sort(courses.begin(), courses.end(), [](const Course& a, const Course& b) {
        return a.courseNumber < b.courseNumber;
        });

    // print the sorted list to a display
    cout << "\nHere is a sample schedule:" << endl;

    for (const Course& course : courses) {
        cout << course.courseNumber << ", " << course.title << endl;
    }
}

// print the title and prerequisites for a single course using the hash table
void printCourseInformation(HashTable& courseTable, const string& courseNumber) {
    Course* course = courseTable.search(courseNumber);

    if (course == nullptr) {
        cout << "Course " << courseNumber << " not found." << endl;
        return;
    }

    // print main course information
    cout << course->courseNumber << ", " << course->title << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";

        // print each prerequisite if there are any
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            Course* prereqCourse = courseTable.search(course->prerequisites[i]);

            if (prereqCourse != nullptr) {
                cout << prereqCourse->courseNumber << ", " << prereqCourse->title;
            }
            else {
                cout << course->prerequisites[i];
            }

            if (i < course->prerequisites.size() - 1) {
                cout << "; ";
            }
        }

        cout << endl;
    }
}

int main() {
    HashTable courseTable;
    string fileName;
    string choice;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;
    cout << "Enter the file name: ";
    getline(cin, fileName);

    while (choice != "9") {
        cout << "\nMenu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";
        getline(cin, choice);

        if (choice == "1") {
            // load the file data into the data structure
            dataLoaded = loadCoursesFromFile(fileName, courseTable);

            if (dataLoaded) {
                cout << "Data loaded successfully." << endl;
            }
        }
        else if (choice == "2") {
            // before printing the course list, data must be loaded
            if (!dataLoaded) {
                cout << "Please load the data file first." << endl;
            }
            else {
                printCourseList(courseTable);
            }
        }
        else if (choice == "3") {
            // before printing course information, data must be loaded
            if (!dataLoaded) {
                cout << "Please load the data file first." << endl;
            }
            else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);
                printCourseInformation(courseTable, courseNumber);
            }
        }
        else if (choice == "9") {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            // display an error message when input does not fall within parameters
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
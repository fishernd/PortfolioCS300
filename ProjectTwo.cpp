/*****************************************************************************
 * Name        : ProjectTwo.cpp
 * Author      : Nathan Fisher
 * Version     : 1.0
 * Copyright   : Copyright © 2025 Nathan Fisher
 * Description : Project Two advising assistance program
 *****************************************************************************/

/*
 * Design Brief
 *
 * The main list of courses is implemented using a Binary Search Tree, allowing
 * for O(h) insertion complexity, where h is the height of the tree. So long as
 * the tree is relatively balanced this will be quick. Because the tree is sorted,
 * lookups are also O(h), and printing the sorted list of courses is O(n).
 *
 * There is a secondary list, which is only used for validation of the prerequisite
 * courses during the loading process. We need to maintain a (temporary) list of
 * prerequisites as each course is loaded, and then once loading is complete check
 * to ensure that those prerequisite courses actually exist. This could have been
 * a simple vector, however, by implementing a minimal hash table we gain the
 * benefit of not having to check the same course multiple times, as the hash table
 * will not have duplicates.
 *
 * This is not a production-ready HashMap, just a quickly knocked together structure
 * which should meet the requirements in as efficient a manner as possible. The
 * default size for the internal array was chosen by trial and error so that the
 * simple modulo based hash produced no collisions with the data we were given and
 * to keep the space requirement small. A larger data set would by necessity
 * require a larger capacity, and possibly the ability to dynamically grow as
 * needed.
 *
 * Compiling
 *
 * This program was designed with the assumption that the data being input is the
 * data we were given as testing. It uses a default capacity for the prerequisites
 * hash table of 27. If a larger dataset is to be loaded, this should be changed
 * to a higher number. For the best chance of avoiding hash collisions, choose
 * a number which is not a power of 2, preferably odd. Depending on the compiler,
 * this might be able to be passed into the compiler in the CPPFLAGS environment
 * variable. Otherwise, edit the `DEFAULT_PREREQUISITE_TABLE_SIZE` macro in this
 * file.
 */

#ifndef DEFAULT_PREREQUISITE_TABLE_SIZE
#define DEFAULT_PREREQUISITE_TABLE_SIZE 27
#endif // !DEFAULT_PREREQUISITE_TABLE_SIZE

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

#ifdef _WIN32
   #include <io.h> 
   #define access    _access_s
#else
   #include <unistd.h>
#endif // _WIN32

using namespace std;

/**
 * Represents a course prerequisite by course number
 */
struct Prerequisite {
    /**
     * An anonymous union which can represent this course by courseId string
     * or as an unsigned 64-bit integer. Conveniently, the course id numbers
     * are 7 digits, so 7 bytes plus null byte is 8 bytes, or a 64 bit int. This
     * eneables simple hashing, as we can just treat the data held by the string
     * as if it were already a number and just hash that using modulo.
     */
    union {
        char      courseId[8];
        u_int64_t num;
    };

    /**
     * Constructor
     */
    Prerequisite() {
        this->num = 0;
    }

    /**
     * Clears the data held by the structure
     */
    void clear() {
        this->num = 0;
    }

    /**
     * Loads the given courseId string into the structure
     */
    void load(string id) {
        id.copy(this->courseId, 7);
    }

    /**
     * Hashes the value for insertion into a hash table. We access the union as
     * a 64 bit unsigned integer, add `idx` and take the modulo of that value and
     * the table capacity
     */
    u_int64_t hash(u_int64_t idx, u_int64_t cap) {
        return (this->num + idx) % cap;
    }

    /**
     * Converts the data back into a c++ `string`
     */
    string getString() {
        // Create a new string from the union, accessed as a byte array
        string out(this->courseId);
        return out;
    }

    bool empty() {
        return this->num == 0 ? true : false;
    }
};

/**
 * A simple hash table containing a list of prerequisites. This table uses open addressing
 * and a simple modulo-based hash. The chief benefit over using an array or vector
 * is that we avoid duplicates, otherwise each duplicate would cause another tree
 * traversal when validating the list of prerequisites. Admittedly, there is minimal
 * benefit with such a small tree, but the larger the tree the more benefit.
 */
class PrereqHashTable {
    private:
        u_int64_t     len;      //! The number of elements currently in use
        u_int64_t     capacity; //! The total number of elements which can be stored
        Prerequisite *items;    //! A dynamically allocated array used to store the elements

    public:
        PrereqHashTable();              //! Constructor
        PrereqHashTable(u_int64_t cap); //! Constructor, with capacity specifier
        virtual ~PrereqHashTable();     //! Destructor
        u_int64_t getCapacity();        //! Getter for the `capacity` property
        Prerequisite *getItems();       //! Getter for the `items` field
        void insert(string courseId);   //! Inserts a new course ID
};

/**
 * Default constructor
 */
PrereqHashTable::PrereqHashTable() {
    // Just zero everything
    this->capacity = 0;
    this->len = 0;
    this->items = nullptr;
}

/**
 * Constructor which also sets the capacity and allocates the internal array
 * \param cap The capacity to allocate
 */
PrereqHashTable::PrereqHashTable(u_int64_t cap) : PrereqHashTable() {
    // Set capacity to `cap`
    this->capacity = cap;
    // Allocate the internal array with size `cap`
    this->items = new Prerequisite[cap];
}

/**
 * Destructor
 */
PrereqHashTable::~PrereqHashTable() {
    // De-allocate the internal array
    delete[] this->items;
}

/**
 * Getter
 * \return the capacity of this structure
 */
u_int64_t PrereqHashTable::getCapacity() {
    return this->capacity;
}

/**
 * Getter
 * \return the internal array of items
 */
Prerequisite* PrereqHashTable::getItems() {
    return this->items;
}

/**
 * Inserts a new course ID number into the table
 * \param courseId the course ID number to insert
 */
void PrereqHashTable::insert(string courseId) {
    Prerequisite prereq;
    u_int32_t    hash;
    u_int32_t    idx;

    // Load the string into the `Prerequisite` union (treating it as a byte array)
    prereq.load(courseId);

    // Iterate for the full capacity, in case of hash collisions
    for (idx = 0; idx < this->capacity; idx++) {
        // Hash the prerequisite
        hash = prereq.hash(idx, this->capacity);

        // If we find a match, just break and exit. Nothing to do, we don't want
        // duplicates anyway.
        if (this->items[hash].num == prereq.num) {
            break;
        } else if (this->items[hash].num == 0) {
            // Found an empty slot, copy the data to the item at this index
            this->items[hash].num = prereq.num;
            break;
        }
    }
}

struct Course {
    string         number;
    string         title;
    vector<string> prerequisites;

    // Clears all of the fields
    void clear() {
        number.clear();
        title.clear();
        prerequisites.clear();
    }

    /**
     * Initializes this Course with the data from a line of text read from the
     * CSV file.
     *
     * \param line the line of text from which to parse the data
     * \param table a temporary hash table to track course prerequisites
     */
    void init(string line, PrereqHashTable *table) {
        char delim = ',';
        stringstream stream(line);

        // Use getline with a comma delimiter to read the first field
        getline(stream, number, delim);
        /* We know that all course numbers are 7 characters long. Anything else
         * is invalid input, so throw an exception
         */
        if (number.size() != 7) {
            this->clear();
            throw runtime_error("Error: invalid course number");
        }

        /**
         * Again, use getline with a comma delimiter to read the field
         */
        getline(stream, title, delim);
        // Assume any string is a valid title, unless it is emopty
        if (title.empty()) {
            this->clear();
            throw runtime_error("Error: empty course title");
        }

        // Read prerequisites until the end of the line
        while (!stream.eof()) {
            string pr;

            getline(stream, pr, delim);

            /* The joys of cross-platform line endings. MS uses cr/lf, so if the
             * CSV file was created in Windows but processed on some Unix(ish)
             * system we will get a final string which is made up of just a
             * carriage return character. So we handle either that case or the
             * case of an empty string
             */
            if ((pr.size() == 1 && pr[0] == '\r') || pr.empty()) {
                continue;
            } else {
                // Add `pr` to both this Course structure and the temporary hash table
                prerequisites.push_back(pr);
                table->insert(pr);
            }
        }
    }

    /**
     * Display the basic info, used for listing all courses
     */
    void display() {
        cout << number << ", " << title << endl;
    }

    /**
     * Display a more detailed course listing, with prerequisites
     */
    void displayDetails() {
        bool comma = false;

        // Display the basic info
        this->display();

        // Skip if no prerequisites
        if (!prerequisites.empty()) {
            cout << "Prerequisites:";
            for (string c : prerequisites) {
                // `comma` will only be false on the first iteration
                if (comma)
                    cout << ", " << c;
                else
                    cout << " " << c;
                comma = true;
            }
            cout << endl;
        }
    }

    /**
     * Utility function, determines if the Course structure is in use
     */
    bool empty() {
        if (this->number.empty())
            return true;
        else
            return false;
    }
};

/**
 * A Binary Search Tree Node containing a `Course` data structure
 */
struct Node {
    Course  course;
    Node   *left;
    Node   *right;

    /**
     * Constructor
     */
    Node() {
        // Just set both children to NULL
        left = nullptr;
        right = nullptr;
    }

    /**
     * Constructor
     */
    Node(Course course) : Node() {
        this->course = course;
    }

    /**
     * Destructor
     */
    virtual ~Node() {
        // Recursively delete all children
        delete left;
        delete right;
    }
};

/**
 * A Binary Search Tree for `Course` data structures, using the Course ID number
 * as the key element
 */
class BinarySearchTree {
    private:
        Node *root;
        void  addNode(Node *node, Course course);
        void  inOrder(Node *node);

    public:
        BinarySearchTree();
        virtual ~BinarySearchTree();
        void     drain();
        void     InOrder();
        void     Insert(Course course);
        Course   Search(string courseNumber);
        bool     Exists(string courseNumber);
};

/**
 * Add a node to the tree as a child of `node`. Recursive function which will
 * run until the proper spot is found for insertion.
 *
 * \param node the node under which this course should be inserted.
 * \param course the course to be inserted
 */
void BinarySearchTree::addNode(Node *node, Course course) {
    // Case 1: this course's number is less than that of `node->course`
    if (course.number < node->course.number) {
        if (node->left == nullptr) {
            // Open spot found, insert here
            node->left = new Node(course);
        } else {
            // Recurse left
            addNode(node->left, course);
        }
    /* Case 2: this course's number is greater than that of `node->course`.
     * Technically speaking this branch would run were we to add a course with
     * a matching number, but we will not ever be doing that because there are
     * no duplicates in the csv data, and subsequent runs of `loadCourses` will
     * empty the tree before reloading it */
    } else {
        if (node->right == nullptr) {
            // open spot found, insert here
            node->right = new Node(course);
        } else {
            // recurse right
            addNode(node->right, course);
        }
    }
}

/**
 * Recurse from `node` in alphanumeric order (a Binary Search Tree inOrder
 * traversal). Print the basic info from each course as it is visited.
 *
 *\param node the node to start with
 */
void BinarySearchTree::inOrder(Node *node) {
    // Bail on encountering a NULL node
    if (node == nullptr)
        return;

    // recurse right
    inOrder(node->left);
    // print info
    node->course.display();
    // recurse right
    inOrder(node->right);
}

/**
 * Constructor
 */
BinarySearchTree::BinarySearchTree() {
    // empty tree, make sure `root` is set to NULL
    root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    // Since we alread have a function which will recursively call delete on
    // the nodes, starting with this->root, just call that method */
    drain();
}

/**
 * Recursively deletes all nodes in the tree, beginning with `root`, then sets
 * this->root equal to NULL
 */
void BinarySearchTree::drain() {
    delete root;
    root = nullptr;
}

/**
 * Begin the inOrder tree traversal, starting with the root node
 */
void BinarySearchTree::InOrder() {
    this->inOrder(root);
}

/**
 * Insert a course into the BST
 * \param course the course to be inserted
 */
void BinarySearchTree::Insert(Course course) {
    if (this->root == nullptr) {
        // Empty tree, make this the root
        this->root = new Node(course);
    } else {
        // Traverse the tree to find the correct spot to insert this course
        this->addNode(this->root, course);
    }
}

/**
 * Searches the BST for a course with a matching ID number
 * \param courseNumber the course id number to look for
 */
Course BinarySearchTree::Search(string courseNumber) {
    Node *currentNode = this->root;

    // If currentNode is null, then we have traversed the entire tree without
    // finding a match. Loop until then.
    while (currentNode != nullptr) {
        if (currentNode->course.number == courseNumber) {
            // match found
            return currentNode->course;
        } else {
            // current node is not a match. If courseNumber is less, go left, if
            // greater go right
            currentNode = (courseNumber < currentNode->course.number) ? currentNode->left
                : currentNode->right;
        }
    }

    // No match found, return an empty course
    Course course;
    return course;
}

/**
 * Check whether this course exists in the tree
 * \param courseNumber the course id number to validate
 */
bool BinarySearchTree::Exists(string courseNumber) {
    // use the search function
    Course course = this->Search(courseNumber);
    // An empty course means nothing was found, while nonempty means we have
    // a match
    return !course.number.empty();
}

/**
 * An enum representing valid choices for user input to the main menu
 */
typedef enum {
    LoadCourses    = 1,
    DisplayCourses = 2,
    FindCourse     = 3,
    Exit           = 9,
}   MenuChoice;

/**
 * The Driver class runs the main loop of the program
 */
class Driver {
    private:
        string csvPath; // the path to the csv file where course data is found
        const char * menuText =
            "\n  /==============================\\\n"
            "  |  Menu                        |\n"
            "  |    1. Load Courses           |\n"
            "  |    2. Display Courses        |\n"
            "  |    3. Find Course by number  |\n"
            "  |    9. Exit                   |\n"
            "  \\==============================/\n";

        BinarySearchTree  tree;        // The BST where course information will be stored

        PrereqHashTable  *prereqTable; // The hash table where prerequisites will be stored
                                       // to validate that they match to actual courses

        bool checkPrerequisites();     // Checks whether all of the prerequisite courses
                                       // are valid or not

    public:
        Driver();                   // Base constructor
        Driver(string csvPath);     // Constructor, with csvpath parameter
        virtual    ~Driver();       // Destructor
        MenuChoice  menu();         // Displays the menu and gets the user's selection
        void        loadCourses();  // Loads the courses from csvfile
        void        printCourses(); // Prints the courses in alphanumeric order
        void        search();       // Searches for a course by its ID number
        void        run();          // Run the main program loop
};

/**
 * Validates that all prerequisites are valid courses
 */
bool Driver::checkPrerequisites() {
    int           i;
    Prerequisite *prerequisites;

    // Get the internal array of prerequisites from the hash table
    prerequisites = this->prereqTable->getItems();

    // loop over the array
    for (i = 0; i < prereqTable->getCapacity(); i++) {
        // Only check non-empty prerequisites
        if (!prerequisites[i].empty()) {
            // Copy the data to a string for comparison
            string course = prerequisites[i].getString();
            // If any course fails the check, bail and exit
            if (!tree.Exists(course)) {
                cerr << "Prerequisite course " << course << " does not exist";
                return false;
            }
        }
    }
    // All prerequisites accounted for, return true
    return true;
}

/**
 * Base constructor
 */
Driver::Driver() {
    prereqTable = new PrereqHashTable(DEFAULT_PREREQUISITE_TABLE_SIZE);
}

/**
 * Constructor, with csv path parameter
 * \param csvPath the path to the csv data file
 */
Driver::Driver(string csvPath) : Driver() {
    this->csvPath = csvPath;
}

/**
 * Destructor
 */
Driver::~Driver() {
    // Delete the hash table which we allocated on the stack
    delete prereqTable;
}

/**
 * Displays the menu and validates the user input to be a valid `MenuChoice`,
 * looping until we get a valid choice from the user
 * \return the `MenuChoice` chosen by the user
 */
MenuChoice Driver::menu() {
    MenuChoice choice;
    int c;
    string s;

    for (;;) {
        // Display the menu
        cout << menuText << endl;
        cout << "Enter choice: ";
        // Grab a line of text
        getline(cin, s);

        /* Run the integer conversion in a try block. If we catch an error, then
         * display an error message on stderr and loop again */
        try {
            c = stoi(s);
        } catch (invalid_argument &e) {
            cerr << s << " is not a valid option." << endl;
            continue;
        }

        /* We know we have a valid integer, now validate that it is a member of
         * the `MenuChoice` enumeration. If it is, return the value. If not,
         * display an error message on stderr and loop again */
        switch (c) {
            case LoadCourses:
            case DisplayCourses:
            case FindCourse:
            case Exit:
                return (MenuChoice)c;
            default:
                cerr << c << " is not a valid option." << endl;
                continue;
        }
    }
}

/**
 * Load the course information from the csv file
 */
void Driver::loadCourses() {
    ifstream infs;
    string   line;
    int      num;

    num = 0;
    /* If this function is called a second time, then it is necessary to empty
     * the tree out before refilling it */
    tree.drain();

    // open the csv file for reading
    infs.open(csvPath);
    // Iterate over the lines in the file
    while (getline(infs, line)) {
        /* Create and initialize a new Course using this line and the
         * `Course::init` method */
        Course course;
        course.init(line, prereqTable);

        // Add this course to the tree
        tree.Insert(course);
        num++;
    }
    // Make sure to close resources after use
    infs.close();
    if (checkPrerequisites() == false)
        throw runtime_error("Prerequisite course check failed");
    cout << "\nLoaded " << num << " courses" << endl;
}

/**
 * Print the course schedule in alphanumeric order
 */
void Driver::printCourses() {
    cout << "\n  Here is a sample schedule:\n" << endl;
    /* performs an inorder traversal of the BST, printing the basic course information
     * for each node as it is visited */
    tree.InOrder();
}

/**
 * Finds a coure by its ID number, taken from user input, and displays the
 * course details
 */
void Driver::search() {
    string courseNumber;

    cout << "What course do you want to know about? ";

    // Read the course number from stdin into a string
    getline(cin, courseNumber);

    /* if the course number is empty or the size is not 7 characters, this is not
     * a valid course ID number */
    if (courseNumber.empty() || courseNumber.size() != 7) {
        cerr << "\nInvalid course number" << endl;
    } else {
        // Do the search
        Course course = this->tree.Search(courseNumber);

        /* Uses the `Course.empty()` helper method to be sure the returned course
         * is not empty */
        if (course.empty()) {
            cout << "\nNo matching course found." << endl;
        } else {
            cout << endl;
            course.displayDetails();
        }
    }
}

void Driver::run() {
    MenuChoice choice;

    cout << "Welcome to the course planner." << endl;

    do {
        choice = this->menu();
        switch (choice) {
        case LoadCourses:
            this->loadCourses();
            break;
        case DisplayCourses:
            this->printCourses();
            break;
        case FindCourse:
            this->search();
            break;
        case Exit:
            break;
        }
    } while (choice != Exit);

    cout << "\nThank you for using the course planner!\n" << endl;
}

int main(int argc, char *argv[]) {
    Driver *driver;
    string  csvPath;

    if (argc > 1) {
        csvPath = argv[1];
    } else {
        cout << "Please enter the path to the csv data file [CS 300 ABCU_Advising_Program_Input.csv]:" << endl;
        getline(cin, csvPath);
        if (csvPath.empty()) {
            csvPath = "CS 300 ABCU_Advising_Program_Input.csv";
        } else if (access(csvPath.c_str(), 0)) {
            cerr << "File " << csvPath << " does not exist" << endl;
            return 1;
        }
    }

    // Create a new Driver and run it
    driver = new Driver(csvPath);
    driver->run();

    delete driver;
    return 0;
}

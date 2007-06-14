/*
 * Author: lesleyn@google.com (Lesley Northam)
 */

#include "pp_path.h"
#include <iostream>

using namespace std;

#define ERROR(msg) \
        cerr << "ERROR: [" __FILE__ << ":" << __LINE__ << "] " << msg << endl

/* A Test to Create a pp_path */
int create_local_path() {
    int return_value = 0;
    pp_path path;

    /* Generate pp_path Information */
    path.push_back("One");
    path.push_back("Two");
    path.push_back("Three");

    /* create a path iterator */
    pp_path::iterator my_iterator;
    my_iterator = path.begin ();

    /* test created path */
    if ((*my_iterator).compare("One") != 0) {
	ERROR("path::push_back(\"One\") failed");
	return_value++;
    }

    my_iterator++;

    if ((*my_iterator).compare("Two") != 0) {
	ERROR("path::push_back(\"Two\") failed");
	return_value++;
    }

    my_iterator++;

    if ((*my_iterator).compare("Three") != 0) {
	ERROR("path::push_back(\"Three\") failed");
	return_value++;
    }

    if (path.size() != 3) {
	ERROR("path::size() failed");
	return_value++;
    }

    /* pop items */
    path.pop_front();
    path.pop_front();
    path.pop_front();

    if (!path.empty()) {
	ERROR("path::empty() failed");
	return_value++;
    }

    return return_value;
}

/* A Test to Create a pp_path */
int create_const_path() {
    int return_value = 0;
    pp_path path;
    const pp_path &const_path = path;
    path.push_back("One");
    path.push_back("Two");
    path.push_back("Three");

    if (const_path.empty()) {
	ERROR("const path empty");
	return_value++;
    }

    if (path.size() != 3) {
	ERROR("path::size() failed");
	return_value++;
    }

    if (const_path.size() != 3) {
	ERROR("const_path::size() failed");
	return_value++;
    }

    pp_path::iterator my_iterator;
    pp_path::const_iterator const_iterator;
    my_iterator = path.begin();
    const_iterator = const_path.begin();

    if (my_iterator->compare(*const_iterator) != 0) {
	ERROR("first item incorrect");
	return_value++;
    }
    
    my_iterator++;
    const_iterator++;

    if (my_iterator->compare(*const_iterator) != 0) {
	ERROR("second item incorrect");
	return_value++;
    }

    my_iterator++;
    const_iterator++;

    if (my_iterator->compare(*const_iterator) != 0) {
	ERROR("third item incorrect");
	return_value++;
    }
  
    path.pop_front();

    if (const_path.size() != 2) {
	ERROR("path::pop_front() failed");
	return_value++;
    }

    return return_value;
}

/* Comparison Tests */
int comparison_test() {
    pp_path pathA, pathB, pathC, pathD, pathE;
  
    int return_value = 0;
    
    /* Generate pp_path Information */
    pathA.push_back("One");
    pathA.push_back("Two");
    pathA.push_back("Three");  
    pathB.push_back("One");
    pathB.push_back("Two");
    pathC.push_back("One");
    pathC.push_back("Two");
    pathC.push_back("Three");
    pathC.push_back("Four");
    pathD.push_back("One");
    pathD.push_back("Seven");
    pathD.push_back("Three");
    pathE.push_back("One");
    pathE.push_back("Two");
    pathE.push_back("Three");

    if (pathA.equals(pathB) == false) {
    } else {
	ERROR("patha == pathb");
	return_value++;
    }

    if (pathA.equals(pathC) == false) {
    } else {
	ERROR("patha == pathc");
	return_value++;
    }

    if (pathA.equals(pathD) == false) {
    } else {
	ERROR("patha == pathd");
	return_value++;
    }  

    if (pathA.equals(pathE) == true) {
    } else {
	ERROR("patha != pathe");
	return_value++;
    }

    return return_value;
}

int test_string_constructor() {
int return_value = 0;
    
    /* try to create some paths using a string in the constructor */
    pp_path path1("/RED/GREEN/BLUE");
    pp_path path2("red/orange/yellow/green/blue/purple/pink");
    pp_path path3("//RED//YELLOW//BLUE//");
    pp_path path4("//////////");
    pp_path path5("");
    pp_path pathA;
    pp_path pathB;
    pp_path pathC;
    pp_path pathD;

    pathA.push_back("RED");
    pathA.push_back("GREEN");
    pathA.push_back("BLUE");
    pathB.push_back("red");
    pathB.push_back("orange");
    pathB.push_back("yellow");
    pathB.push_back("green");
    pathB.push_back("blue");
    pathB.push_back("purple");
    pathB.push_back("pink");
    pathC.push_back("RED");
    pathC.push_back("YELLOW");
    pathC.push_back("BLUE");
    
    /* Check correctness of paths */
    if (!pathA.equals(path1)) {
	ERROR("string constructor failure 1");
	return_value++;
    }
    if (!pathB.equals(path2)) {
	ERROR("string constructor failure 2");
	return_value++;
    }
    if (!pathC.equals(path3)) {
	ERROR("string constructor failure 3");
	return_value++;
    }
    if (!pathD.equals(path4)) {
	ERROR("string constructor failure 4");
	return_value++;
    }
    if (!pathD.equals(path5)) {
	ERROR("string constructor failure 5");
	return_value++;
    } 
    return return_value;
}

int main() {
    int error_count = 0;
    error_count += create_local_path();
    error_count += create_const_path();
    error_count += comparison_test();
    error_count += test_string_constructor();
    return error_count;
}

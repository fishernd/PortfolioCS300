CS 300 Project
==============

Contents
========
- [Rubric](#rubric)
- [Design](#design)
- [Building](#building)

## Rubric
The scenario for this project is a hypothetical Computer Science deparment at a
hypothetical university, which is in need of an advising assistance program. The
program must be able to read input from a CSV file, validte that input, display
a menu, take and validate user input, and react appropriately. The menu choices
are:

1. Load the data from CSV
2. Print courses in alphanumeric order
3. Display details for a specific course
9. Exit the program

As part of the design process, we were tasked with creating a runtime analysis of
three possible data structures which could be used to complete the requirements.

## Design
The courses are read from CSV file and stored in a bespoke Binary Search Tree.
Prerequisite courses must be validated (that they actually exist). In order to
perform that task each prerequisite is loaded into a bespoke Hash Table. After
the BST is populated, the program iterates over the values stored in the Hash Table,
verifying that a matching course is found in the tree. Using a Hash Table ensures
that each prerequisite course only has to be verified once, as the table rejects
duplicate entries.

## Building
There is a GNU-style Makefile for building the program and corresponding runtime
analysis. The runtime analysis can be exported from markdown to PDF, html or MS
word .docx formats using pandoc (and Tex for PDF).

+--------+-------------------------------------+
| Target | Artifact                            |
+:======:+:====================================+
| bin    | ProjectTwo                          |
| all    | All artifacts                       |
| docs   | All formats of the runtime analysis |
| html   | runtime_analysis.html               |
| pdf    | runtime_analysis.pdf                |
| docx   | runtime_analysis.docx               |
+--------+-------------------------------------+
table: Makefile targets

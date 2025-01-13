# toutlameche
A word prediction application developed in C.

## Project Overview
This application predicts the next word in a text sequence using N-gram based prediction. It analyzes previously entered text to build a prediction tree database, learning from word patterns and their frequencies of occurrence.

### Key Features
- N-gram based prediction algorithm
- Tree-based data structure for efficient word prediction
- Terminal-based text input and prediction
- Persistence system: save and load prediction tree

### Technical Details
- Developed in C
- Dynamic memory management
- Custom tree data structure for storing N-grams
- Custom file format for tree serialization and deserialization

### How it Works
The program:
1. Loads the prediction tree from file if available
2. Analyzes user input to identify word patterns
3. Organizes word sequences in a tree structure for efficient lookup
4. Predicts the next word based on the tree traversal
5. Dynamically updates the prediction tree with new input
6. Serializes and saves the prediction tree to file

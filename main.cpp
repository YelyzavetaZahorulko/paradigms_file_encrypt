#include <cstdio>
#include <cstdlib>
#include <stack>

#define INITIAL_CAPACITY 100
#define MAX_LINES 100
#define MAX_LINE_LENGTH 100

class TextContainer {
private:
    char* buffer; // for dynamic memory allocation
    int current_size;
    int capacity;


    static void myStrcpy(char* dest, const char* src, int length) { // копіює символи з одного рядка в інший
        for (int i = 0; i < length; i++) {
            dest[i] = src[i];
        }
    }

public:
    static int myStrlen(const char* str) { // прописана функція, що визначає довжину рядка
        int len = 0;
        while( str[len] != '\0') {
            len++;
        }
        return len;
    }

    TextContainer() {
        buffer = new char[INITIAL_CAPACITY];
        buffer[0] = '\0';
        current_size =0;
        capacity = INITIAL_CAPACITY;
    }

    TextContainer(const TextContainer& other) {
        current_size = other.current_size;
        capacity = other.capacity;
        buffer = new char[capacity];
        myStrcpy(buffer, other.buffer, current_size + 1);
    }

    TextContainer& operator=(const TextContainer& other) { // функція перевантаження оператора, для правильного виділення пам'яті
        if (this != &other) {
            delete[] buffer;
            current_size = other.current_size;
            capacity = other.capacity;
            buffer = new char[capacity];
            myStrcpy(buffer, other.buffer, current_size + 1);
        }
        return *this;
    }

    ~TextContainer() {
        delete[] buffer;
    }

    void resize(int new_capacity) {
        char* new_buffer = new char[new_capacity];
        myStrcpy(new_buffer, buffer, current_size);
        delete[] buffer;
        buffer = new_buffer;
        capacity = new_capacity;
    }

    void append(const char* text_to_append) {
        int append_length = myStrlen(text_to_append);
        if (current_size + append_length + 1 > capacity) {
            resize(current_size + append_length + 1);
        }

        for ( int i = 0; i < append_length; i++) {
            buffer[current_size + i] = text_to_append[i];
        }

        current_size += append_length;
        buffer[current_size] = '\0';
    }

    char* getBuffer() {
        return buffer;
    }

    int getCurrentSize() const {
        return current_size;
    }

    void insert(int index, const char* text_to_insert) {
        int insert_length = myStrlen(text_to_insert);
        if ( current_size + insert_length + 1 > capacity) {
            resize(current_size + insert_length +1);
        }

        for (int i = current_size - 1; i >= index; i--) {
            buffer[i + insert_length] = buffer[i];
        }
        // Insert the new text
        for (int i = 0; i < insert_length; i++) {
            buffer[index + i] = text_to_insert[i];
        }
        current_size += insert_length;
        buffer[current_size] = '\0';
    }

    void deleteText(int index, int count) {
        if (index < 0 || index >= current_size || count <= 0) {
            printf("Error: Invalid index or count.\n");
            return;
        }
        if (index + count > current_size) {
            count = current_size - index;
        }
        for (int i = index; i < current_size - count; i++) {
            buffer[i] = buffer[i + count];
        }
        current_size -= count;
        buffer[current_size] = '\0';
    }

    void insertReplacement(int index, const char* text_to_insert) {
        int insert_length = myStrlen(text_to_insert);
        if (index < 0 || index >= current_size) {
            printf("Error: Invalid index.\n");
            return;
        }
        int end_index = index + insert_length;
        for (int i = 0; i < insert_length && index + i < current_size; i++) {
            buffer[index + i] = text_to_insert[i];
        }
        if (end_index > current_size) {
            for (int i = current_size; i < end_index; i++) {
                buffer[i] = text_to_insert[i - index];
            }
            current_size = end_index;
            buffer[current_size] = '\0';
        }
    }

    void copyFrom(const TextContainer& other) {
        if (this != &other) {
            resize(other.capacity);
            for (int i = 0; i < other.current_size; ++i) {
                buffer[i] = other.buffer[i];
            }
            current_size = other.current_size;
            buffer[current_size] = '\0';
        }
    }

};
class TextEditor {
private:
    TextContainer* text_array;
    int line_count ;
    int capacity;
    char* clipboard;
    std::stack<TextContainer*> undo_stack;
    std::stack<TextContainer*> redo_stack;
    int cursorLine;
    int cursorIndex;

    void freeMemory() {
        if (text_array != nullptr) {
            delete[] text_array;
            text_array = nullptr;
        }
        if (clipboard != nullptr) {
            delete[] clipboard;
            clipboard = nullptr;
        }
        line_count = 0;
        capacity = 0;
    }

    void saveState() {
        TextContainer* state = new TextContainer[line_count];
        if (text_array) {
            for (int i = 0; i < line_count; i++) {
                state[i] = text_array[i];
            }
        }
        undo_stack.push(state);
    }

    void clearRedoStack() {
        while (!redo_stack.empty()) {
            TextContainer* old_state = redo_stack.top();
            delete[] old_state;
            redo_stack.pop();
        }
    }

    void pushToUndoStack() {
        TextContainer* current_state = new TextContainer[line_count];
        for (int i = 0; i < line_count; i++) {
            current_state[i].append(text_array[i].getBuffer());
        }
        undo_stack.push(current_state);
    }

    void pushToRedoStack() {
        TextContainer* current_state = new TextContainer[line_count];
        for (int i = 0; i < line_count; i++) {
            current_state[i].append(text_array[i].getBuffer());
        }
        redo_stack.push(current_state);
    }

public:
    TextEditor() {
        text_array = nullptr;
        line_count = 0;
        capacity =INITIAL_CAPACITY;
        clipboard = nullptr;
        cursorLine = 0;
        cursorIndex = 0;

    }

    ~TextEditor() {
        while (!undo_stack.empty()) {
            delete[] undo_stack.top();
            undo_stack.pop();
        }
        freeMemory();
    }

    static void printHelp(){
        printf("Commands: \n");
        printf("1 - append <text> - append text to the end \n");
        printf("2 - start the new line \n");
        printf("3 - save <filename> - use files to save the information \n");
        printf("4 - load <filename> - use files to load the information \n");
        printf("5 - print the current text to console \n");
        printf("6 - insert the text by line and symbol index \n");
        printf("7 - search <word> \n");
        printf("8 - delete symbol by line and index \n");
        printf("9 - insert text with replacemenet \n");
        printf("10 - cut text by line and index \n");
        printf("11 - copy text by line and index \n");
        printf("12 - paste text by line and index \n");
        printf("13 - undo \n");
        printf("14 - redo \n");
        printf("15 - move cursor up\n");
        printf("16 - move cursor down\n");
        printf("17 - move cursor left\n");
        printf("18 - move cursor right\n");
        printf("19 - exit the program\n");
    }

    void init() {
        text_array = new TextContainer[INITIAL_CAPACITY];
        capacity = INITIAL_CAPACITY;
    }

    void resize(int new_capacity) {
        TextContainer* new_array = new TextContainer[new_capacity];
        for ( int i = 0; i < line_count; i++) {
            new_array[i] = text_array[i];
        }
        delete[] text_array;
        text_array = new_array;
        capacity = new_capacity;
    }

    void appendText(const char* text_to_append) {
        if ( line_count >= MAX_LINES) {
            printf("Error: Maximum of lines reached. \n");
            return;
        }
        if (line_count >= capacity) {
            resize(capacity*2);
        }
        saveState();
        text_array[cursorLine].insert(cursorIndex, text_to_append);
        cursorIndex += TextContainer::myStrlen(text_to_append);
    }


    void saveToFile(const char* filename) {
        FILE* file = fopen(filename, "w");
        if (file == nullptr) {
            printf(">Unable to open file for writing.\n");
            return;
        }
        for (int i = 0; i < line_count; i++) {
            fprintf(file, "%s\n", text_array[i].getBuffer());
        }
        fclose(file);
        printf(">Text has been saved successfully");
    }

    void loadFromFile(char* filename) {
        FILE* file = fopen(filename, "r");
        if (file == nullptr) {
            printf(">Unable to open file for reading.\n");
            return;
        }
        char buffer[MAX_LINE_LENGTH];
        freeMemory();
        init();
        line_count = 0;
        while (fgets(buffer, MAX_LINE_LENGTH, file) != nullptr) {
            int len = 0;
            while ( buffer[len] != '\n' && buffer[len] != '\0') {
                len++;
            }
            buffer[len] = '\0';
            appendText(buffer);
        }
        fclose(file);
        printText();
    }

    void printText() {
        if (line_count == 0) {
            printf(">Text container is empty.\n");
            return;
        }
        printf(">Current text:\n");
        for (int i = 0; i < line_count; i++) {
            printf("%s\n", text_array[i].getBuffer());
        }
        printf("Cursor at line %d, index %d\n", cursorLine + 1, cursorIndex);
    }

    void insertText(int line, int index, const char* text_to_insert) {
        if ( line >= line_count || line < 0) {
            printf("Error: Invalid line number. \n");
            return;
        }
        saveState();
        text_array[cursorLine].insert(cursorIndex, text_to_insert);
        cursorIndex += TextContainer::myStrlen(text_to_insert);
    }

    void search_word(char* word) {
        int found_count = 0;
        for (int i = 0; i < line_count; i++) {
            char* buffer = text_array[i].getBuffer();
            char* found = buffer;
            int word_length = 0;
            while (word[word_length] != '\0') {
                word_length++;
            }
            while (*found != '\0') {
                char* temp = found;
                int j = 0;
                while (temp[j] == word[j] && temp[j] != '\0' && word[j] != '\0') {
                    j++;
                }
                if (j == word_length) {
                    int word_index = found - buffer;
                    printf(">Found '%s' at line %d, index %d\n", word, i, word_index);
                    found += j;
                    found_count++;
                } else {
                    found++;
                }
            }
        }
        if (found_count == 0) {
            printf(">Word '%s' not found.\n", word);
        }
    }

    void deleteText(int line, int index, int count) {
        if (line >= line_count || line < 0) {
            printf("Error: Invalid line number.\n");
            return;
        }
        saveState();
        text_array[line].deleteText(index, count);
        // text_array[cursorLine].deleteText(cursorIndex, count);
        //cursorIndex -= TextContainer::myStrlen(text_to_append);
    }

    void insertReplacement(int line, int index, const char* text_to_replace) {
        // int line = cursorLine;
        // int index = cursorIndex;

        if (line >= line_count || line < 0) {
            printf("Error: Invalid line number.\n");
            return;
        }
        saveState();
        text_array[line].insertReplacement(index, text_to_replace);
    }

    void cutText(int count) {
        int line = cursorLine;
        int index = cursorIndex;

        if (line >= line_count || line < 0) {
            printf("Error: Invalid line number.\n");
            return;
        }
        char* buffer = text_array[line].getBuffer();
        if (index < 0 || index >= text_array[line].getCurrentSize() || count <= 0) {
            printf("Error: Invalid index or count.\n");
            return;
        }
        if (index + count > text_array[line].getCurrentSize()) {
            count = text_array[line].getCurrentSize() - index;
        }
        delete[] clipboard;
        clipboard = new char[count + 1];
        for (int i = 0; i < count; i++) {
            clipboard[i] = buffer[index + i];
        }
        clipboard[count] = '\0';
        saveState();
        text_array[line].deleteText(index, count);
    }

    void copyText(int line, int index, int count) {
        // int line = cursorLine;
        // int index = cursorIndex;

        if (line >= line_count || line < 0) {
            printf("Error: Invalid line number.\n");
            return;
        }
        char* buffer = text_array[line].getBuffer();
        if (index < 0 || index >= text_array[line].getCurrentSize() || count <= 0) {
            printf("Error: Invalid index or count.\n");
            return;
        }
        if (index + count > text_array[line].getCurrentSize()) {
            count = text_array[line].getCurrentSize() - index;
        }
        delete[] clipboard;
        clipboard = new char[count + 1];
        for (int i = 0; i < count; i++) {
            clipboard[i] = buffer[index + i];
        }
        clipboard[count] = '\0';
    }

    void pasteText(int line, int index) {
        // int line = cursorLine;
        // int index = cursorIndex;

        if (line >= line_count || line < 0) {
            printf("Error: Invalid line number.\n");
            return;
        }
        if (clipboard == nullptr) {
            printf("Clipboard is empty.\n");
            return;
        }
        if (index < 0 || index > text_array[line].getCurrentSize()) {
            printf("Error: Invalid index.\n");
            return;
        }
        saveState();
        text_array[line].insert(index, clipboard);
    }

    void undo() {
        if (undo_stack.empty()) {
            printf("No steps to undo.\n");
            return;
        }

        pushToRedoStack();

        TextContainer* previous_state = undo_stack.top();
        undo_stack.pop();

        delete[] text_array;

        text_array = previous_state;

        line_count = 0;
        while (line_count < capacity && text_array[line_count].getBuffer() != nullptr) {
            line_count++;
        }

        printf("Undo successful. Restored to the previous state.\n");
    }


    void redo() {
        if (redo_stack.empty()) {
            printf("No steps to redo.\n");
            return;
        }

        pushToUndoStack();

        TextContainer* state_to_redo = redo_stack.top();
        redo_stack.pop();

        delete[] text_array;

        text_array = state_to_redo;

        line_count = 0;
        while (line_count < capacity && text_array[line_count].getBuffer() != nullptr) {
            line_count++;
        }

        printf("Redo successful. Restored to the previous state.\n");
    }

    void moveCursorUp() {
        if (cursorLine > 0) {
            cursorLine--;
            if (cursorIndex > text_array[cursorLine].getCurrentSize()) {
                cursorIndex = text_array[cursorLine].getCurrentSize();
            }
        }
    }

    void moveCursorDown() {
            if (cursorLine < line_count - 1) {
                cursorLine++;
                // Перевіряємо, щоб курсор не виходив за межі нового рядка
                if (cursorIndex > text_array[cursorLine].getCurrentSize()) {
                    cursorIndex = text_array[cursorLine].getCurrentSize();
                }
            }
        }

    void moveCursorLeft() {
        if (cursorIndex > 0) {
            cursorIndex--;
        } else if (cursorLine > 0) {
            cursorLine--;
            cursorIndex = text_array[cursorLine].getCurrentSize();
        }
    }

    void moveCursorRight() {
        if (cursorLine < line_count) {
            if (cursorIndex < text_array[cursorLine].getCurrentSize()) {
                cursorIndex++;
            } else if (cursorLine < line_count - 1) {
                cursorLine++;
                cursorIndex = 0;
            }
        }
    }



    void handleCommand(int command) {
        char* input = nullptr;
        size_t input_size = 0;
        if (command == 1) {
            printf("Enter text to append: ");
            getline(&input, &input_size, stdin);
            int len = 0;
            while (input[len] != '\n' && input[len] != '\0') {
                len++;
            }
            input[len] = '\0';
            appendText(input);
            free(input);
        }
        else if (command == 2) {
            appendText("");
        }
        else if (command == 3) {
            printf("Enter filename to save: ");
            getline(&input, &input_size, stdin);
            int len = 0;
            while (input[len] != '\n' && input[len] != '\0') {
                len++;
            }
            input[len] = '\0';
            saveToFile(input);
            free(input);
        } else if (command == 4) {
            printf("Enter filename to load: ");
            getline(&input, &input_size, stdin);
            int len = 0;
            while (input[len] != '\n' && input[len] != '\0') {
                len++;
            }
            input[len] = '\0';
            loadFromFile(input);
            free(input);
        }
        else if (command == 5) {
            printText();
        } else if (command == 6) {
            printf("Enter line number: ");
            int line;
            scanf("%d", &line);
            printf("Enter index: ");
            int index;
            scanf("%d", &index);
            getchar();
            printf("Enter text to insert: ");
            getline(&input, &input_size, stdin);
            int len = 0;
            while (input[len] != '\n' && input[len] != '\0') {
                len++;
            }
            input[len] = '\0';
            insertText(line, index, input);
            free(input);
        } else if (command == 7) {
            printf("Enter word to search: ");
            getline(&input, &input_size, stdin);
            int len = 0;
            while (input[len] != '\n' && input[len] != '\0') {
                len++;
            }
            input[len] = '\0';
            search_word(input);
            free(input);
        }
        else if (command == 8) {
            printf("Enter line number: ");
            int line;
            scanf("%d", &line);
            printf("Enter start index: ");
            int index;
            scanf("%d", &index);
            printf("Enter number of characters to delete: ");
            int count;
            scanf("%d", &count);
            deleteText(line, index, count);
        }
        else if (command == 9) {
            printf("Enter line number: ");
            int line;
            scanf("%d", &line);
            printf("Enter index: ");
            int index;
            scanf("%d", &index);
            getchar();
            printf("Enter text to insert with replacement: ");
            getline(&input, &input_size, stdin);
            int len = 0;
            while (input[len] != '\n' && input[len] != '\0') {
                len++;
            }
            input[len] = '\0';
            insertReplacement(line, index, input);
            free(input);
        }
        else if (command == 10) {
            // printf("Enter line number: ");
            // int line;
            // scanf("%d", &line);
            // printf("Enter start index: ");
            // int index;
            // scanf("%d", &index);
            printf("Enter number of characters to cut: ");
            int count;
            scanf("%d", &count);
            cutText(count);
        }
        else if (command == 11) {
            printf("Enter line number: ");
            int line;
            scanf("%d", &line);
            printf("Enter start index: ");
            int index;
            scanf("%d", &index);
            printf("Enter number of characters to copy: ");
            int count;
            scanf("%d", &count);
            copyText(line, index, count);
        }
        else if (command == 12) {
            printf("Enter line number: ");
            int line;
            scanf("%d", &line);
            printf("Enter index: ");
            int index;
            scanf("%d", &index);
            getchar();
            pasteText(line, index);
        }
        else if (command == 13) {
            undo();
        }
        else if (command == 14) {
            redo();
        }
        else if (command == 16) {
            moveCursorUp();
        }
        else if (command == 17) {
            moveCursorDown();
        }
        else if (command == 18) {
            moveCursorLeft();
        }
        else if (command == 19) {
            moveCursorRight();
        }
        else if (command == 16) {
            freeMemory();
            exit(0);
        }
        else {
            printf("The command is not implemented.\n");
        }
    }
};

int main() {
    TextEditor editor;
    editor.init();
    editor.printHelp();
    int command;
    while (true) {
        printf("\nChoose the command: \n>");
        if (scanf("%d", &command) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();

        if (command < 1 || command > 19) {
            printf("Invalid command number. Please enter a number between 1 and 10.\n");
            continue;
        }

        if (command == 19) { // If the command is 10, we break the loop to exit
            printf(">Exiting...\n");
            break;
        }

        editor.handleCommand(command);
    }

    return 0;
}

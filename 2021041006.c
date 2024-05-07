//
// Created by samsung on 24. 4. 20.
//
//
// Created by samsung on 24. 4. 20.
//
//
// Created by samsung on 24. 4. 18.
//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)


typedef struct {                //위치 저장하기 위한 구조체
    int x;
    int y;
}Position;

typedef struct {            //스택을 위한 구조체
    Position* elements;
    int top;
    int maxSize;
}Stack;

typedef struct {            //방향 좌표를 저장하기 위한 구조체
    short int vert;
    short int horiz;
}offsets;

offsets move[8] = { {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1} };

// 스택 초기화 함수
void initStack(Stack* stack, int initialSize) {                         //스택 초기화
    stack->elements = (Position*)malloc(initialSize * sizeof(Position));        //공간 모자라면 재할당
    stack->top = -1;
    stack->maxSize = initialSize;
}
// 스택이 꽉 찼는지 확인 후 크기 2배 재할당
int isFull(Stack* stack) {

    return stack->top == stack->maxSize - 1;

}

// 스택이 비었는지 확인
int isEmpty(Stack* stack) {
    return stack->top == -1;
}
// 스택에 요소 추가

void push(Stack* stack, Position pos) {
    if (isFull(stack)) {
        // 스택이 꽉 찼을 때 크기를 2배로 확장
        stack->maxSize *= 2;
        stack->elements = (Position*)realloc(stack->elements, stack->maxSize * sizeof(Position));
    }
    if (stack->elements == NULL) {
        return; // 메모리 재할당 실패 시 함수 종료
    }
    stack->elements[++stack->top] = pos;        //top +1한 곳에 값 입력
}

// 스택에서 요소 제거
Position pop(Stack* stack) {
    if (!isEmpty(stack)) {
        return stack->elements[stack->top--];                       //top -1 -> 나가는 값 반환
    }
}

// 스택 해제
void freeStack(Stack* stack) {
    free(stack->elements);
    stack->elements = NULL;
    stack->top = -1;
    stack->maxSize = 0;
}

int read_maze(const char* filename, int* rows, int* cols, char*** maze) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("파일을 열 수 없습니다");
        *rows = 0;
        *cols = 0;
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        perror("메모리 할당 실패");
        fclose(file);
        *rows = 0;
        *cols = 0;
        return -1;
    }

    fread(buffer, fileSize, 1, file);
    buffer[fileSize] = '\0';

    *rows = 0;
    char* line = strtok(buffer, "\n");
    if (line != NULL) {
        *cols = strlen(line);
        (*rows)++;
    }
    while ((line = strtok(NULL, "\n")) != NULL) {
        (*rows)++;
    }

    // 2차원 배열로 메모리 할당
    *maze = (char**)malloc(*rows * sizeof(char*));
    for (int i = 0; i < *rows; i++) {
        (*maze)[i] = (char*)malloc(*cols + 1);
    }

    rewind(file);
    int row = 0;
    while (fgets((*maze)[row], *cols + 2, file) != NULL) {
        (*maze)[row][*cols] = '\0';  // null-termination 보장
        row++;
    }

    fclose(file);
    free(buffer);

    // 처음 미로 출력
    printf("<<INPUT MAZE>>\n");
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            printf("%c", (*maze)[i][j]);
        }
        printf("\n");
    }

    return 0;
}

void add_borders(char*** maze, int* rows, int* cols) {
    int newRows = *rows + 2;
    int newCols = *cols + 2;
    char** newMaze = (char**)malloc(newRows * sizeof(char*));

    if (newMaze == NULL) {
        perror("메모리 할당 실패");
        return; // 메모리 할당 실패시 함수 종료
    }

    for (int i = 0; i < newRows; i++) {
        newMaze[i] = (char*)malloc(newCols + 1);
        if (newMaze[i] == NULL) {
            perror("메모리 할당 실패");
            // 이전에 할당된 메모리 정리
            for (int j = 0; j < i; j++) {
                free(newMaze[j]);
            }
            free(newMaze);
            return; // 메모리 할당 실패시 함수 종료
        }

        for (int j = 0; j < newCols; j++) {
            if (i == 0 || i == newRows - 1 || j == 0 || j == newCols - 1) {         //끝부분 1 추가
                newMaze[i][j] = '1';
            }
            else {
                newMaze[i][j] = (*maze)[i - 1][j - 1];
            }
        }
        newMaze[i][newCols] = '\0';         //행 넘기기
    }


    // 새 미로로 업데이트
    *maze = newMaze;
    *rows = newRows;
    *cols = newCols;
}

void printPath(char** maze, Stack* stack,int rows,int cols) {               //최종 결과값 출력함수
   printf("<<SOLUTION>>\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maze[i][j] == '5') {                                //방문했다 표시한 5인 값들을 옳은 경로로 확인하고 표시
                printf("0");
            } else {
                printf("1");
            }
        }
        printf("\n");
    }
}


void path(char** newMaze, int rows, int cols) {           //경로 찾는 함수
    Stack s;
    initStack(&s, 10);                      //스택 크기 10으로 초기화
    int nextX = 0, nextY = 0;
    Position current = {1, 1};                 //현재위치 1,1로 지정
    Position end = {rows - 2, cols - 2};          //수정된 종료 위치

    bool pathFound = false;                                 //도착 변수
    push(&s, current);                                      //현재위치 스택에 push
    newMaze[current.x][current.y] = '5';                      //이어갈 미로에 처음 위치 방문 표시, 나중에 이 경로만 0으로 바꾸면 됨

    while (!isEmpty(&s) && !pathFound) {
        current = s.elements[s.top];                        // 스택의 맨 위 요소를 확인하지만 pop하지 않음

        if (current.x == end.x && current.y == end.y) {    //도착위치에 가면 반복문 탈출
            pathFound = true;
           //printf("도착");                              //도착 했는지 확인
            break;
        }

        bool moved = false;                                 //이동 가능한지
        for (int i = 0; i < 8; i++) {                               //offset에 저장된 8방향 찾기
            nextX = current.x + move[i].vert;
            nextY = current.y + move[i].horiz;
            if (nextX >= 0 && nextX < rows && nextY >= 0 && nextY < cols && newMaze[nextX][nextY] == '0') {     //이동 가능한 0인 경우
                newMaze[nextX][nextY] = '5';                                                                    //갈 수 있다는 표시 하고
                push(&s, (Position){nextX, nextY});                                                                //다음위치 스택에 입력
                moved = true;                                                                                         //이동 표시
                break;                                                                                  // 새 위치로 이동했으므로 루프 중단
            }
        }

        if (!moved) {
            pop(&s);                                                 // 모든 방향을 확인했지만 이동할 곳이 없으면 현재 위치를 스택에서 제거
        }
    }                                                                   //도착지 찾을때 까지 반복
                 //옳은 경로 제외하고 나머지 1로 바꾸는 함수 호출
    printPath(newMaze,&s,rows,cols  );
    freeStack(&s);
}

int main() {
    const char* filename = "maze.txt";              //파일 입력
    int rows = 0, cols = 0;
    char** maze = NULL;

    if (read_maze(filename, &rows, &cols, &maze) == 0) {            //파일 열었으면 아래 함수들 실행
        add_borders(&maze, &rows, &cols);  // 수정된 호출
        path(maze, rows, cols);  // 경로 찾기 함수 호출 및 결과 출력

        // 메모리 해제
        for (int i = 0; i < rows; i++) {
            free(maze[i]);
        }
        free(maze);
    }
    else {
        printf("미로를 읽는 데 실패했습니다.\n");
        return -1;
    }
    return 0;
}


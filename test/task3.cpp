#pragma warning(disable : 4996)
#include <stdio.h>

int ReadLine(int s[]) {
    int ch = getchar();
    if (ch <= 0) {
        //printf("ch = %d\n", ch);
        return EOF;
    }
    int len = 0;
    while (ch != '\n' && ch > 0) {
        s[len] = ch;
        ch = getchar();
        len = len + 1;
    }
    s[len] = 0;
    return len;
}

void PrintLine(int s[]) {
    int i = 0;
    while (s[i] != 0) {
        putchar(s[i]);
        i = i + 1;
    }
    putchar('\n');
}

void PrintInt(int x) {
    if (x < 0) {
        putchar('-');
        x = -x;
    }
    if (x >= 10) {
        PrintInt(x / 10);
    }
    putchar(x % 10 + '0');
}

void Print1FDiv(int x, int y) {
    int ans = x * 100 / y;
    int Up = (ans + 5) / 100;
    int Down = (ans + 5) % 100;
    //printf("x = %d, y = %d, ans = %d, Up = %d, Down = %d\n", x, y, ans, Up, Down);

    if (Down >= 100) {
        Up = Up + 1;
        Down = Down - 100;
    }

    // 需要进位
    PrintInt(Up);
    putchar('.');
    putchar(Down / 10 + '0');
    
}

//struct Course {
//    int credit;      // 学分
//    int need[8][8];  // 前置课程
//    int cnt;         // 前置课程的种类数
//    int grade;       // 得分等级, -1 表示没有读过
//} a[110];
int credit[110];        // 学分
int need[110][8][8];  // 前置课程
int cnt[110];         // 前置课程的种类数
int grade[110];       // 得分等级, -1 表示没有读过
int course[110][110];
int course_cnt;  // hash表的长度
int Insert(int s[]) {
    int now = 0;
    while (s[now] != 0) {
        course[course_cnt][now] = s[now];
        now = now + 1;
    }
    course_cnt = course_cnt + 1;
    return course_cnt - 1;
}
int GetLen(int s[]) {
    int len = 0;
    while (s[len] != 0) {
        len = len + 1;
    }
    return len;
}
int comp(int s1[], int s2[]) {
    int len1 = GetLen(s1);
    int len2 = GetLen(s2);
    if (len1 != len2) {
        return 0;
    }
    int i = 0;
    while (i < len1) {
        if (s1[i] != s2[i]) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
int Find(int s[]) {
    int i = 0;
    while (i < course_cnt) {
        if (comp(s, course[i])) {
            return i;
        }
        i = i + 1;
    }
    return Insert(s);
}

// 按照输入顺序存储课程
int course_input[110];
// 将当前课程的信息保存下来
void Prework(int line[]) {
    if (GetLen(line) == 0) {
        return;
    }
    // 找当前课程 第1个|之前
    int now[110] = {0};
    int tmp = 0;
    int i = 0;
    while (line[i] != '|') {
        now[0] = now[0] + 1;
        tmp = now[0];
        now[tmp] = line[i];
        i = i + 1;
    }

    now[0] = now[0] + 1;
    tmp = now[0];
    now[tmp] = 0;
    i = i + 1; 


    int index = Find(now + 1);
    course_input[0] = course_input[0] + 1;
    tmp = course_input[0];
    course_input[tmp] = index;

    // 当前课程的学分 1~2个|之间
    credit[index] = line[i] - '0';
    i = i + 2;

    // 当前课程的依赖课程 2~3个|之间
    while (line[i] != 0 && line[i] != '|') {
        // 一组依赖课程
        while (line[i] != 0 && line[i] != ';' && line[i] != '|') {
            // 一门依赖课程
            now[0] = 0;
            while (line[i] != 0 && line[i] != ',' && line[i] != ';' && line[i] != '|') {
                now[0] = now[0] + 1;
                tmp = now[0];
                now[tmp] = line[i];
                i = i + 1;
            }
            now[0] = now[0] + 1;
            tmp = now[0];
            now[tmp] = 0;

            int count = cnt[index];
            need[index][count][0] = need[index][count][0] + 1;
            tmp = need[index][count][0];
            need[index][count][tmp] = Find(now + 1);
            if (line[i] == ';' || line[i] == '|') break;
            i = i + 1;
        }
        cnt[index] = cnt[index] + 1;
        if (line[i] == '|') break;
        i = i + 1;
    }
    if (line[i] != 0) i = i + 1;

    // 当前课程的得分
    if (line[i] == 'A') {
        grade[index] = 4;
    } else if (line[i] == 'B') {
        grade[index] = 3;
    } else if (line[i] == 'C') {
        grade[index] = 2;
    } else if (line[i] == 'D') {
        grade[index] = 1;
    } else if (line[i] == 'F') {
        grade[index] = 0;
    } else {
        grade[index] = -1;
    }
}

// 输出GPA
void PrintGPA() {
    //double sum_credit_f = 0, sum_grade_f = 0;
    int sum_credit = 0;
    int sum_grade = 0;
    int i = 0;
    while (i < course_cnt) {
        if (grade[i] == -1) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        //sum_credit_f += credit[i];
        sum_grade = sum_grade + grade[i] * credit[i];
        //sum_grade_f += grade[i] * credit[i];
        i = i + 1;
    }

   /* if (sum_credit == 0)
        printf("GPA: 0.0\n");
    else
        printf("GPA: %.1f\n", sum_grade / sum_credit);*/

    if (sum_credit == 0) {
        putchar('G');
        putchar('P');
        putchar('A');
        putchar(':');
        putchar(' ');
        putchar('0');
        putchar('.');
        putchar('0');
        putchar('\n');
    } else {
        putchar('G');
        putchar('P');
        putchar('A');
        putchar(':');
        putchar(' ');
        Print1FDiv(sum_grade, sum_credit);
        putchar('\n');
    }
}

// 输出尝试学分
void PrintAttemped() {
    int sum_credit = 0;
    int i = 0; 
    while (i < course_cnt) {
        if (grade[i] == -1) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        i = i + 1;
    }
    putchar('H');
    putchar('o');
    putchar('u');
    putchar('r');
    putchar('s');
    putchar(' ');
    putchar('A');
    putchar('t');
    putchar('t');
    putchar('e');
    putchar('m');
    putchar('p');
    putchar('t');
    putchar('e');
    putchar('d');
    putchar(':');
    putchar(' ');
    PrintInt(sum_credit);
    putchar('\n');
    //printf("Hours Attempted: %d\n", sum_credit);
}

// 输出已修学分
void PrintCompleted() {
    int sum_credit = 0;
    int i = 0; 
    while (i < course_cnt) {
        if (grade[i] <= 0) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        i = i + 1;
    }
    putchar('H');
    putchar('o');
    putchar('u');
    putchar('r');
    putchar('s');
    putchar(' ');
    putchar('C');
    putchar('o');
    putchar('m');
    putchar('p');
    putchar('l');
    putchar('e');
    putchar('t');
    putchar('e');
    putchar('d');
    putchar(':');
    putchar(' ');
    PrintInt(sum_credit);
    putchar('\n');
    //printf("Hours Completed: %d\n", sum_credit);
}

// 输出剩余学分
int credit_remaining;
void PrintRemaining() {
    int sum_credit = 0;
    int i = 0;
    while (i < course_cnt) {
        if (grade[i] > 0) {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        i = i + 1;
    }
    credit_remaining = sum_credit;
    putchar('C');
    putchar('r');
    putchar('e');
    putchar('d');
    putchar('i');
    putchar('t');
    putchar('s');
    putchar(' ');
    putchar('R');
    putchar('e');
    putchar('m');
    putchar('a');
    putchar('i');
    putchar('n');
    putchar('i');
    putchar('n');
    putchar('g');
    putchar(':');
    putchar(' ');
    PrintInt(sum_credit);
    putchar('\n');
    //printf("Credits Remaining: %d\n", sum_credit);
}

// 判断第index个课程是否需要修读
int NeedToLearn(int index) {
    // 已经修读过并且没有挂科
    if (grade[index] > 0) return 0;

    // 没有前置课, 可以直接修读
    if (cnt[index] == 0) return 1;

    // 有前置课, 判断是否满足某一组前置课的要求
    int i = 0; 
    while (i < cnt[index]) {
        // 第i组前置课
        int can_learn = 1;
        int j = 1; 
        while (j <= need[index][i][0]) {
            int need_course = need[index][i][j];
            if (grade[need_course] <= 0) can_learn = 0;
            j = j + 1;
        }
        if (can_learn) return 1;
        i = i + 1;
    }
    return 0;
}

int main() {
     //freopen("./input/task3.1.in", "r", stdin);
    int line[1000];
    //while (scanf("%s", line) != EOF) {
    while (ReadLine(line) != EOF) {
        Prework(line);
        //fprintf(stderr, "%s\n", line);
    }
    PrintGPA();
    PrintAttemped();
    PrintCompleted();
    PrintRemaining();
    
    putchar('\n');
    putchar('P');
    putchar('o');
    putchar('s');
    putchar('s');
    putchar('i');
    putchar('b');
    putchar('l');
    putchar('e');
    putchar(' ');
    putchar('C');
    putchar('o');
    putchar('u');
    putchar('r');
    putchar('s');
    putchar('e');
    putchar('s');
    putchar(' ');
    putchar('t');
    putchar('o');
    putchar(' ');
    putchar('T');
    putchar('a');
    putchar('k');
    putchar('e');
    putchar(' ');
    putchar('N');
    putchar('e');
    putchar('x');
    putchar('t');
    putchar('\n');
    //printf("\nPossible Courses to Take Next\n");
    int i = 1;
    while (i <= course_input[0]) {
        int index = course_input[i];
        if (NeedToLearn(index)) {
            putchar(' ');
            putchar(' ');
            PrintLine(course[index]);
        }
        i = i + 1;
    }
    if (credit_remaining == 0) {
        putchar(' ');
        putchar(' ');
        putchar('N');
        putchar('o');
        putchar('n');
        putchar('e');
        putchar(' ');
        putchar('-');
        putchar(' ');
        putchar('C');
        putchar('o');
        putchar('n');
        putchar('g');
        putchar('r');
        putchar('a');
        putchar('t');
        putchar('u');
        putchar('l');
        putchar('a');
        putchar('t');
        putchar('i');
        putchar('o');
        putchar('n');
        putchar('s');
        putchar('!');
        putchar('\n');
        //printf("  None - Congratulations!\n");
    }
    return 0;
}
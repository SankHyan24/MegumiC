#include <stdio.h>

int ReadLine(int s[])
{
    int ch = getchar();
    if (ch <= 0)
    {
        // printf("ch = %d\n", ch);
        return EOF;
    }
    int len = 0;
    while (ch != '\n' && ch > 0)
    {
        s[len] = ch;
        ch = getchar();
        len = len + 1;
    }
    s[len] = 0;
    return len;
}

void PrintLine(int s[])
{
    int i = 0;
    while (s[i] != 0)
    {
        putchar(s[i]);
        i = i + 1;
    }
    putchar('\n');
}

void PrintInt(int x)
{
    if (x < 0)
    {
        putchar('-');
        x = -x;
    }
    if (x >= 10)
    {
        PrintInt(x / 10);
    }
    putchar(x % 10 + '0');
}

void Print1FDiv(int x, int y)
{
    int ans = x * 100 / y;
    int Up = (ans + 5) / 100;
    int Down = (ans + 5) % 100;
    // printf("x = %d, y = %d, ans = %d, Up = %d, Down = %d\n", x, y, ans, Up, Down);

    if (Down >= 100)
    {
        Up = Up + 1;
        Down = Down - 100;
    }

    // ��Ҫ��λ
    PrintInt(Up);
    putchar('.');
    putchar(Down / 10 + '0');
}

// struct Course {
//     int credit;      // ѧ��
//     int need[8][8];  // ǰ�ÿγ�
//     int cnt;         // ǰ�ÿγ̵�������
//     int grade;       // �÷ֵȼ�, -1 ��ʾû�ж���
// } a[110];
int credit[110];     // ѧ��
int need[110][8][8]; // ǰ�ÿγ�
int cnt[110];        // ǰ�ÿγ̵�������
int grade[110];      // �÷ֵȼ�, -1 ��ʾû�ж���
int course[110][110];
int course_cnt; // hash���ĳ���
int Insert(int s[])
{
    // printf("Insert: ");
    // PrintLine(s);
    int now = 0;
    while (s[now] != 0)
    {
        course[course_cnt][now] = s[now];
        now = now + 1;
    }
    course_cnt = course_cnt + 1;
    return course_cnt - 1;
}
int GetLen(int s[])
{
    int len = 0;
    while (s[len] != 0)
    {
        len = len + 1;
    }
    return len;
}
int comp(int s1[], int s2[])
{
    int len1 = GetLen(s1);
    int len2 = GetLen(s2);
    if (len1 != len2)
    {
        return 0;
    }
    int i = 0;
    while (i < len1)
    {
        if (s1[i] != s2[i])
        {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}
int Find(int s[])
{
    int i = 0;
    while (i < course_cnt)
    {
        if (comp(s, course[i]))
        {
            return i;
        }
        i = i + 1;
    }
    int g = Insert(s);
    // printf("at %d\n", g);
    return g;
}
int putch(int ch)
{
    putchar(ch);
    return ch;
}
// ��������˳��洢�γ�
int course_input[110];
// ����ǰ�γ̵���Ϣ��������
void Prework(int line[])
{
    if (GetLen(line) == 0)
    {
        return;
    }
    // �ҵ�ǰ�γ� ��1��|֮ǰ
    int now[110] = {0};
    int tmp = 0;
    int i = 0;
    while (line[i] != '|')
    {
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

    // ��ǰ�γ̵�ѧ�� 1~2��|֮��
    credit[index] = line[i] - '0';
    i = i + 2;

    // ��ǰ�γ̵������γ� 2~3��|֮��
    while (line[i] != 0 && line[i] != '|')
    {
        // һ�������γ�
        while (line[i] != 0 && line[i] != ';' && line[i] != '|')
        {
            // һ�������γ�
            now[0] = 0;
            while (line[i] != 0 && line[i] != ',' && line[i] != ';' && line[i] != '|')
            {
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
            if (line[i] == ';' || line[i] == '|')
                break;
            i = i + 1;
        }
        cnt[index] = cnt[index] + 1;
        if (line[i] == '|')
            break;
        i = i + 1;
    }
    if (line[i] != 0)
        i = i + 1;

    // ��ǰ�γ̵ĵ÷�
    if (line[i] == 'A')
    {
        grade[index] = 4;
    }
    else if (line[i] == 'B')
    {
        grade[index] = 3;
    }
    else if (line[i] == 'C')
    {
        grade[index] = 2;
    }
    else if (line[i] == 'D')
    {
        grade[index] = 1;
    }
    else if (line[i] == 'F')
    {
        grade[index] = 0;
    }
    else
    {
        grade[index] = -1;
    }
}

// ���GPA
void PrintGPA()
{
    // double sum_credit_f = 0, sum_grade_f = 0;
    int sum_credit = 0;
    int sum_grade = 0;
    int i = 0;
    while (i < course_cnt)
    {
        if (grade[i] == -1)
        {
            i = i + 1;
            continue;
        }
        sum_credit = sum_credit + credit[i];
        // sum_credit_f += credit[i];
        sum_grade = sum_grade + grade[i] * credit[i];
        // sum_grade_f += grade[i] * credit[i];
        i = i + 1;
    }

    /* if (sum_credit == 0)
         printf("GPA: 0.0\n");
     else
         printf("GPA: %.1f\n", sum_grade / sum_credit);*/

    if (sum_credit == 0)
    {
        putchar('G');
        putchar('P');
        putchar('A');
        putchar(':');
        putchar(' ');
        putchar('0');
        putchar('.');
        putchar('0');
        putchar('\n');
    }
    else
    {
        putchar('G');
        putchar('P');
        putchar('A');
        putchar(':');
        putchar(' ');
        Print1FDiv(sum_grade, sum_credit);
        putchar('\n');
    }
}

// �������ѧ��
void PrintAttemped()
{
    int sum_credit = 0;
    int i = 0;
    while (i < course_cnt)
    {
        if (grade[i] == -1)
        {
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
    // printf("Hours Attempted: %d\n", sum_credit);
}

// �������ѧ��
void PrintCompleted()
{
    int sum_credit = 0;
    int i = 0;
    while (i < course_cnt)
    {
        if (grade[i] <= 0)
        {
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
    // printf("Hours Completed: %d\n", sum_credit);
}

// ���ʣ��ѧ��
int credit_remaining;
void PrintRemaining()
{
    int sum_credit = 0;
    int i = 0;
    while (i < course_cnt)
    {
        if (grade[i] > 0)
        {
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
    // printf("Credits Remaining: %d\n", sum_credit);
}

// �жϵ�index���γ��Ƿ���Ҫ�޶�
int NeedToLearn(int index)
{
    // �Ѿ��޶�������û�йҿ�
    if (grade[index] > 0)
        return 0;

    // û��ǰ�ÿ�, ����ֱ���޶�
    if (cnt[index] == 0)
        return 1;

    // ��ǰ�ÿ�, �ж��Ƿ�����ĳһ��ǰ�ÿε�Ҫ��
    int i = 0;
    while (i < cnt[index])
    {
        // ��i��ǰ�ÿ�
        int can_learn = 1;
        int j = 1;
        while (j <= need[index][i][0])
        {
            int need_course = need[index][i][j];
            if (grade[need_course] <= 0)
                can_learn = 0;
            j = j + 1;
        }
        if (can_learn)
            return 1;
        i = i + 1;
    }
    return 0;
}
static int PrintStr(int s[110])
{
    int i = 0;
    while (s[i] != 0)
    {
        putch(s[i]);
        i = i + 1;
    }
    return 0;
}
static int printNeedArray()
{
    int j = 0;
    int i = 1;
    while (i <= course_input[0])
    {
        int index = course_input[i];
        putch(' ');
        putch('{');
        PrintStr(course[index]);
        putch('}');
        putch(':');
        PrintInt(cnt[index]);
        putch(10);
        j = 0;
        while (j < cnt[index])
        {
            PrintInt(need[index][j][0]);
            putch(' ');
            putch(' ');
            putch(':');
            int k = 1;
            while (k <= need[index][j][0])
            {
                PrintStr(course[need[index][j][k]]);
                putch(' ');
                k = k + 1;
            }
            j = j + 1;
            putch(10);
        }
        putch(10);
        i = i + 1;
    }
    return 0;
}
int main()
{
    // freopen("./input/task3.1.in", "r", stdin);
    int line[1000];
    // while (scanf("%s", line) != EOF) {
    while (ReadLine(line) != EOF)
    {
        Prework(line);
        // fprintf(stderr, "%s\n", line);
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
    // printf("\nPossible Courses to Take Next\n");
    // int i = 1;
    // while (i <= course_input[0])
    // {
    //     int index = course_input[i];
    //     if (NeedToLearn(index))
    //     {
    //         putchar(' ');
    //         putchar(' ');
    //         // putchar('[');
    //         // PrintInt(index);
    //         // putchar(']');
    //         PrintLine(course[index]);
    //     }
    //     i = i + 1;
    // }
    printNeedArray();
    if (credit_remaining == 0)
    {
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
        // printf("  None - Congratulations!\n");
    }
    return 0;
}
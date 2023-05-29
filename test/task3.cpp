
function int ReadLine(int s[100])
{
    int ch = getch();
    if (ch <= 0)
    {
        // printf("ch = %d\n", ch);
        return -1;
    }
    int len = 0;
    while (ch != 10 && ch > 0)
    {
        s[len] = ch;
        ch = getch();
        len = len + 1;
    }
    s[len] = 0;
    return len;
}

function int PrintLine(int s[100])
{
    int i = 0;
    while (s[i] != 0)
    {
        putch(s[i]);
        i = i + 1;
    }
    putch(10);
}

function int PrintInt(int x)
{
    if (x < 0)
    {
        putch('-');
        x = -x;
    }
    if (x >= 10)
    {
        PrintInt(x / 10);
    }
    putch(x % 10 + '0');
}

function int Print1FDiv(int x, int y)
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

    PrintInt(Up);
    putch('.');
    putch(Down / 10 + '0');
}

int credit[150];
int need[150][8][8];
int cnt[150];
int grade[150];
int course1[150][150];
int course[150][150];
int course3[150][150];
int course_cnt;
function int Insert(int s[1000])
{
    int now = 0;
    while (s[now] != 0)
    {
        course[course_cnt][now] = s[now];
        now = now + 1;
    }
    course_cnt = course_cnt + 1;
    return course_cnt - 1;
}
function int GetLen(int s[1000])
{
    int len = 0;
    while (s[len] != 0)
    {
        len = len + 1;
    }
    return len;
}
function int comp(int s1[1000], int s2[1000])
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
function int Find(int s[1000])
{
    int i = 0;
    while (i < course_cnt)
    {
        if (comp(s, course + i * 150))
        {
            return i;
        }
        i = i + 1;
    }
    return Insert(s);
}
int course_input[150];
function int Prework(int line[1000])
{
    if (GetLen(line) == 0)
    {
        return 0;
    }
    int now[150] = {0};
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

    int index = Find(now + 4);
    course_input[0] = course_input[0] + 1;
    tmp = course_input[0];
    course_input[tmp] = index;

    credit[index] = line[i] - '0';
    i = i + 2;

    while (line[i] != 0 && line[i] != '|')
    {
        while (line[i] != 0 && line[i] != ';' && line[i] != '|')
        {
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
            need[index][count][tmp] = Find(now + 4);
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

function int PrintGPA()
{
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
        sum_grade = sum_grade + grade[i] * credit[i];
        i = i + 1;
    }

    if (sum_credit == 0)
    {
        putch('G');
        putch('P');
        putch('A');
        putch(':');
        putch(' ');
        putch('0');
        putch('.');
        putch('0');
        putch(10);
    }
    else
    {
        putch('G');
        putch('P');
        putch('A');
        putch(':');
        putch(' ');
        Print1FDiv(sum_grade, sum_credit);
        putch(10);
    }
}

function int PrintAttemped()
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
    putch('H');
    putch('o');
    putch('u');
    putch('r');
    putch('s');
    putch(' ');
    putch('A');
    putch('t');
    putch('t');
    putch('e');
    putch('m');
    putch('p');
    putch('t');
    putch('e');
    putch('d');
    putch(':');
    putch(' ');
    PrintInt(sum_credit);
    putch(10);
}

function int PrintCompleted()
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
    putch('H');
    putch('o');
    putch('u');
    putch('r');
    putch('s');
    putch(' ');
    putch('C');
    putch('o');
    putch('m');
    putch('p');
    putch('l');
    putch('e');
    putch('t');
    putch('e');
    putch('d');
    putch(':');
    putch(' ');
    PrintInt(sum_credit);
    putch(10);
}

int credit_remaining;
function int PrintRemaining()
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
    putch('C');
    putch('r');
    putch('e');
    putch('d');
    putch('i');
    putch('t');
    putch('s');
    putch(' ');
    putch('R');
    putch('e');
    putch('m');
    putch('a');
    putch('i');
    putch('n');
    putch('i');
    putch('n');
    putch('g');
    putch(':');
    putch(' ');
    PrintInt(sum_credit);
    putch(10);
}

function int NeedToLearn(int index)
{
    if (grade[index] > 0)
        return 0;

    if (cnt[index] == 0)
        return 1;

    int i = 0;
    while (i < cnt[index])
    {
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

function int main()
{
    int line[1000];
    while (ReadLine(line) != -1)
    {
        Prework(line);
    }
    PrintGPA();
    PrintAttemped();
    PrintCompleted();
    PrintRemaining();

    putch(10);
    putch('P');
    putch('o');
    putch('s');
    putch('s');
    putch('i');
    putch('b');
    putch('l');
    putch('e');
    putch(' ');
    putch('C');
    putch('o');
    putch('u');
    putch('r');
    putch('s');
    putch('e');
    putch('s');
    putch(' ');
    putch('t');
    putch('o');
    putch(' ');
    putch('T');
    putch('a');
    putch('k');
    putch('e');
    putch(' ');
    putch('N');
    putch('e');
    putch('x');
    putch('t');
    putch(10);
    int i = 1;
    while (i <= course_input[0])
    {
        int index = course_input[i];
        if (NeedToLearn(index))
        {
            putch(' ');
            putch(' ');
            // PrintInt(index);
            PrintLine(course + (index)*150 * 4);
        }
        i = i + 1;
    }
    if (credit_remaining == 0)
    {
        putch(' ');
        putch(' ');
        putch('N');
        putch('o');
        putch('n');
        putch('e');
        putch(' ');
        putch('-');
        putch(' ');
        putch('C');
        putch('o');
        putch('n');
        putch('g');
        putch('r');
        putch('a');
        putch('t');
        putch('u');
        putch('l');
        putch('a');
        putch('t');
        putch('i');
        putch('o');
        putch('n');
        putch('s');
        putch('!');
        putch(10);
    }
    return 0;
}
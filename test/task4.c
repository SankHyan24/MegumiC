

const int maxn = 10010;
int n, aa[10010];

function int ReadInt()
{
    int ch = getch();
    while ((ch > 57 || ch < 48) && ch != 45) // 9 0 -
    {
        ch = getch();
    }
    int ans = 0;
    int flag = 1;
    if (ch == 45)
    {
        flag = -1;
        ch = getch();
    }
    while (ch >= 48 && ch <= 57)
    {
        ans = ans * 10 + ch - 48;
        ch = getch();
    }
    return ans * flag;
}

function int PrintInt(int x)
{
    if (x < 0)
    {
        putch(45); // '-'
        x = -x;
    }
    if (x >= 10)
    {
        PrintInt(x / 10);
    }
    putch(x % 10 + 48); // '0'
}

function int Median3(int a[10010], int Left, int Right)
{
    int Center = (Left + Right) / 2;
    int temp = 0;

    if (a[Left] > a[Center])
    {
        temp = a[Left];
        a[Left] = a[Center];
        a[Center] = temp;
    }
    if (a[Left] > a[Right])
    {
        temp = a[Left];
        a[Left] = a[Right];
        a[Right] = temp;
    }
    if (a[Center] > a[Right])
    {
        temp = a[Center];
        a[Center] = a[Right];
        a[Right] = temp;
    }

    temp = a[Center];
    a[Center] = a[Right - 1];
    a[Right - 1] = temp;

    return a[Right - 1];
}

function int Quicksort(int a[10010], int Left, int Right)
{
    if (Left >= Right)
    {
        return 0;
    }
    if (Right - Left + 1 == 2)
    {
        if (a[Left] > a[Right])
        {
            int temp;
            temp = a[Left];
            a[Left] = a[Right];
            a[Right] = temp;
        }
        return 0;
    }
    int L = Left;
    int R = Right;
    int Pivot;
    Pivot = Median3(a, L, R);
    int i = Left;
    int j = Right - 1;
    while (1)
    {
        i = i + 1;
        while (a[i] < Pivot)
        {
            i = i + 1;
        }
        j = j - 1;
        while (a[j] > Pivot)
        {
            j = j - 1;
        }
        if (i < j)
        {
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
        else
        {
            break;
        }
    }

    int temp;
    temp = a[i];
    a[i] = a[Right - 1];
    a[Right - 1] = temp;
    Quicksort(a, Left, i - 1);
    Quicksort(a, i + 1, Right);
    return 0;
}

function int swap(int a[10010], int i, int j)
{
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
    return a[j];
}

function int at(int a[10010], int i)
{
    return a[i];
}

function int PrintArr(int a[10010], int n)
{
    int i = 1;
    while (i <= n)
    {
        PrintInt(a[i]);
        i = i + 1;
        putch(32);
        if (i % 10 == 0)
        {
            putch(10);
        }
    }
    return 0;
}

function int main()
{
    putch('n');
    n = ReadInt();
    int i = 1;
    while (i <= n)
    {
        aa[i] = ReadInt();
        i = i + 1;
    }
    Quicksort(aa, 1, n);
    i = 1;
    while (i <= n)
    {
        PrintInt(aa[i]);
        i = i + 1;
        putch(10);
    }
    return 0;
}
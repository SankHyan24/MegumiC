

const int maxn = 10010;
int n, a[10010];

function int ReadInt()
{
    int ch = getch();
    while ((ch > 57 || ch < 48) && ch != 45) // 9 0 -
    {
        ch = getch();
    }
    int ans = 0;
    int flag = 1;

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

function int sc()
{
    return 123;
}
function int main()
{

    n = ReadInt();
    int i = 1;
    while (i <= n)
    {
        a[i] = ReadInt();
        i = i + 1;
    }
    i = 1;
    while (i <= n)
    {
        PrintInt(a[i]);
        i = i + 1;
        putch(10); // \n
    }
    return 0;
}
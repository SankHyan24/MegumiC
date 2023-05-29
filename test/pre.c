static int ReadInt()
{
    int ch = getch();
    while ((ch > '9' || ch < '0') && ch != '-')
    {
        ch = getch();
    }
    int ans = 0;
    int flag = 1;
    if (ch == '-')
    {
        flag = -1;
        ch = getch();
    }
    while (ch >= '0' && ch <= '9')
    {
        ans = ans * 10 + ch - '0';
        ch = getch();
    }
    return ans * flag;
}

static int GetIntLength(int x)
{
    int ans = 1;
    if (x < 0)
    {
        x = -x;
        ans = ans + 1;
    }
    if (x >= 10)
    {
        ans = ans + GetIntLength(x / 10);
    }
    return ans;
}

static int PrintInt(int x)
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
